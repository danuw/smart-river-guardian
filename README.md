# 🌊 Smart River Guardian — Edge AI Earth Guardians Contest Submission

> **A solar-powered, AI-enabled water quality monitoring system that provides real-time environmental data for community water protection.**

[![Hackster Contest](https://img.shields.io/badge/Hackster-Edge_AI_Earth_Guardians-blue)](https://www.hackster.io/contests/earthguardians/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)
[![Status](https://img.shields.io/badge/Status-Prototype-yellow)](https://www.hackster.io/danuw/smart-river-guardian-48cadc)

---

## 📋 Project Overview

**Smart River Guardian** is an IoT + Edge AI solution designed to monitor and protect river ecosystems. The system combines low-power hardware, solar energy harvesting, and edge intelligence to track water quality in real time and provide immediate safety feedback through visual indicators.

### 🎯 What This Project Achieves

- **Real-time water monitoring**: Temperature, pH, conductivity (TDS), and turbidity sensing
- **Immediate visual feedback**: NeoPixel LED strip shows Safe (Green), Caution (Amber), or Danger (Red)
- **Off-grid operation**: Solar-powered with battery backup for autonomous deployment
- **Remote connectivity**: LoRa communication for data transmission to monitoring networks
- **Community accessible**: Open-source design using affordable, available components

---

## 🔧 Technical Implementation

### Hardware Components
- **MCU**: Seeed XIAO nRF52840 Sense (chosen for low power and TinyML capabilities)
- **Sensors**: 
  - DS18B20 waterproof temperature sensor
  - pH sensor with analog interface board
  - TDS/conductivity sensor with analog interface board  
  - Turbidity sensor (planned)
- **Power**: Solar panel + Li-ion battery with charge controller
- **Communication**: SX1262 LoRa module for remote data transmission
- **Feedback**: WS2812B NeoPixel LED strip (5 LEDs)
- **Target cost**: Under $250 per node

### Software Features
- **Arduino-based firmware** for accessibility and community development
- **Temperature compensation** for accurate sensor readings
- **Multi-sample filtering** using median filters to reduce noise
- **Configurable thresholds** for different water types (drinking vs river water)
- **Edge AI integration** planned using Edge Impulse for data classification
- **LoRaWAN protocol** for network connectivity

### Current Implementation Status
The prototype successfully demonstrates:
- ✅ **Multi-sensor integration**: Temperature, pH, and TDS sensors working
- ✅ **Visual status display**: LED color coding based on water quality thresholds
- ✅ **Temperature compensation**: EC measurements adjusted to 25°C standard
- ✅ **Solar power system**: Autonomous operation capability
- ✅ **Data collection**: Sensor readings with serial output for monitoring

---

## 🌍 Environmental Impact & Use Cases

### Problem Addressed
Traditional water quality monitoring systems are:
- **Expensive**: Professional stations cost $8,000-$12,000
- **Sparse**: Limited deployment due to high costs
- **Delayed**: Weekly or monthly data updates
- **Inaccessible**: Technical expertise required for operation

### Smart River Guardian Solution
- **Affordable**: Built with commodity components for community deployment
- **Dense networks**: Multiple nodes can monitor entire waterways
- **Real-time**: Immediate status feedback and regular data transmission
- **User-friendly**: Visual LED status requires no technical training

### Target Users
- **Sailing clubs and marinas**: Real-time water safety for recreational activities
- **Environmental organizations**: Community-based water quality monitoring
- **Educational institutions**: Hands-on environmental science projects
- **Local authorities**: Cost-effective water quality surveillance networks

---

## 📊 Technical Specifications

### Sensor Performance
- **Temperature**: DS18B20 digital sensor, ±0.5°C accuracy
- **pH**: Analog sensor with calibration, 0-14 pH range
- **Conductivity**: Analog sensor with temperature compensation, outputs EC and TDS
- **Power consumption**: Low-power design with sensor power gating

### Communication
- **LoRa frequency**: 868MHz (EU) / 915MHz (US)
- **Protocol**: LoRaWAN for network integration
- **Range**: Several kilometers depending on terrain and gateway placement
- **Data format**: Compact binary payloads for efficient transmission

### Power System
- **Solar panel**: 6W capacity for energy harvesting
- **Battery**: Li-ion with multiple day autonomy target
- **Power management**: MPPT charge controller and low-power sleep modes
- **Autonomy goal**: 2+ weeks operation without solar charging

---

## 🚀 Project Structure

```
├── README.md              # Main project documentation
├── sections/               # Detailed documentation
│   ├── 01-introduction.md  # Project motivation and goals
│   ├── 02-hardware.md      # Component selection and integration
│   ├── 03-software.md      # Firmware and AI implementation  
│   ├── 04-design-decisions.md # Technical choices and rationale
│   ├── 05-results.md       # Testing and validation
│   ├── 06-future-work.md   # Development roadmap
│   └── 07-conclusion.md    # Project summary and impact
├── src/                    # Arduino firmware
│   ├── tds_temp_neopix/    # Current multi-sensor implementation
│   ├── ph_calibration/     # pH sensor calibration utilities
│   └── readme.md           # Firmware documentation and wiring
├── PRD.md                  # Detailed hardware requirements
└── setup/                  # Installation guides
```

---

## 🏆 Contest Relevance: Edge AI for Environmental Protection

### Edge AI Implementation
- **Platform**: Edge Impulse integration for TinyML model development
- **Processing**: On-device data classification and anomaly detection
- **Benefits**: Reduced false alarms, immediate decision-making, bandwidth efficiency
- **Model training**: Data forwarder setup for collecting labeled environmental data

### Environmental Conservation Focus
- **Water quality protection**: Real-time monitoring enables immediate response to pollution
- **Community empowerment**: Accessible technology for citizen science and local monitoring
- **Ecosystem monitoring**: Dense sensor networks can track environmental changes
- **Early warning system**: Rapid detection of contamination events

### Innovation Aspects
- **Democratized monitoring**: 90%+ cost reduction makes professional-grade monitoring accessible
- **Intelligent edge processing**: AI reduces noise and false alarms in environmental data
- **Open-source design**: Community-driven development and improvement
- **Modular architecture**: Extensible platform for additional sensors and capabilities

---

## 🛠️ Getting Started

### Hardware Setup
1. **Core components**: XIAO nRF52840, sensor boards, solar system, SX1262 LoRa module
2. **Wiring**: Follow pin assignments in `src/readme.md`
3. **Enclosure**: Waterproof housing with cable glands for deployment
4. **Calibration**: pH sensor calibration using buffer solutions

### Software Installation
1. **Arduino IDE**: Install with nRF52 board package
2. **Libraries**: OneWire, DallasTemperature, Adafruit_NeoPixel
3. **Firmware**: Upload from `src/tds_temp_neopix/`
4. **LoRa setup**: Configure network credentials for data transmission

### Deployment
1. **Testing**: Validate sensor readings and LED status indications
2. **Installation**: Mount in waterproof enclosure at deployment site
3. **Network**: Connect to LoRaWAN network (TTN, Helium, etc.)
4. **Monitoring**: Dashboard setup for remote data visualization

---

## 🤝 Open Source & Community

### Licensing
- **Hardware**: Open source design files and documentation
- **Software**: Arduino code released under permissive license
- **Community**: Welcoming contributions and collaborative improvement

### Development Status
This is a work-in-progress prototype demonstrating the core concepts and initial implementation. At contest submission time:
- Basic multi-sensor integration achieved
- Visual feedback system operational  
- Power system and solar charging implemented
- LoRa communication capability integrated
- Edge AI integration planned and partially implemented

### Future Development
- **Expanded sensor suite**: Additional water quality parameters
- **Enhanced AI models**: Improved classification and prediction
- **Field deployment**: Extended testing and validation
- **Community adoption**: Documentation and support for replication

---

## 📞 Contact & Links

**Project Lead**: [Dan Benitah](https://www.hackster.io/danuw)  
**Live Project**: [Smart River Guardian on Hackster](https://www.hackster.io/danuw/smart-river-guardian-48cadc)  
**Contest**: [Edge AI Earth Guardians](https://www.hackster.io/contests/earthguardians/)

---

*This project demonstrates how Edge AI can make environmental monitoring accessible to communities, enabling data-driven protection of our water resources through real-time, intelligent sensing systems.*
