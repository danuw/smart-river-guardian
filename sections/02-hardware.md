# 🔧 Hardware Selection & Setup

## Design Approach

The Smart River Guardian hardware focuses on practical deployment with available components:
1. **Standard components**: Using readily available sensors and modules
2. **Low power**: Designed for battery and solar operation
3. **Community accessible**: Arduino-compatible with open documentation

## Core Processing Unit: Seeed XIAO nRF52840

The **Seeed Studio XIAO nRF52840 Sense** was selected as the main controller for this project.

![XIAO nRF52840 Sense Pinout](https://files.seeedstudio.com/wiki/XIAO-BLE/pinout2.png)

**Selection rationale:**
- **Low power capabilities**: nRF52840 supports deep sleep modes
- **Small form factor**: XIAO series compact design (20mm x 17.5mm)
- **TinyML potential**: 1MB flash, 256KB RAM for Edge AI applications
- **Arduino compatibility**: Familiar development environment
- **Built-in charging**: Battery management although I later discovered that I needed an MPPT for solar applications

*Note: Some challenges were discovered including EEPROM not natively supported vs ESP32 boards, and serial monitor/TinyUSB issues.*

## Water Quality Sensors

### Temperature Sensing - DS18B20
Water temperature is needed for sensor calibration and temperature compensation of other readings.

- **Sensor**: Waterproof DS18B20 temperature probe
- **Interface**: 1-Wire digital protocol (Pin D4 with 4.7k pull-up)
- **Purpose**: Temperature compensation for pH and conductivity readings

### pH Measurement
pH helps measure the level of acidity and bases in water. Water tends to be close to pH 7, lemon is acidic around pH 3, and detergent is basic around pH 10.

- **Interface**: Analog pH sensor board connected to Pin A2
- **Power**: 5V supply required for sensor board
- **Calibration**: Uses buffer solutions for accuracy

### TDS (Total Dissolved Solids)
TDS indicates how many milligrams of soluble solids are dissolved in a liter of water. Higher TDS values mean more dissolved solids and less clean water.

- **Components**: DS18B20 for temperature, 5V and 3V power supplies
- **Interface**: Analog output to Pin A0
- **Processing**: Manual EC→TDS calculation to avoid EEPROM/K value issues

**From the current firmware:**
```cpp
// Temperature compensation to 25°C (approx 2%/°C)
float ec25 = ec / (1.0f + 0.02f * (tempC - 25.0f));
// TDS conversion factor (commonly 0.5 for NaCl equivalence)
float tds = ec25 * 0.5f;
```

### Turbidity (Planned)
To measure water clarity.
- **Dependencies**: Analog interface, 5V power
- **Purpose**: Detect suspended particles and water clarity

## Power System Planning

### Solar Power Challenges
Initial assumption was that solar panel charging would be straightforward, but irregular sun conditions required more sophisticated power management. A charge controller capable of regulating power/voltage is needed to avoid damaging the board and components.

**Components planned:**
- **Solar Panel**: 6W capacity for energy harvesting  
- **Charge Controller**: MPPT-type controller (considered Seeed Lipo Rider series)
- **Battery**: Li-ion/LiPo with sufficient capacity for multi-day operation

### Power Management Strategy
The firmware includes power gating for sensors:

```cpp
// From actual implementation
digitalWrite(SENSOR_POWER_EN, HIGH);  // Enable sensor rail
delay(1000);                          // Allow settling time
// Take measurements...
digitalWrite(SENSOR_POWER_EN, LOW);   // Disable for sleep
```

## Communication: LoRa Integration

### LoRaWAN Module
- **Module**: SX1262 LoRa module for LoRaWAN connectivity
- **Purpose**: Remote data transmission to monitoring networks
- **Integration**: SPI communication with the XIAO board

## Visual Feedback: NeoPixel LEDs

### Status Display
A NeoPixel LED strip provides immediate visual feedback:

**Current implementation:**
- **Green**: Safe water conditions (≤500 ppm TDS for drinking water, ≤250 ppm for river water)
- **Amber**: Caution conditions (500-1000 ppm drinking water, 250-1000 ppm river water)  
- **Red**: Avoid conditions (>1000 ppm TDS)

**From the working firmware:**
```cpp
if (SCENARIO == 1) { // Drinking water
  // Green ≤500, Amber 500–1000, Red >1000 ppm
  color = (tds <= 500.0f)  ? strip.Color(0,255,0) :
          (tds <= 1000.0f) ? strip.Color(255,150,0) :
                             strip.Color(255,0,0);
} else {             // River water
  // Green ≤250, Amber 250–1000, Red >1000 ppm
  color = (tds <= 250.0f)  ? strip.Color(0,255,0) :
          (tds <= 1000.0f) ? strip.Color(255,150,0) :
                             strip.Color(255,0,0);
}
```

## Current Wiring Configuration

**From the working prototype:**
- DS18B20 (temp) → D4 (with 4.7 kΩ pull-up to 3V3)
- TDS (Gravity board) → A0 (board powered at 5V; analog out to A0)
- pH board → A2 (board powered at 5V; analog out to A2)  
- NeoPixel (5 LEDs) → D8 (power at 5V; common GND; ~330Ω in series recommended)

https://app.cirkitdesigner.com/project/dc315478-267e-4272-85d5-1c0eeec2eb3f

## Required Arduino Libraries

**Currently implemented:**
- OneWire
- DallasTemperature  
- DFRobot_pH
- Adafruit_NeoPixel
- Adafruit_TinyUSB (with noted challenges)

## Development Status

**Currently working:**
- Temperature reading with DS18B20
- TDS measurement with temperature compensation
- pH sensor integration (with calibration needs)
- LED status display based on TDS thresholds
- Basic power management with sensor gating

**Planned/In Development:**
- Turbidity sensor integration
- LoRa communication implementation
- Solar charging system integration
- Custom PCB design
- Waterproof enclosure assembly