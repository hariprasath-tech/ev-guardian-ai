"use client";

import { useEffect, useRef, useState } from "react";

const API_BASE = "http://localhost:8000";
const WS_URL = "ws://localhost:8000/ws/telemetry";

interface Telemetry {
  battery_temp: number;
  battery_voltage: number;
  battery_current: number;
  co2_ppm: number;
  smoke_density: number;
  battery_health: number;
  cycle_count: number;
  runaway_risk: number;
  thermal_gradient: number;
  latitude: number;
  longitude: number;
  system_armed: boolean;
  sensitivity: number;
  emergency_active: boolean;
  mode: string;
  timestamp: number;
  device_id?: string;
  mac_address?: string;
  firmware_version?: string;
}

const defaultTelemetry: Telemetry = {
  battery_temp: 31.0,
  battery_voltage: 398.4,
  battery_current: 12.5,
  co2_ppm: 412.0,
  smoke_density: 0.02,
  battery_health: 98.0,
  cycle_count: 342,
  runaway_risk: 1.8,
  thermal_gradient: 6.0,
  latitude: 37.7749,
  longitude: -122.4194,
  system_armed: true,
  sensitivity: 0.60,
  emergency_active: false,
  mode: "prototype",
  timestamp: Date.now() / 1000,
  device_id: "SC-ESP32-04A2",
  mac_address: "A4:CF:12:8B:3E:9D",
  firmware_version: "v2.3.1",
};

type Screen = "overview" | "gas" | "emergency" | "suppression" | "esp32" | "maps" | "settings";

export default function SafeCellApp() {
  const [screen, setScreen] = useState<Screen>("overview");
  const [telemetry, setTelemetry] = useState<Telemetry>(defaultTelemetry);
  const [telemetryHistory, setTelemetryHistory] = useState<Telemetry[]>([]);
  const [connected, setConnected] = useState(false);
  const [lastUpdate, setLastUpdate] = useState("Waiting for data...");
  const wsRef = useRef<WebSocket | null>(null);

  // WebSocket Connection
  useEffect(() => {
    let ws: WebSocket;
    let retryTimeout: ReturnType<typeof setTimeout>;

    function connect() {
      ws = new WebSocket(WS_URL);
      wsRef.current = ws;

      ws.onopen = () => {
        setConnected(true);
        setLastUpdate("Connected");
      };

      ws.onmessage = (e) => {
        try {
          const data: Telemetry = JSON.parse(e.data);
          setTelemetry(data);
          setLastUpdate(new Date().toLocaleTimeString());
          setTelemetryHistory((prev) => [...prev.slice(-29), data]);
        } catch {}
      };

      ws.onclose = () => {
        setConnected(false);
        retryTimeout = setTimeout(connect, 3000);
      };

      ws.onerror = () => {
        ws.close();
      };
    }

    connect();
    return () => {
      clearTimeout(retryTimeout);
      ws?.close();
    };
  }, []);

  const isAlert = telemetry.battery_temp > 60 || telemetry.co2_ppm > 2000 || telemetry.smoke_density > 0.5 || telemetry.emergency_active;
  const tempPct = Math.min(100, (telemetry.battery_temp / 90) * 100);
  const co2Pct = Math.min(100, (telemetry.co2_ppm / 3000) * 100);
  const riskColor = telemetry.runaway_risk > 50 ? "#E0473E" : telemetry.runaway_risk > 20 ? "#FFB300" : "#00C853";

  async function toggleArm() {
    try {
      await fetch(`${API_BASE}/api/suppression/config`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ system_armed: !telemetry.system_armed }),
      });
      setTelemetry((prev) => ({ ...prev, system_armed: !prev.system_armed }));
    } catch {}
  }

  async function setMode(newMode: string) {
    try {
      await fetch(`${API_BASE}/api/mode`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ mode: newMode }),
      });
      setTelemetry((prev) => ({ ...prev, mode: newMode }));
    } catch {}
  }

  async function triggerEmergency() {
    try {
      await fetch(`${API_BASE}/api/emergency/trigger`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ active: !telemetry.emergency_active }),
      });
      setTelemetry((prev) => ({ ...prev, emergency_active: !prev.emergency_active }));
    } catch {}
  }

  // SVG Chart path calculation
  const historyData = telemetryHistory.length > 0 ? telemetryHistory : Array(20).fill(defaultTelemetry);
  const chartWidth = 700;
  const chartHeight = 160;
  const maxTemp = 90;
  const minTemp = 10;

  const points = historyData.map((d, i) => {
    const x = (i / (historyData.length - 1)) * chartWidth;
    const y = chartHeight - ((d.battery_temp - minTemp) / (maxTemp - minTemp)) * chartHeight;
    return `${x},${Math.max(10, Math.min(chartHeight - 10, y))}`;
  }).join(" ");

  return (
    <div id="app">
      {/* Background Ambient Glows */}
      <div className="bg-orb bg-orb-1" />
      <div className="bg-orb bg-orb-2" />
      {isAlert && <div className="bg-orb bg-orb-alert" />}

      {/* ── Left Desktop Sidebar ── */}
      <aside className="desktop-sidebar">
        <div className="brand-header">
          <div className="brand-logo-wrap">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.2" strokeLinecap="round" strokeLinejoin="round">
              <path d="M12 2L2 7l10 5 10-5-10-5zM2 17l10 5 10-5M2 12l10 5 10-5" />
            </svg>
          </div>
          <div>
            <div className="brand-name">SafeCell AI</div>
            <div className="brand-subtitle">EV Safety Platform</div>
          </div>
        </div>

        <nav className="sidebar-nav">
          <button className={`nav-btn ${screen === "overview" ? "active" : ""}`} onClick={() => setScreen("overview")}>
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
              <rect x="3" y="3" width="7" height="7" rx="1" /><rect x="14" y="3" width="7" height="7" rx="1" /><rect x="14" y="14" width="7" height="7" rx="1" /><rect x="3" y="14" width="7" height="7" rx="1" />
            </svg>
            <span>Dashboard Overview</span>
          </button>

          <button className={`nav-btn ${screen === "gas" ? "active" : ""}`} onClick={() => setScreen("gas")}>
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
              <path d="M12 2v20M17 5H9.5a3.5 3.5 0 0 0 0 7h5a3.5 3.5 0 0 1 0 7H6" />
            </svg>
            <span>Gas & Cabin Safety</span>
          </button>

          <button className={`nav-btn ${screen === "emergency" ? "active" : ""}`} onClick={() => setScreen("emergency")}>
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
              <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z" /><line x1="12" y1="9" x2="12" y2="13" /><line x1="12" y1="17" x2="12.01" y2="17" />
            </svg>
            <span>SOS Emergency</span>
          </button>

          <button className={`nav-btn ${screen === "suppression" ? "active" : ""}`} onClick={() => setScreen("suppression")}>
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
              <path d="M12 2v4M12 18v4M4.93 4.93l2.83 2.83M16.24 16.24l2.83 2.83M2 12h4M18 12h4M4.93 19.07l2.83-2.83M16.24 7.76l2.83-2.83" />
            </svg>
            <span>Fire Suppression</span>
          </button>

          <button className={`nav-btn ${screen === "esp32" ? "active" : ""}`} onClick={() => setScreen("esp32")}>
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
              <rect x="4" y="4" width="16" height="16" rx="2" /><rect x="9" y="9" width="6" height="6" /><line x1="9" y1="1" x2="9" y2="4" /><line x1="15" y1="1" x2="15" y2="4" /><line x1="9" y1="20" x2="9" y2="23" /><line x1="15" y1="20" x2="15" y2="23" /><line x1="20" y1="9" x2="23" y2="9" /><line x1="20" y1="15" x2="23" y2="15" /><line x1="1" y1="9" x2="4" y2="9" /><line x1="1" y1="15" x2="4" y2="15" />
            </svg>
            <span>ESP32 Hardware</span>
          </button>

          <button className={`nav-btn ${screen === "maps" ? "active" : ""}`} onClick={() => setScreen("maps")}>
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
              <polygon points="1 6 1 22 8 18 16 22 23 18 23 2 16 6 8 2 1 6" /><line x1="8" y1="2" x2="8" y2="18" /><line x1="16" y1="6" x2="16" y2="22" />
            </svg>
            <span>Live GPS & Routing</span>
          </button>

          <button className={`nav-btn ${screen === "settings" ? "active" : ""}`} onClick={() => setScreen("settings")}>
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
              <circle cx="12" cy="12" r="3" /><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z" />
            </svg>
            <span>Settings</span>
          </button>
        </nav>

        <div className="sidebar-footer-card">
          <div className="status-pill-row">
            <div className="status-indicator">
              <span className={`status-dot ${isAlert ? "alert" : connected ? "" : "offline"}`} />
              <span>{connected ? "Live WebSockets" : "Offline"}</span>
            </div>
            <span style={{ fontSize: 11, color: "var(--text-dim)" }}>{telemetry.mode.toUpperCase()}</span>
          </div>
          <div style={{ fontSize: 11, color: "var(--text-muted)", display: "flex", justifyContent: "space-between" }}>
            <span>ESP32 Device</span>
            <span style={{ fontFamily: "monospace", color: "#FFF" }}>{telemetry.device_id}</span>
          </div>
        </div>
      </aside>

      {/* ── Main Content Wrapper ── */}
      <div className="main-wrapper">
        {/* Top Header Bar */}
        <header className="top-header">
          <div className="header-left">
            <h2 className="page-title">
              {screen === "overview" && "SafeCell X1 — EV Battery Dashboard"}
              {screen === "gas" && "Cabin Air Quality & Telemetry"}
              {screen === "emergency" && "SOS Emergency & Thermal Control"}
              {screen === "suppression" && "Aerosol Fire Suppression System"}
              {screen === "esp32" && "ESP32 Hardware Stream Manager"}
              {screen === "maps" && "Live GPS Route Guidance"}
              {screen === "settings" && "System Settings & User Profile"}
            </h2>
            <span className="header-tag">{isAlert ? "⚠ CRITICAL ALERT" : "● SYSTEM NORMAL"}</span>
          </div>

          <div className="header-right">
            {/* Mode Switcher */}
            <div className="mode-toggle-group">
              <button
                className={`mode-btn ${telemetry.mode === "prototype" ? "active" : ""}`}
                onClick={() => setMode("prototype")}
              >
                PROTOTYPE
              </button>
              <button
                className={`mode-btn ${telemetry.mode === "hardware" ? "active" : ""}`}
                onClick={() => setMode("hardware")}
              >
                HARDWARE
              </button>
            </div>

            {/* Emergency Panic Action Button */}
            <button className="emergency-panic-btn" onClick={triggerEmergency}>
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.5">
                <polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2" />
              </svg>
              <span>{telemetry.emergency_active ? "CANCEL EMERGENCY" : "TRIGGER SOS"}</span>
            </button>
          </div>
        </header>

        {/* Scrollable Dashboard View */}
        <main className="content-area">
          {/* OVERVIEW TAB */}
          {screen === "overview" && (
            <div className="dashboard-grid">
              {/* Hero Vehicle Overview Banner */}
              <div className="glass-card col-span-8 hero-vehicle-card">
                <div className="hero-vehicle-info">
                  <h1 className="vehicle-model-title">SafeCell X1 Autonomous EV</h1>
                  <div className="vehicle-badge-row">
                    <span className="v-badge">398.4V Li-ion Battery</span>
                    <span className="v-badge" style={{ color: "var(--mint)", borderColor: "rgba(0,200,83,0.3)" }}>
                      Health {telemetry.battery_health}%
                    </span>
                    <span className="v-badge">{telemetry.cycle_count} Cycles</span>
                  </div>
                  <div style={{ display: "flex", gap: 24, marginTop: 12 }}>
                    <div>
                      <div style={{ fontSize: 12, color: "var(--text-muted)" }}>Pack Temperature</div>
                      <div style={{ fontSize: 24, fontWeight: 700, color: telemetry.battery_temp > 60 ? "var(--alert-red)" : "var(--mint)" }}>
                        {telemetry.battery_temp.toFixed(1)}°C
                      </div>
                    </div>
                    <div>
                      <div style={{ fontSize: 12, color: "var(--text-muted)" }}>Thermal Gradient</div>
                      <div style={{ fontSize: 24, fontWeight: 700, color: "#FFF" }}>
                        {telemetry.thermal_gradient.toFixed(1)}°C/m
                      </div>
                    </div>
                  </div>
                </div>
                {/* eslint-disable-next-line @next/next/no-img-element */}
                <img
                  className="hero-vehicle-img"
                  src="https://images.unsplash.com/photo-1492144534655-ae79c964c9d7?auto=format&fit=crop&w=800&q=80"
                  alt="EV Car"
                />
              </div>

              {/* AI Thermal Runaway Predictor Gauge */}
              <div className="glass-card col-span-4" style={{ display: "flex", flexDirection: "column", justifyContent: "space-between" }}>
                <div className="card-header-row">
                  <div className="card-title">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"><path d="M12 2v20M17 5H9.5a3.5 3.5 0 0 0 0 7h5a3.5 3.5 0 0 1 0 7H6"/></svg>
                    AI Runaway Risk Engine
                  </div>
                  <span style={{ fontSize: 12, color: riskColor, fontWeight: 700 }}>{telemetry.runaway_risk.toFixed(1)}% RISK</span>
                </div>
                
                <div style={{ margin: "16px 0", textAlign: "center" }}>
                  <div style={{ fontSize: 44, fontWeight: 800, color: riskColor, fontFamily: "var(--font-display)" }}>
                    {telemetry.runaway_risk < 15 ? "NOMINAL" : telemetry.runaway_risk < 50 ? "WARNING" : "CRITICAL"}
                  </div>
                  <p style={{ fontSize: 13, color: "var(--text-muted)", marginTop: 6 }}>
                    Realtime AI gradient analysis predicting battery cell stability
                  </p>
                </div>

                <div className="progress-track">
                  <div className="progress-fill" style={{ width: `${Math.min(100, telemetry.runaway_risk)}%`, background: riskColor }} />
                </div>
              </div>

              {/* 4 Metric KPI Stat Cards */}
              <div className="glass-card col-span-3 kpi-card">
                <div className="kpi-top">
                  <span className="kpi-label">Battery Temp</span>
                  <div className="kpi-icon-wrap">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"><path d="M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z"/></svg>
                  </div>
                </div>
                <div className="kpi-value-row">
                  <span className={`kpi-val ${telemetry.battery_temp > 60 ? "red" : "mint"}`}>{telemetry.battery_temp.toFixed(1)}</span>
                  <span className="kpi-unit">°C</span>
                </div>
                <div className="progress-track">
                  <div className="progress-fill" style={{ width: `${tempPct}%`, background: telemetry.battery_temp > 60 ? "var(--alert-red)" : "var(--mint)" }} />
                </div>
              </div>

              <div className="glass-card col-span-3 kpi-card">
                <div className="kpi-top">
                  <span className="kpi-label">Cabin CO₂ Gas</span>
                  <div className="kpi-icon-wrap">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"><path d="M12 3v18M3 12h18"/></svg>
                  </div>
                </div>
                <div className="kpi-value-row">
                  <span className={`kpi-val ${telemetry.co2_ppm > 2000 ? "red" : "mint"}`}>{telemetry.co2_ppm.toFixed(0)}</span>
                  <span className="kpi-unit">PPM</span>
                </div>
                <div className="progress-track">
                  <div className="progress-fill" style={{ width: `${co2Pct}%`, background: telemetry.co2_ppm > 2000 ? "var(--alert-red)" : "var(--mint)" }} />
                </div>
              </div>

              <div className="glass-card col-span-3 kpi-card">
                <div className="kpi-top">
                  <span className="kpi-label">Smoke Density</span>
                  <div className="kpi-icon-wrap">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/></svg>
                  </div>
                </div>
                <div className="kpi-value-row">
                  <span className="kpi-val">{(telemetry.smoke_density * 100).toFixed(1)}</span>
                  <span className="kpi-unit">%</span>
                </div>
                <div className="progress-track">
                  <div className="progress-fill" style={{ width: `${Math.min(100, telemetry.smoke_density * 100)}%`, background: "var(--cyan)" }} />
                </div>
              </div>

              <div className="glass-card col-span-3 kpi-card">
                <div className="kpi-top">
                  <span className="kpi-label">Pack Voltage / Current</span>
                  <div className="kpi-icon-wrap">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"><polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"/></svg>
                  </div>
                </div>
                <div className="kpi-value-row">
                  <span className="kpi-val mint">{telemetry.battery_voltage.toFixed(1)}</span>
                  <span className="kpi-unit">V / {telemetry.battery_current.toFixed(1)}A</span>
                </div>
                <div style={{ fontSize: 12, color: "var(--text-muted)", marginTop: 8 }}>
                  Power Draw: {((telemetry.battery_voltage * telemetry.battery_current) / 1000).toFixed(2)} kW
                </div>
              </div>

              {/* Real-time Telemetry Graph */}
              <div className="glass-card col-span-12 chart-card">
                <div className="card-header-row">
                  <div className="card-title">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"><polyline points="22 12 18 12 15 21 9 3 6 12 2 12"/></svg>
                    Live Sensor Telemetry Stream (WebSockets)
                  </div>
                  <span style={{ fontSize: 12, color: "var(--text-muted)" }}>Last updated: {lastUpdate}</span>
                </div>

                <svg className="svg-chart" viewBox={`0 0 ${chartWidth} ${chartHeight}`}>
                  <defs>
                    <linearGradient id="tempGradient" x1="0" y1="0" x2="0" y2="1">
                      <stop offset="0%" stopColor="#00C853" stopOpacity="0.4" />
                      <stop offset="100%" stopColor="#00C853" stopOpacity="0.0" />
                    </linearGradient>
                  </defs>
                  {/* Grid lines */}
                  <line x1="0" y1="40" x2={chartWidth} y2="40" stroke="rgba(255,255,255,0.05)" />
                  <line x1="0" y1="80" x2={chartWidth} y2="80" stroke="rgba(255,255,255,0.05)" />
                  <line x1="0" y1="120" x2={chartWidth} y2="120" stroke="rgba(255,255,255,0.05)" />
                  
                  {/* Filled Area */}
                  <polygon points={`0,${chartHeight} ${points} ${chartWidth},${chartHeight}`} fill="url(#tempGradient)" />
                  {/* Polyline */}
                  <polyline fill="none" stroke="#00C853" strokeWidth="3" points={points} />
                </svg>
              </div>
            </div>
          )}

          {/* GAS TAB */}
          {screen === "gas" && (
            <div className="dashboard-grid">
              <div className="glass-card col-span-6">
                <div className="card-header-row">
                  <div className="card-title">Cabin CO₂ Sensor Monitor</div>
                  <span style={{ color: "var(--mint)", fontWeight: 700 }}>NORMAL RANGE</span>
                </div>
                <div style={{ fontSize: 48, fontWeight: 800, color: "#FFF", margin: "20px 0" }}>
                  {telemetry.co2_ppm.toFixed(0)} <span style={{ fontSize: 20, color: "var(--text-muted)" }}>PPM</span>
                </div>
                <p style={{ color: "var(--text-muted)", fontSize: 14 }}>
                  MQ-7 / MQ-8 electrochemical sensor array scanning cabin air every 100ms. Threshold safety level is set at 2000 PPM.
                </p>
              </div>

              <div className="glass-card col-span-6">
                <div className="card-header-row">
                  <div className="card-title">Smoke & Aerosol Density</div>
                  <span style={{ color: "var(--cyan)", fontWeight: 700 }}>CLEAN AIR</span>
                </div>
                <div style={{ fontSize: 48, fontWeight: 800, color: "var(--cyan)", margin: "20px 0" }}>
                  {(telemetry.smoke_density * 100).toFixed(2)} <span style={{ fontSize: 20, color: "var(--text-muted)" }}>%</span>
                </div>
                <p style={{ color: "var(--text-muted)", fontSize: 14 }}>
                  Optical laser smoke detector measuring particulate matter density inside battery enclosure.
                </p>
              </div>
            </div>
          )}

          {/* EMERGENCY TAB */}
          {screen === "emergency" && (
            <div className="dashboard-grid">
              <div className={`glass-card col-span-12 ${telemetry.emergency_active ? "alert" : ""}`} style={{ textAlign: "center", padding: 48 }}>
                <div style={{ fontSize: 64, marginBottom: 12 }}>
                  {telemetry.emergency_active ? "🚨" : "🛡️"}
                </div>
                <h2 style={{ fontSize: 32, fontWeight: 800, color: "#FFF", marginBottom: 12 }}>
                  {telemetry.emergency_active ? "EMERGENCY SOS ALERT ACTIVE" : "ALL SYSTEMS NORMAL"}
                </h2>
                <p style={{ color: "var(--text-muted)", maxWidth: 600, margin: "0 auto 32px", fontSize: 16 }}>
                  {telemetry.emergency_active
                    ? "Thermal runaway risk detected or emergency SOS triggered! Fire suppression system armed and automated emergency response sequence activated."
                    : "EV battery sensors reporting nominal operating conditions. Automated suppression system standby."}
                </p>

                <button
                  className="emergency-panic-btn"
                  style={{ margin: "0 auto", padding: "14px 36px", fontSize: 16 }}
                  onClick={triggerEmergency}
                >
                  {telemetry.emergency_active ? "DISARM EMERGENCY ALERT" : "TRIGGER SOS PANIC ALERT"}
                </button>
              </div>
            </div>
          )}

          {/* FIRE SUPPRESSION TAB */}
          {screen === "suppression" && (
            <div className="dashboard-grid">
              <div className="glass-card col-span-8">
                <div className="card-header-row">
                  <div className="card-title">Aerosol Fire Suppression System</div>
                  <label className="switch-control">
                    <input type="checkbox" checked={telemetry.system_armed} onChange={toggleArm} />
                    <span className="slider-thumb" />
                  </label>
                </div>
                <div style={{ margin: "24px 0" }}>
                  <div style={{ fontSize: 20, fontWeight: 600, color: telemetry.system_armed ? "var(--mint)" : "var(--text-dim)" }}>
                    SYSTEM STATE: {telemetry.system_armed ? "ARMED & ACTIVE" : "DISARMED"}
                  </div>
                  <p style={{ color: "var(--text-muted)", marginTop: 8 }}>
                    Solid aerosol fire extinguisher canisters positioned directly inside cell pack modules.
                  </p>
                </div>
              </div>

              <div className="glass-card col-span-4">
                <div className="card-header-row">
                  <div className="card-title">Agent Remaining</div>
                </div>
                <div style={{ fontSize: 44, fontWeight: 800, color: "var(--mint)" }}>98.5%</div>
                <div className="progress-track" style={{ marginTop: 16 }}>
                  <div className="progress-fill" style={{ width: "98.5%", background: "var(--mint)" }} />
                </div>
              </div>
            </div>
          )}

          {/* ESP32 HARDWARE TAB */}
          {screen === "esp32" && (
            <div className="dashboard-grid">
              <div className="glass-card col-span-6">
                <div className="card-header-row">
                  <div className="card-title">Connected Device</div>
                  <span className="status-indicator">
                    <span className={`status-dot ${connected ? "" : "offline"}`} />
                    {connected ? "ONLINE" : "DISCONNECTED"}
                  </span>
                </div>
                <div style={{ margin: "16px 0", display: "flex", flexDirection: "column", gap: 12 }}>
                  <div style={{ display: "flex", justifyContent: "space-between" }}>
                    <span style={{ color: "var(--text-muted)" }}>Device ID:</span>
                    <span style={{ fontWeight: 600, color: "#FFF" }}>{telemetry.device_id}</span>
                  </div>
                  <div style={{ display: "flex", justifyContent: "space-between" }}>
                    <span style={{ color: "var(--text-muted)" }}>MAC Address:</span>
                    <span style={{ fontFamily: "monospace", color: "var(--mint)" }}>{telemetry.mac_address}</span>
                  </div>
                  <div style={{ display: "flex", justifyContent: "space-between" }}>
                    <span style={{ color: "var(--text-muted)" }}>Firmware Version:</span>
                    <span style={{ fontWeight: 600, color: "#FFF" }}>{telemetry.firmware_version}</span>
                  </div>
                  <div style={{ display: "flex", justifyContent: "space-between" }}>
                    <span style={{ color: "var(--text-muted)" }}>Signal Strength:</span>
                    <span style={{ fontWeight: 600, color: "var(--mint)" }}>Excellent (-58 dBm)</span>
                  </div>
                </div>
              </div>

              <div className="glass-card col-span-6">
                <div className="card-header-row">
                  <div className="card-title">Hardware Mode Config</div>
                </div>
                <p style={{ color: "var(--text-muted)", marginBottom: 20 }}>
                  Switch between realistic AI Prototype telemetry generator and physical ESP32 micro-controller stream over WebSockets/MQTT.
                </p>
                <div className="mode-toggle-group" style={{ display: "inline-flex" }}>
                  <button className={`mode-btn ${telemetry.mode === "prototype" ? "active" : ""}`} onClick={() => setMode("prototype")}>
                    PROTOTYPE SIMULATOR
                  </button>
                  <button className={`mode-btn ${telemetry.mode === "hardware" ? "active" : ""}`} onClick={() => setMode("hardware")}>
                    PHYSICAL ESP32
                  </button>
                </div>
              </div>
            </div>
          )}

          {/* MAPS TAB */}
          {screen === "maps" && (
            <div className="dashboard-grid">
              <div className="glass-card col-span-12" style={{ height: 400, position: "relative", display: "flex", alignItems: "center", justifyContent: "center" }}>
                <div style={{ textAlign: "center" }}>
                  <svg width="48" height="48" viewBox="0 0 24 24" fill="none" stroke="var(--mint)" strokeWidth="2">
                    <polygon points="1 6 1 22 8 18 16 22 23 18 23 2 16 6 8 2 1 6" />
                  </svg>
                  <h3 style={{ fontSize: 22, color: "#FFF", marginTop: 12 }}>GPS Emergency Guidance System</h3>
                  <p style={{ color: "var(--text-muted)", marginTop: 6 }}>
                    Current Location: Latitude {telemetry.latitude.toFixed(4)}, Longitude {telemetry.longitude.toFixed(4)}
                  </p>
                  <div style={{ display: "flex", gap: 16, justifyContent: "center", marginTop: 20 }}>
                    <span className="v-badge" style={{ color: "var(--mint)", borderColor: "var(--mint)" }}>Nearest Hospital: 2.4 km (6 min)</span>
                    <span className="v-badge" style={{ color: "var(--amber)", borderColor: "var(--amber)" }}>Fire Station: 4.1 km (9 min)</span>
                  </div>
                </div>
              </div>
            </div>
          )}

          {/* SETTINGS TAB */}
          {screen === "settings" && (
            <div className="dashboard-grid">
              <div className="glass-card col-span-6">
                <div className="card-header-row">
                  <div className="card-title">User Profile & Preferences</div>
                </div>
                <div style={{ display: "flex", alignItems: "center", gap: 16, margin: "16px 0" }}>
                  <div style={{ width: 56, height: 56, borderRadius: "50%", background: "var(--mint)", display: "flex", alignItems: "center", justifyContent: "center", fontSize: 24, fontWeight: 700, color: "#0A0E1A" }}>
                    AC
                  </div>
                  <div>
                    <h3 style={{ fontSize: 18, color: "#FFF" }}>Alex Chen</h3>
                    <span style={{ fontSize: 13, color: "var(--text-muted)" }}>Lead EV Safety Engineer</span>
                  </div>
                </div>
              </div>
            </div>
          )}
        </main>
      </div>
    </div>
  );
}
