# Smart River Guardian - Code Evolution

This document traces the development journey of the Smart River Guardian firmware, showing how I built up the system incrementally, testing each component before adding the next.

## Code Evolution Timeline

As mentioned in the [published Hackster article](https://www.hackster.io/danuw/smart-river-guardian-48cadc): *"I went one component at a time, testing, debugging, and making sure each worked before adding the next."*

### Phase 1: Basic Sensor Testing

#### 1. **Temperature Only** → `tds_temp/tds_temp.ino`
- **What it does**: Basic DS18B20 temperature sensor + TDS sensor
- **Key features**: Simple temperature reading, basic TDS calculation
- **Purpose**: Prove the sensors work individually
- **Libraries**: OneWire, DallasTemperature, Adafruit_TinyUSB

#### 2. **pH Sensor Only** → `ph-only/ph-only.ino`  
- **What it does**: Isolated pH sensor testing
- **Key features**: Raw pH voltage readings, basic conversion
- **Purpose**: Test pH sensor independently before integration
- **Libraries**: Adafruit_TinyUSB for dual serial output

### Phase 2: Multi-Sensor Integration

#### 3. **Combined Sensors** → `tds_temo_neopixel_pH/tds_temo_neopixel_pH.ino`
- **What it does**: Temperature + TDS + pH + NeoPixel visual feedback
- **Key features**: 
  - All three sensors working together
  - First LED status implementation  
  - Temperature compensation for TDS
  - Dual serial output (TinyUSB + Serial)
- **Purpose**: Integrate all sensors with visual feedback

#### 4. **Production Version** → `tds_temp_neopix/tds_temp_neopix.ino`
- **What it does**: Clean, optimized version for deployment
- **Key features**:
  - Simplified, more reliable code
  - Better sensor filtering
  - Configurable scenarios (drinking water vs river water)
  - Power-optimized for solar operation
- **Purpose**: Final production firmware

### Phase 3: Edge AI & Data Collection

#### 5. **Calibration Tools** → `ph_calibration/ph_calibration.ino`
- **What it does**: pH sensor calibration utility
- **Key features**: Two-point calibration with buffer solutions
- **Purpose**: Accurate pH readings for AI training

#### 6. **Data Collection** → `ei/code/ei-df2/ei-df2.ino`
- **What it does**: Export sensor data for Edge Impulse training
- **Key features**: 
  - CSV data export format
  - High-frequency sampling
  - Robust data filtering
- **Purpose**: Generate training data for TinyML models

#### 7. **Data Forwarding** → `ei-data-forward/ei-data-forward.ino`
- **What it does**: Stream data to Edge Impulse directly
- **Key features**: Real-time data streaming
- **Purpose**: Live data collection for model training

## Development Philosophy

The incremental approach proved crucial for debugging and validation:

1. **One sensor at a time**: Each component was tested individually
2. **Multimeter debugging**: "Multimeter = best friend" - verify voltages and signals
3. **Gradual complexity**: Add features only after previous ones work reliably
4. **Real-world testing**: Test with actual liquids (tap water, mineral water, Coca-Cola)

## Current Production Firmware

**Main Code**: `tds_temp_neopix/tds_temp_neopix.ino`

This is the current production firmware used in deployed systems:
- Multi-sensor integration (DS18B20 temperature, pH, TDS)
- NeoPixel visual feedback (Green/Amber/Red status based on water quality)
- Temperature compensation for accurate TDS readings
- Configurable thresholds for different scenarios
- Power optimization for solar/battery operation

## Hardware Connections

Based on the current production firmware:

| Component | XIAO Pin | Notes |
|-----------|----------|-------|
| **DS18B20 Temperature** | D4 | 4.7kΩ pull-up resistor to 3V3 required |
| **TDS Sensor** | A0 | Analog output from TDS sensor board |
| **pH Sensor** | A2 | Analog output from pH sensor board |
| **NeoPixel LEDs** | D8 | 5V power, 330Ω resistor recommended |

## Key Features by Development Phase

### Early Development Features
- **Basic sensor readings**: Raw voltage values from analog sensors
- **Temperature compensation**: Critical for accurate TDS measurements
- **Serial debugging**: Extensive logging for development and troubleshooting

### Production Features  
- **Visual status indicators**: LED colors provide instant water quality assessment
  - Green: Safe conditions
  - Amber: Caution advised  
  - Red: Avoid contact
- **Configurable scenarios**: Different thresholds for drinking water vs recreational water
- **Data filtering**: Running averages and outlier rejection for stable readings
- **Power management**: Optimized for battery/solar operation

### Edge AI Integration
- **CSV data export**: Formatted for Edge Impulse training
- **Real-time classification**: TinyML models running on-device (in development)
- **Training data collection**: Systematic data gathering from different water types

## Required Libraries

Install these through Arduino IDE Library Manager:

**Core Libraries** (used across all versions):
- `OneWire` - DS18B20 temperature sensor communication
- `DallasTemperature` - Temperature sensor interface
- `Adafruit_NeoPixel` - LED strip control

**Advanced Features** (used in some versions):
- `Adafruit_TinyUSB` - Dual serial output capability

## Arduino IDE Setup

1. Install **Seeed nRF52 Boards** package
2. Select **Seeed XIAO nRF52840 Sense** as target board  
3. Set **TinyUSB Stack** if using dual serial output variants

## Testing & Calibration Journey

### The "Tap, Bottle, Coke" Approach
As documented in the Hackster article, I used a simple but effective testing method:

1. **Tap water**: Baseline "safe" readings
2. **Mineral water**: Higher TDS values  
3. **Coca-Cola**: High acidity (low pH) and high TDS - perfect "avoid" test case

This simple approach provided enough variation to:
- Test sensor responses across different ranges
- Validate LED color logic
- Generate training data for Edge Impulse models
- Prove the concept before moving to real river water

### pH Calibration Process
Use `ph_calibration/ph_calibration.ino`:
1. Prepare buffer solutions (pH 4.0, 7.0, 10.0)
2. Take voltage readings for each buffer
3. Calculate calibration constants: `pH = CAL_M * voltage + CAL_B`
4. Update constants in production firmware

## File Organization

**Development Sequence**:
1. `ph-only/` - pH sensor isolation test
2. `tds_temp/` - Temperature + TDS basic functionality  
3. `tds_temo_neopixel_pH/` - All sensors + LEDs integration
4. `tds_temp_neopix/` - **Production version** (current)
5. `ph_calibration/` - Calibration utilities
6. `ei/code/` - Edge Impulse data collection

**Deployment Ready**:
- `tds_temp_neopix/tds_temp_neopix.ino` - **Use this for actual deployments**

**Utilities**:
- `ph_calibration/ph_calibration.ino` - For sensor calibration
- `ei/code/ei-df2/ei-df2.ino` - For AI model training data

**Empty folders**: Several empty folders remain from development iterations and can be safely deleted.

## Development Lessons Learned

### Key Insights from the Build Process:

1. **Incremental development works**: Testing each sensor individually prevented complex debugging sessions
2. **Multimeter is essential**: As noted in the article - "Multimeter = best friend"
3. **Simple test cases are effective**: The "tap vs Coke" approach provided enough variation for development
4. **Temperature compensation matters**: Critical for accurate TDS readings across conditions
5. **Visual feedback is powerful**: LEDs make the system immediately useful to non-technical users

### Common Development Issues:
- **Serial communication**: TinyUSB can be temperamental - regular Serial often more reliable
- **Sensor power sequencing**: Some sensors need settling time after power-on
- **Pull-up resistors**: Essential for DS18B20 - don't forget the 4.7kΩ resistor
- **Voltage levels**: Verify sensor board outputs match XIAO input requirements

## Next Steps

From the published article's "What's Next" section:
- Tidier 3D-printed case (instead of jumper-wire-spaghetti!)
- Collect real river data → refine the model  
- PCB design with NextPCB → plug-and-play, solar-ready version
- Add LoRa for remote river health monitoring
- Future exploration with nRF54L15 + Zephyr + TinyML

---

*This evolutionary approach - building incrementally and testing with simple, available materials - made the project manageable and ultimately successful.* 