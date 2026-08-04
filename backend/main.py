import asyncio
import json
import time
from typing import List
from fastapi import FastAPI, WebSocket, WebSocketDisconnect, Depends, HTTPException, Body
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel

from backend.hal import hal
from backend.database import get_telemetry_history, set_config, get_config
from backend.auth import create_access_token, get_current_user, require_role, USERS
from backend.ai_engine import AIBatteryEngine
from backend.mqtt_client import mqtt_service

app = FastAPI(
    title="SafeCell AI Platform Backend API",
    description="FastAPI Backend for EV Battery Monitoring, Fire Suppression & ESP32 Hardware Integration",
    version="1.0.0"
)

# CORS Middleware for Next.js Frontend
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# WebSocket Connection Manager
class ConnectionManager:
    def __init__(self):
        self.active_connections: List[WebSocket] = []

    async def connect(self, websocket: WebSocket):
        await websocket.accept()
        self.active_connections.append(websocket)

    def disconnect(self, websocket: WebSocket):
        if websocket in self.active_connections:
            self.active_connections.remove(websocket)

    async def broadcast(self, message: dict):
        payload = json.dumps(message)
        disconnected = []
        for connection in self.active_connections:
            try:
                await connection.send_text(payload)
            except Exception:
                disconnected.append(connection)
        for conn in disconnected:
            self.disconnect(conn)

manager = ConnectionManager()

# Background Telemetry Stream Task
@app.on_event("startup")
async def startup_event():
    mqtt_service.start()
    asyncio.create_task(telemetry_stream_loop())

async def telemetry_stream_loop():
    while True:
        if hal.mode == "prototype":
            telemetry = hal.generate_prototype_telemetry()
        else:
            telemetry = hal.latest_telemetry
        await manager.broadcast(telemetry)
        await asyncio.sleep(1.0)

# Root & Health Check Endpoints
@app.get("/")
def root():
    return {
        "system": "SafeCell AI Platform API",
        "status": "online",
        "mode": hal.mode,
        "swagger_docs": "http://127.0.0.1:8000/docs",
        "health_check": "http://127.0.0.1:8000/health",
        "frontend_url": "http://localhost:3000"
    }

@app.get("/health")
def health_check():
    return {
        "status": "online",
        "system": "SafeCell AI Platform Backend",
        "mode": hal.mode,
        "active_websockets": len(manager.active_connections),
        "timestamp": time.time()
    }

# Auth Endpoints
class LoginRequest(BaseModel):
    username: str
    password: str

@app.post("/api/auth/login")
def login(credentials: LoginRequest):
    user = USERS.get(credentials.username.lower())
    if not user or user["password"] != credentials.password:
        raise HTTPException(status_code=401, detail="Invalid username or password")
    
    token = create_access_token({"sub": credentials.username, "role": user["role"], "name": user["name"]})
    return {
        "access_token": token,
        "token_type": "bearer",
        "user": {
            "username": credentials.username,
            "name": user["name"],
            "role": user["role"]
        }
    }

# Mode Switch Endpoint
@app.get("/api/mode")
def get_mode():
    return {"mode": hal.mode}

@app.post("/api/mode")
def set_mode(payload: dict = Body(...)):
    new_mode = payload.get("mode")
    if hal.set_mode(new_mode):
        return {"status": "success", "mode": hal.mode}
    raise HTTPException(status_code=400, detail="Invalid mode. Choose 'prototype' or 'hardware'")

# Telemetry Endpoints
@app.get("/api/telemetry")
def get_latest_telemetry():
    return hal.latest_telemetry

@app.get("/api/telemetry/history")
def get_history(limit: int = 30):
    return get_telemetry_history(limit=limit)

@app.post("/api/telemetry")
def post_telemetry(payload: dict = Body(...)):
    if hal.mode == "hardware":
        hal.update_hardware_telemetry(payload)
    return {"status": "received"}

# Fire Suppression Config
@app.post("/api/suppression/config")
def update_suppression(payload: dict = Body(...)):
    if "system_armed" in payload:
        hal.system_armed = bool(payload["system_armed"])
        hal.latest_telemetry["system_armed"] = hal.system_armed
    if "sensitivity" in payload:
        hal.sensitivity = float(payload["sensitivity"])
        hal.latest_telemetry["sensitivity"] = hal.sensitivity
    return {
        "status": "updated",
        "system_armed": hal.system_armed,
        "sensitivity": hal.sensitivity
    }

# Emergency Alert Endpoint
@app.post("/api/emergency/trigger")
def trigger_emergency(payload: dict = Body(...)):
    active = payload.get("active", True)
    hal.emergency_active = bool(active)
    hal.latest_telemetry["emergency_active"] = hal.emergency_active
    return {
        "status": "alert_updated",
        "emergency_active": hal.emergency_active
    }

# Device Management Endpoints
@app.get("/api/device/status")
def get_device_status():
    return {
        "device_id": hal.device_id,
        "mac_address": hal.mac_address,
        "firmware_version": hal.firmware_version,
        "status": "Connected",
        "signal_strength": "Excellent (-58 dBm)",
        "last_synced": "Just now"
    }

# AI Prediction Endpoint
@app.get("/api/ai/predict")
def get_ai_prediction():
    tel = hal.latest_telemetry
    return AIBatteryEngine.analyze_telemetry(
        tel["battery_temp"],
        tel["battery_voltage"],
        tel["battery_current"],
        tel["co2_ppm"],
        tel["smoke_density"]
    )

# WebSocket Endpoint
@app.websocket("/ws/telemetry")
async def websocket_endpoint(websocket: WebSocket):
    await manager.connect(websocket)
    try:
        while True:
            data = await websocket.receive_text()
            try:
                payload = json.loads(data)
                if hal.mode == "hardware":
                    hal.update_hardware_telemetry(payload)
            except Exception:
                pass
    except WebSocketDisconnect:
        manager.disconnect(websocket)
