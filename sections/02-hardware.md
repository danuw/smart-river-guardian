# 02 — Hardware

Hardware selection & setup.

Draft content goes here.




How do we infer water quality?

We plan on using a number of sensors to help us monitor and measure the quality of the water.

### Water Temperature
Note that some of the measurement of those sensors may depend on temperature to infer the actual value, so we will start by adding a temperature sensor into the water.

### pH
What is pH: helps measure the level of acidity and bases in a liquid (for info, water tends to be close to pH7, lemon is acid and around pH3, and detergent is basic at around pH10)

https://how2electronics.com/ph-meter-using-ph-sensor-arduino-oled/

### TDS
TDS or Total Dissolved Solids indicates how many milligrams of soluble solids are dissolved in a liter of water. The higher the TDS value, the more soluble solids are dissolved in the water, and the less the water is clean.
BOM: 
- DS18b20
- 5V and 3V power supplies

https://how2electronics.com/tds-sensor-arduino-interfacing-water-quality-monitoring/

### Turbidity:
To measure Water Clarity
Dependencies:
- analog
- 5V

https://how2electronics.com/diy-turbidity-meter-using-turbidity-sensor-arduino/


### Dissolved Oxygen

Dependencies:
- 3V to 5V
- 90s to get a full response at 25 degrees


### nRF52840 Sense

https://cdn-reichelt.de/bilder/web/xxl_ws/A300%2FXIAO_NRF52840SE_PLUS_01.png?type=ProductXxl&

I wanted to use the small form factor the Xiao series, I needed TinyML capabilities, battery charging and low power.
nrf54L15 - I ended up getting a couple of those but decided not to use for this project yet as it uses Zephyr OS.
Esp32S3 Sense - I believe it was more power hungry.

(I later found some challenges such EEPROM not natively supported versus ESP32 boards, serial monitor/TinyUSB as well, ...)

Note: I thought I was sorted for battery charging with Solar panel, but sun being very irregular, I needed a charger capable of regulating the power/voltage not to damage the board and its components. I needed to get a MPTT, settled on the Seeed Lipo Rider charger series (I postponed its purchases having found a Lipo Amigo Pro at home)

### Solar Panel