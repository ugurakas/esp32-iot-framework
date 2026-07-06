# esp32-iot-framework
A modular and scalable ESP32 IoT framework designed for production-ready embedded applications using ESP-IDF and FreeRTOS.
# ESP32 IoT Framework

A modular and scalable ESP32 IoT framework designed for production-ready embedded applications using ESP-IDF and FreeRTOS.

---

## Overview

This project provides a reusable software architecture for ESP32-based IoT devices. It is designed with modularity, scalability and maintainability in mind, making it suitable for real-world embedded products.

The framework aims to simplify the development of connected devices by providing ready-to-use modules for networking, communication, storage and system management.

---

## Features

- ESP-IDF based architecture
- FreeRTOS multitasking
- Wi-Fi Manager
- MQTT Client
- BLE Support (planned)
- OTA Firmware Update
- NVS Storage
- Logging System
- Event Manager
- JSON Parser (cJSON)
- Device Configuration
- Error Handling
- Watchdog Support
- Modular Driver Layer

---

## Project Structure

```
esp32-iot-framework/
│
├── main/
├── components/
├── drivers/
├── services/
├── middleware/
├── config/
├── docs/
├── images/
└── README.md
```

---

## Technologies

- C
- ESP-IDF
- FreeRTOS
- MQTT
- Wi-Fi
- BLE
- OTA
- cJSON
- NVS

---

## Development Roadmap

### Version 1.0

- [x] Project Architecture
- [ ] Logger Module
- [ ] Wi-Fi Manager
- [ ] MQTT Client
- [ ] Sensor Service
- [ ] JSON Parser

### Version 1.1

- [ ] OTA Update
- [ ] BLE Manager
- [ ] CLI Interface
- [ ] Power Management

### Version 2.0

- [ ] Secure Boot
- [ ] Flash Encryption
- [ ] Unit Tests
- [ ] Web Configuration

---

## Future Goals

The long-term goal is to build a production-ready embedded framework that can be reused across multiple IoT products including:

- Smart Home
- Smart Energy
- Industrial IoT
- Environmental Monitoring
- Remote Sensor Nodes

---

## License

This project is licensed under the MIT License.

---

## Author

**Uğur Akas**

Embedded Software Engineer

LinkedIn:
https://linkedin.com/in/ugur-akas

GitHub:
https://github.com/ugurakas

![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v5.5-blue)

![FreeRTOS](https://img.shields.io/badge/FreeRTOS-Supported-green)

![License](https://img.shields.io/badge/License-MIT-yellow)

![Language](https://img.shields.io/badge/Language-C-blue)
