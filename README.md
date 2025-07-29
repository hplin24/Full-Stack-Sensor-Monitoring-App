# Full-Stack Embedded Sensor Monitoring System

### Introduction

**Full-Stack Embedded Sensor Monitoring System** is an end-to-end system that integrates an STM32 microcontroller (NUCLEO-F446RE) with a modern Node.js-based application stack. It allows users to monitor temperature and humidity data from a DHT11 sensor using a simple desktop widget.

This project includes:

- Embedded C firmware for STM32 NUCLEO-F446RE (using HAL and STM32CubeIDE)
- Windows C++ communication module (Win32 API)
- Node.js native addon (C++ bindings)
- RESTful backend server using Express.js
- Electron + React.js frontend desktop widget

---

### Features

- 🔌 Temperature and humidity monitoring via DHT11 sensor
- 💠 Firmware communication over UART using ST-Link VCP
- 🔄 Native C++ to JavaScript bridging via Node.js addon
- 🌐 RESTful API for hardware interaction
- 💻 Electron-based desktop UI using React

---

### Architecture

```
DHT11 Sensor --> STM32 (UART) --> C++ Communication Module --> Node.js Addon --> Express API --> Electron/React UI
```

---

### Project Structure

```bash
.
├── firmware/           # Embedded C code for STM32 NUCLEO-F446RE
│   └── dht11/
├── app/
│   ├── backend/        # Express.js REST API
│   ├── frontend/       # React + Electron desktop app
│   └── shared/         # Shared types used by both frontend and backend
├── images/             # README images
└── README.md
```

---

### Getting Started

> ⚠️ Prerequisites: STM32 NUCLEO-F446RE board, DHT11 sensor, Windows PC, Node.js (v22+), C++ build tools

#### 1. Flash Firmware

Use STM32CubeIDE to build and flash the firmware located in `/firmware/dht11/` to the NUCLEO board.

#### 2. Build and Start Backend Server

```bash
cd app/backend
npm install
npm start
```

Note: This step also builds the Node.js native addon

#### 3. Build an Launch Desktop UI

```bash
cd app/frontend
npm install
npm start
```

---

### Demo

<table>
  <tr>
    <td><img src="images/board_topview.png" alt="Image 1" width="340"/></td>
    <td><img src="images/widget.png" alt="Image 2" width="300"/></td>
  </tr>
</table>

---

### Tech Stack

- STM32 (HAL, C)
- C++ (Win32 API)
- Node.js + Custom addon
- Express.js
- React.js + Electron

---

### License

MIT
