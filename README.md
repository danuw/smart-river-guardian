
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

## Contact

**Project Creator**: [Dan Benitah](https://www.hackster.io/danuw)  
**Published Project**: [Smart River Guardian on Hackster.io](https://www.hackster.io/danuw/smart-river-guardian-48cadc)  
**Edge Impulse Project**: [TinyML model](https://studio.edgeimpulse.com/studio/789893)

## License

This project is open source under the MIT License. See the LICENSE file for details.
