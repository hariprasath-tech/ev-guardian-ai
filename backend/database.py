import sqlite3
import time
import json
import os

DB_FILE = os.path.join(os.path.dirname(__file__), "safecell.db")

def init_db():
    conn = sqlite3.connect(DB_FILE)
    cursor = conn.cursor()
    cursor.execute("""
    CREATE TABLE IF NOT EXISTS telemetry_history (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        timestamp REAL,
        device_id TEXT,
        battery_temp REAL,
        battery_voltage REAL,
        battery_current REAL,
        co2_ppm REAL,
        smoke_density REAL,
        thermal_gradient REAL,
        runaway_risk REAL,
        battery_health REAL,
        cycle_count INTEGER,
        latitude REAL,
        longitude REAL,
        system_armed INTEGER,
        emergency_active INTEGER
    )
    """)
    cursor.execute("""
    CREATE TABLE IF NOT EXISTS system_config (
        key TEXT PRIMARY KEY,
        value TEXT
    )
    """)
    conn.commit()
    conn.close()

def save_telemetry(data: dict):
    conn = sqlite3.connect(DB_FILE)
    cursor = conn.cursor()
    cursor.execute("""
    INSERT INTO telemetry_history (
        timestamp, device_id, battery_temp, battery_voltage, battery_current,
        co2_ppm, smoke_density, thermal_gradient, runaway_risk, battery_health,
        cycle_count, latitude, longitude, system_armed, emergency_active
    ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    """, (
        data.get("timestamp", time.time()),
        data.get("device_id", "SC-ESP32-04A2"),
        data.get("battery_temp", 31.0),
        data.get("battery_voltage", 398.4),
        data.get("battery_current", 12.5),
        data.get("co2_ppm", 412.0),
        data.get("smoke_density", 0.02),
        data.get("thermal_gradient", 9.0),
        data.get("runaway_risk", 1.8),
        data.get("battery_health", 98.0),
        data.get("cycle_count", 342),
        data.get("latitude", 37.7749),
        data.get("longitude", -122.4194),
        1 if data.get("system_armed", True) else 0,
        1 if data.get("emergency_active", False) else 0
    ))
    conn.commit()
    conn.close()

def get_telemetry_history(limit=50):
    conn = sqlite3.connect(DB_FILE)
    cursor = conn.cursor()
    cursor.execute("""
    SELECT timestamp, battery_temp, battery_voltage, battery_current, co2_ppm, smoke_density, runaway_risk
    FROM telemetry_history
    ORDER BY id DESC LIMIT ?
    """, (limit,))
    rows = cursor.fetchall()
    conn.close()
    
    # Reverse to return in chronological order
    history = []
    for r in reversed(rows):
        history.append({
            "timestamp": r[0],
            "battery_temp": r[1],
            "battery_voltage": r[2],
            "battery_current": r[3],
            "co2_ppm": r[4],
            "smoke_density": r[5],
            "runaway_risk": r[6]
        })
    return history

def set_config(key: str, value: str):
    conn = sqlite3.connect(DB_FILE)
    cursor = conn.cursor()
    cursor.execute("INSERT OR REPLACE INTO system_config (key, value) VALUES (?, ?)", (key, value))
    conn.commit()
    conn.close()

def get_config(key: str, default=None):
    conn = sqlite3.connect(DB_FILE)
    cursor = conn.cursor()
    cursor.execute("SELECT value FROM system_config WHERE key = ?", (key,))
    row = cursor.fetchone()
    conn.close()
    return row[0] if row else default

# Initialize DB on module load
init_db()
