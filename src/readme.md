Run the code:


Wiring:

- DS18B20 (temp) → D4 (with 4.7 kΩ pull-up to 3V3)
- TDS (Gravity board) → A0 (board powered at 5 V; analog out to A0)
- pH board (PO) → A2 (board powered at 5 V; analog out to A2)
- NeoPixel (5 LEDs) → D8 (power at 5 V; common GND; ~330 Ω in series on DIN recommended)

It:

- Prints Temp, TDS (raw/V/EC25/TDS ppm) and pH (raw/V/pH) once per second
- Uses manual EC→TDS calc (so no EEPROM/K value issues)
- Uses simple two-constant pH calibration (set your values at the top)
- Colors the LED strip Green/Amber/Red by TDS thresholds (choose scenario: drinking or river)

Arduino libraries to add:

- OneWire
- FlashStorage?
- DallasTemperature
- DFRobot_pH
- Adafruit_TinyUSB
- Adafruit_NeoPixel
- 