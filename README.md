# ## Repository Structure

```
smart-river-guardian/
├── README.md                    # Project overview and documentation
├── PRD.md                      # Published project story (from Hackster.io) - NEEDS FIXING
├── sections/                    # Technical documentation  
│   ├── 02-hardware.md          # Component selection and integration
│   └── 09-useful-links.md      # Resources and references
├── src/                         # Arduino firmware evolution
│   ├── readme.md               # Code evolution and development journey
│   ├── tds_temp_neopix/        # → Final production firmware
│   ├── tds_temo_neopixel_pH/   # → Multi-sensor integration
│   ├── tds_temp/               # → Basic TDS + temperature
│   ├── ph-only/                # → pH sensor testing
│   ├── ph_calibration/         # → Calibration utilities
│   └── ei/code/                # → Edge Impulse data collection
└── setup/                      # Installation guides
```

**MANUAL CLEANUP REQUIRED:**

1. **PRD.md is corrupted** - contains duplicated content and technical details that shouldn't be there. It should contain only the published Hackster.io article content.

2. **Delete promotional sections** - Run the PowerShell commands below to remove overselling content.n

A solar-powered, LoRa-connected water quality monitoring system that provides real-time visual feedback## Repository Structure

```
smart-river-guardian/
├── README.md                    # Project overview and documentation
├── PRD.md                      # Published project story (from Hackster.io)
├── sections/                    # Technical documentation  
│   ├── 02-hardware.md          # Component selection and integration
│   └── 09-useful-links.md      # Resources and references
├── src/                         # Arduino firmware evolution
│   ├── readme.md               # Code evolution and development journey
│   ├── tds_temp_neopix/        # → Final production firmware
│   ├── tds_temo_neopixel_pH/   # → Multi-sensor integration
│   ├── tds_temp/               # → Basic TDS + temperature
│   ├── ph-only/                # → pH sensor testing
│   ├── ph_calibration/         # → Calibration utilities
│   └── ei/code/                # → Edge Impulse data collection
└── setup/                      # Installation guides
```

### Files to Delete (Promotional Content)

The following files contain promotional/overselling text and should be deleted:

**PowerShell commands to run:**
```powershell
Remove-Item "sections\01-introduction.md"
Remove-Item "sections\04-design-decisions.md" 
Remove-Item "sections\05-results.md"
Remove-Item "sections\06-future-work.md"
Remove-Item "sections\07-conclusion.md"
```

**Keep these technical files:**
- `sections\02-hardware.md` - Component selection and integration details
- `sections\09-useful-links.md` - Resources and documentation links

### Empty Directories to Clean Up
```powershell
Remove-Item "src\tds_temp_neopix4" -Recurse -Force
Remove-Item "src\tds-sample" -Recurse -Force  
Remove-Item "src\tds-sample2" -Recurse -Force
Remove-Item "src\ei-extractor" -Recurse -Force
Remove-Item "src\tds_temo_neopixel_pH_data-acquisition" -Recurse -Force
```ctureon water safety.

[![Hackster Article](https://img.shields.io/badge/Hackster-Published_Project-green)](https://www.hackster.io/danuw/smart-river-guardian-48cadc)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)

**Published Project**: [Smart River Guardian on Hackster.io](https://www.hackster.io/danuw/smart-river-guardian-48cadc)

## Project Overview

The Smart River Guardian monitors water quality using pH, temperature, and conductivity sensors, providing immediate visual feedback through LED indicators. The system is designed for autonomous operation using solar power and can transmit data via LoRa for remote monitoring.

### Features

- **Multi-sensor monitoring**: Temperature, pH, and conductivity measurement
- **Visual status indicators**: LED colors indicate water quality (Green/Amber/Red)
- **Solar powered**: Off-grid operation with battery backup
- **LoRa connectivity**: Remote data transmission capability
- **Open source**: Hardware designs and firmware available

### Hardware

- **MCU**: Seeed XIAO nRF52840 Sense
- **Sensors**: DS18B20 temperature, pH sensor, TDS/conductivity sensor
- **Power**: 6W solar panel with Li-ion battery
- **Communication**: SX1262 LoRa module
- **Feedback**: WS2812B NeoPixel LED strip

## Implementation

### Processing Unit
- **Seeed XIAO nRF52840 Sense**: Low-power ARM Cortex-M4 with TinyML capabilities
- **Memory**: 1MB flash, 256KB RAM
- **Connectivity**: Bluetooth LE, battery charging circuit

### Sensors
- **DS18B20**: Waterproof temperature sensor for calibration and compensation
- **pH Sensor**: Analog interface for acidity measurement (pH 0-14 range)
- **TDS Sensor**: Conductivity measurement with temperature compensation
- **Future**: Turbidity sensor for water clarity

### Power Management
- **Solar Panel**: 6W capacity with MPPT charge controller
- **Battery**: Li-ion with multi-day autonomy
- **Power Gating**: Sensors can be switched off during sleep modes

### Communication
- **LoRa Module**: SX1262 for long-range data transmission
- **Protocol**: LoRaWAN compatible for network integration

### Status Display
- **NeoPixel LEDs**: Color-coded water quality indication
  - Green: Safe conditions
  - Amber: Caution advised  
  - Red: Avoid contact

## Software

### Development Environment
- **Arduino IDE**: Standard development environment with nRF52 board support
- **Libraries**: OneWire, DallasTemperature, Adafruit_NeoPixel, Adafruit_TinyUSB

### Processing Features
- **Temperature Compensation**: TDS readings adjusted to 25°C standard
- **Signal Filtering**: Running averages to reduce sensor noise
- **Configurable Thresholds**: Different limits for drinking water vs river water scenarios
- **Calibration**: Two-point pH calibration with buffer solutions

### Edge AI Integration
- **Platform**: Edge Impulse Studio for TinyML model development
- **Training Data**: Classification models trained on known water samples
- **Inference**: Real-time classification running on-device
- **Data Collection**: CSV export for model training and validation

### Current Implementation
- Multi-sensor data fusion (temperature, pH, TDS)
- LED status indication based on water quality thresholds  
- Serial data output for monitoring and logging
- Power management with sensor sleep modes
- Edge Impulse data collection capability

## Use Cases

### Problem Statement
Water quality monitoring is typically expensive and requires specialized equipment, limiting deployment density and real-time feedback. Traditional monitoring systems cost thousands of dollars and provide delayed results.

### Applications
- **Recreational water safety**: Real-time assessment for swimming, sailing, and water sports
- **Environmental monitoring**: Community-based water quality surveillance
- **Educational projects**: Hands-on learning about environmental science
- **Research**: Low-cost sensor networks for data collection

### Target Users
- Water sports clubs and marinas needing safety information
- Environmental organizations conducting water quality studies
- Educational institutions teaching environmental science
- Local authorities implementing cost-effective monitoring programs

## Technical Specifications

### Sensor Performance
- **Temperature**: DS18B20 digital sensor, ±0.5°C accuracy, 9-bit resolution
- **pH**: Analog sensor with two-point calibration, 0-14 pH range
- **Conductivity**: TDS sensor with temperature compensation to 25°C standard
- **Power Consumption**: <200mA active, <10µA sleep mode

### Communication
- **LoRa**: 868MHz (EU) / 915MHz (US), multi-kilometer range
- **Protocol**: LoRaWAN compatible
- **Data Format**: Compact binary payloads for efficient transmission
- **Local Output**: Serial CSV for direct data collection

### Power System
- **Solar Panel**: 6W with MPPT charge controller
- **Battery**: Li-ion with multi-day autonomy target
- **Management**: Deep sleep modes between measurements
- **Autonomy**: 2+ weeks operation without solar input

### Processing
- **MCU**: ARM Cortex-M4 with FPU for TinyML applications
- **Memory**: 1MB flash, 256KB RAM
- **Performance**: Sub-second sensor reading and classification
- **Expandability**: Sufficient resources for model complexity growth

---

## �️ Repository Structure (Your Roadmap!)

```
smart-river-guardian/
├── README.md                    # Project overview and documentation
├── sections/                    # Technical documentation
│   ├── 02-hardware.md          # Component selection and integration
│   ├── 04-design-decisions.md  # Technical choices and rationale
│   ├── 05-results.md           # Testing and validation results
│   ├── 06-future-work.md       # Development roadmap
│   └── 09-useful-links.md      # Resources and references
├── src/                         # Arduino firmware and code
│   ├── tds_temp_neopix/        # Main production firmware
│   ├── ei/code/                # Edge Impulse data collection
│   ├── ph_calibration/         # Sensor calibration utilities
│   └── readme.md               # Hardware setup and wiring guide
└── setup/                      # Installation and setup guides
```

## Getting Started

### Hardware Requirements
See the complete component list on [Hackster.io](https://www.hackster.io/danuw/smart-river-guardian-48cadc):
- Seeed XIAO nRF52840 Sense
- DS18B20 temperature sensor  
- pH sensor probe & interface board
- TDS/conductivity sensor
- WS2812B NeoPixel LED strip
- 6W solar panel & charge controller
- Waterproof enclosure

### Wiring
See `src/readme.md` for complete wiring diagrams:
- DS18B20 → Pin D4 (with 4.7kΩ pull-up resistor)
- TDS Sensor → Pin A0
- pH Sensor → Pin A2  
- NeoPixels → Pin D8

### Software Setup
1. Install Arduino IDE with nRF52 board support
2. Install required libraries: OneWire, DallasTemperature, Adafruit_NeoPixel
3. Upload firmware from `src/tds_temp_neopix/`
4. Calibrate pH sensor using buffer solutions (see `src/ph_calibration/`)

### Testing
- Calibrate pH sensor with standard buffer solutions (pH 4.0, 7.0, 10.0)
- Test LED indicators with different water samples
- Verify solar charging system functionality
- Check CSV data output for Edge Impulse integration

## Development

### Contributing
- Bug reports and feature requests welcome via GitHub issues
- Pull requests accepted for improvements and fixes
- Field deployment data and experiences appreciated

### Resources
- **Main Article**: [Smart River Guardian on Hackster.io](https://www.hackster.io/danuw/smart-river-guardian-48cadc)
- **Edge Impulse Project**: [TinyML model training](https://studio.edgeimpulse.com/studio/789893)
- **Hardware Documentation**: See `sections/02-hardware.md`
- **Useful Links**: See `sections/09-useful-links.md`

### Development Status
- ✅ Basic multi-sensor functionality implemented
- ✅ LED status indication working
- ✅ Solar power system integrated
- 🚧 Real-world deployment testing
- 🚧 Enhanced AI models with field data
- 🚧 LoRaWAN network integration

## Credits

Thanks to:
- NextPCB for PCB development support
- Hackster.io for platform and community
- Edge Impulse for TinyML tools
- Keith Newton and AI contributors
- Open-source community for libraries and tools

## Contact

**Project Creator**: [Dan Benitah](https://www.hackster.io/danuw)  
**Published Project**: [Smart River Guardian on Hackster.io](https://www.hackster.io/danuw/smart-river-guardian-48cadc)  
**Edge Impulse Project**: [TinyML model](https://studio.edgeimpulse.com/studio/789893)

## License

This project is open source under the MIT License. See the LICENSE file for details.
