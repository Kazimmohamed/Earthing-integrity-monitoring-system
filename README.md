# ⚡ Earthing Integrity Monitoring System

A real-time embedded system designed to monitor the **integrity of electrical earthing** in streetlights, parking areas, and similar installations. The system uses an **Arduino UNO** to process leakage current data, detect faults, and alert users with **buzzer** and **GSM module communication**.

---

## ✅ Features

- Leakage current detection using CT sensor
- Earth continuity verification
- Earth resistance measurement via voltage injection
- Real-time buzzer alerts
- SMS notifications to personnel via GSM
- Safe, affordable, and expandable design
- Future support for PLC communication

---

## 🔩 Hardware Components

| Component         | Description                         |
|------------------|-------------------------------------|
| Arduino UNO       | Microcontroller                     |
| CT Sensor         | 1000:1 turns ratio                  |
| Burden Resistor   | 4.7 kΩ                              |
| Capacitor         | 10 μF (for smoothing)               |
| Diode             | PN junction (half-wave rectifier)   |
| Op-Amp            | Amplifies CT sensor signal          |
| GSM Module        | SIM800L or compatible               |
| Buzzer            | 5V piezo buzzer                     |
| Relay (optional)  | For isolation or switching          |

---

## 📐 System Diagram (Concept)

