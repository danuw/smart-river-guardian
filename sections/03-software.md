# 💻 Software Architecture: Edge AI for Environmental Intelligence

## Firmware Strategy: Power-Efficient Intelligence

The Smart River Guardian firmware balances sophisticated data processing with ultra-low power consumption. Built on Arduino framework for accessibility, the code leverages the nRF52840's capabilities while maintaining community-friendly development practices.

### Core Firmware Architecture

```cpp
void loop() {
    // 1. Wake from deep sleep
    wakeSensors();
    
    // 2. Collect sensor data with oversampling
    SensorData data = collectSensorReadings();
    
    // 3. Apply Edge AI processing
    ProcessedData clean_data = processWithAI(data);
    
    // 4. Determine status and update LED
    WaterStatus status = determineWaterQuality(clean_data);
    updateStatusLED(status);
    
    // 5. Transmit via LoRa (if scheduled)
    if (shouldTransmit()) {
        transmitData(clean_data, status);
    }
    
    // 6. Return to deep sleep
    enterDeepSleep(SAMPLING_INTERVAL);
}
```

## Edge AI Integration: Real-Time Intelligence

### Anomaly Detection with TinyML
Traditional threshold-based systems generate false alarms from sensor noise, temperature drift, and temporary disturbances. Our Edge AI approach provides intelligent filtering and anomaly detection.

**Training Data Collection:**
The system collects labeled training data through multiple deployment phases:
1. **Clean baseline**: Establish normal parameter ranges for specific locations
2. **Known events**: Record sensor responses during controlled conditions
3. **Environmental variations**: Capture diurnal cycles, weather impacts, seasonal changes

**Model Architecture:**
We implement a lightweight neural network using TensorFlow Lite for Microcontrollers:

```cpp
// Edge Impulse generated model integration
#include "model_variables.h"
#include "model.h"

// Feature extraction from sensor readings
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

void extractFeatures(SensorData* data) {
    // Statistical features from sliding window
    features[0] = data->ph_mean;
    features[1] = data->ph_std;
    features[2] = data->ec_mean;
    features[3] = data->ec_trend;
    features[4] = data->turbidity_spike;
    features[5] = data->temp_rate_change;
    // ... additional engineered features
}

WaterQualityClassification runInference(float* features) {
    signal_t signal;
    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    signal.get_data = &get_signal_data;
    
    ei_impulse_result_t result = { 0 };
    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
    
    // Return classification with confidence
    return parseResults(&result);
}
```

### Data Quality Assurance

**Multi-Sample Filtering:**
Instead of single readings, the system collects 10-20 samples per sensor over 30 seconds, applying median filtering to remove outliers:

```cpp
float readSensorRobust(int pin, int samples = 15) {
    float readings[samples];
    
    // Collect multiple readings
    for (int i = 0; i < samples; i++) {
        readings[i] = analogRead(pin) * (3.0f / 1023.0f);
        delay(50);  // Allow ADC settling
    }
    
    // Apply median filter
    std::sort(readings, readings + samples);
    return readings[samples / 2];  // Return median
}
```

**Temperature Compensation:**
All sensors require temperature compensation for accurate readings:

```cpp
float compensateEC(float ec_raw, float temp_c) {
    // Standard 2%/°C compensation to 25°C reference
    float alpha = 0.02f;  // Temperature coefficient
    float ec25 = ec_raw / (1.0f + alpha * (temp_c - 25.0f));
    
    // Additional polynomial correction for probe characteristics
    float compensation_factor = 1.0f + 0.0001f * pow(temp_c - 25.0f, 2);
    return ec25 * compensation_factor;
}
```

## Edge Impulse Integration: TinyML Pipeline

### Data Acquisition Setup
Following Edge Impulse's data forwarder protocol for efficient model training:

```cpp
void streamTrainingData() {
    Serial.println("EI_START");
    
    while (training_mode) {
        SensorData data = collectSensorReadings();
        
        // Format for Edge Impulse ingestion
        Serial.print("pH:");
        Serial.print(data.ph, 3);
        Serial.print(",EC:");
        Serial.print(data.ec, 1);
        Serial.print(",Turbidity:");
        Serial.print(data.turbidity, 2);
        Serial.print(",Temp:");
        Serial.println(data.temperature, 2);
        
        delay(100);  // 10Hz sampling for training
    }
}
```

### Model Training Workflow
1. **Data Collection**: Deploy sensors in known conditions (clean, polluted, transitional)
2. **Feature Engineering**: Extract statistical and temporal features from raw sensor streams
3. **Model Training**: Use Edge Impulse Studio for neural network optimization
4. **Quantization**: Convert to 8-bit integer models for microcontroller deployment
5. **Validation**: Test accuracy against held-out data and field conditions

### Real-Time Classification
The trained model runs every sampling cycle, providing:
- **Anomaly scores**: Confidence that current readings are abnormal
- **Classification**: Normal/Caution/Alert categories
- **Trend analysis**: Rising/falling pollution indicators

```cpp
typedef struct {
    float anomaly_score;     // 0.0-1.0, higher = more unusual
    uint8_t classification;  // 0=Normal, 1=Caution, 2=Alert
    float confidence;        // Model confidence in classification
    bool trend_rising;       // Pollution trend direction
} AIAnalysis;

AIAnalysis analyzeWaterQuality(SensorData* data) {
    // Extract features for ML model
    extractFeatures(data);
    
    // Run inference
    WaterQualityClassification result = runInference(features);
    
    // Post-process results
    AIAnalysis analysis;
    analysis.anomaly_score = result.anomaly;
    analysis.classification = result.category;
    analysis.confidence = result.confidence;
    analysis.trend_rising = (data->ec_trend > 0.1f || data->turbidity_trend > 0.1f);
    
    return analysis;
}
```

## Communication Protocol: Efficient Data Uplink

### LoRaWAN Payload Optimization
With LoRaWAN's strict payload limits (51 bytes max), every bit counts:

```cpp
typedef struct __attribute__((packed)) {
    uint16_t temperature;    // 0.1°C resolution, -40 to +85°C
    uint16_t ph;            // 0.01 pH resolution, 0-14 range
    uint16_t ec;            // 1 µS/cm resolution, 0-65535 range
    uint16_t turbidity;     // 0.1 NTU resolution, 0-6553.5 range
    uint16_t battery;       // 0.01V resolution, 0-6.55V
    uint8_t status;         // Bit-packed: 3 bits status, 5 bits flags
    uint8_t ai_confidence;  // AI model confidence 0-255
    uint16_t uptime_hours;  // Hours since last reset
} WaterQualityPayload;  // Total: 16 bytes

void transmitLoRaWAN(ProcessedData* data, AIAnalysis* ai) {
    WaterQualityPayload payload;
    
    // Pack data with scaling
    payload.temperature = (uint16_t)((data->temperature + 40.0f) * 10.0f);
    payload.ph = (uint16_t)(data->ph * 100.0f);
    payload.ec = (uint16_t)(data->ec);
    payload.turbidity = (uint16_t)(data->turbidity * 10.0f);
    payload.battery = (uint16_t)(data->battery_voltage * 100.0f);
    
    // Pack status bits
    payload.status = (ai->classification & 0x07) |           // 3 bits: status
                     ((data->calibration_valid ? 1 : 0) << 3) | // 1 bit: cal valid
                     ((ai->trend_rising ? 1 : 0) << 4);          // 1 bit: trending
    
    payload.ai_confidence = (uint8_t)(ai->confidence * 255.0f);
    payload.uptime_hours = (uint16_t)(millis() / 3600000UL);
    
    // Transmit via Wio-E5
    if (wio_e5.send((uint8_t*)&payload, sizeof(payload))) {
        last_transmission = millis();
        transmission_failures = 0;
    } else {
        transmission_failures++;
    }
}
```

### TTN Decoder Function
The Things Network decoder unpacks the binary payload:

```javascript
function decodeUplink(input) {
    var data = input.bytes;
    var decoded = {};
    
    // Unpack with proper scaling
    decoded.temperature = ((data[0] << 8) | data[1]) / 10.0 - 40.0;
    decoded.ph = ((data[2] << 8) | data[3]) / 100.0;
    decoded.conductivity = (data[4] << 8) | data[5];
    decoded.turbidity = ((data[6] << 8) | data[7]) / 10.0;
    decoded.battery = ((data[8] << 8) | data[9]) / 100.0;
    
    // Unpack status bits
    var status = data[10];
    decoded.water_status = ['Safe', 'Caution', 'Danger'][status & 0x07];
    decoded.calibration_valid = (status & 0x08) ? true : false;
    decoded.pollution_rising = (status & 0x10) ? true : false;
    
    decoded.ai_confidence = data[11] / 255.0;
    decoded.uptime_hours = (data[12] << 8) | data[13];
    
    return {
        data: decoded,
        warnings: [],
        errors: []
    };
}
```

## Power Management: Maximizing Autonomy

### Adaptive Sampling Strategy
The system adjusts sampling frequency based on conditions and battery level:

```cpp
uint32_t calculateSamplingInterval() {
    uint32_t base_interval = 600000;  // 10 minutes default
    
    // Increase frequency during pollution events
    if (current_status > 0) {
        base_interval /= 2;  // Sample every 5 minutes
    }
    
    // Reduce frequency when battery low
    if (battery_voltage < 3.6f) {
        base_interval *= 2;  // Extend to 20 minutes
    }
    
    // Night mode: reduce sampling 10pm-6am
    if (isNightTime()) {
        base_interval *= 3;  // Extend to 30 minutes
    }
    
    return base_interval;
}
```

### Sleep State Management
Precise power state control maximizes battery life:

```cpp
void enterDeepSleep(uint32_t sleep_ms) {
    // Disable all peripherals
    digitalWrite(SENSOR_POWER_EN, LOW);
    strip.clear();
    strip.show();
    
    // Configure wake sources
    nrf_gpio_cfg_sense_set(WAKEUP_PIN, NRF_GPIO_PIN_SENSE_LOW);
    
    // Enter System OFF mode (lowest power)
    sd_power_system_off();
    
    // Execution resumes here after wake
}
```

## Open Source & Community Features

### Calibration Interface
Simple web-based calibration via Bluetooth:

```cpp
void handleCalibrationMode() {
    if (bluetooth_connected) {
        // Stream real-time readings for calibration
        StaticJsonDocument<200> doc;
        doc["ph_raw"] = ph_voltage;
        doc["ph_calibrated"] = ph_value;
        doc["ec_raw"] = ec_voltage;
        doc["temperature"] = water_temp;
        
        String json_output;
        serializeJson(doc, json_output);
        ble_uart.println(json_output);
    }
}
```

### Field Diagnostic Tools
Built-in self-test capabilities assist field deployment:

```cpp
bool runSystemDiagnostics() {
    bool all_ok = true;
    
    // Test each sensor
    all_ok &= testSensor("DS18B20", testTemperatureSensor());
    all_ok &= testSensor("pH", testPHSensor());
    all_ok &= testSensor("EC", testECSensor());
    all_ok &= testSensor("Turbidity", testTurbiditySensor());
    
    // Test communication
    all_ok &= testSensor("LoRa", testLoRaConnection());
    
    // Test power system
    all_ok &= testSensor("Solar", testSolarCharging());
    all_ok &= testSensor("Battery", testBatteryHealth());
    
    return all_ok;
}
```

## Future Software Enhancements

**Mesh Networking**: Multi-node coordination for pollution event triangulation
**Predictive Analytics**: Time-series forecasting of water quality trends  
**Federated Learning**: Collaborative model improvement across deployments
**Integration APIs**: Direct connection to citizen science platforms and regulatory databases

This software architecture transforms raw sensor data into actionable environmental intelligence while maintaining the simplicity and accessibility essential for community-driven environmental monitoring.

