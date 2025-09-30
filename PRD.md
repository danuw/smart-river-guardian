# Smart River Guardian

*Cold water swimming is good for you they say… They also say our rivers are bad and dangerous, wouldn't it be great to know at a glance?*

**Published Project**: [Smart River Guardian on Hackster.io](https://www.hackster.io/danuw/smart-river-guardian-48cadc)

## Story

Water quality in the UK is frequently in the news – sewage spills, algal blooms and debates about whether the River Seine in Paris is fit for Olympic swimming remind us that clean water isn't guaranteed anywhere. When my kids go sailing at the Welsh Harp reservoir, the water often looks and smells bad. Sailing doesn't require full immersion like swimming, but falls happen, and there's no way to tell whether the water is safe (that is also the most fun part if you ask me!).

So a real‑time indicator would reassure us when conditions are good and help us push for improvement when they're not.

That's where the Smart River Guardian comes in: a simple, modular, edge-AI powered device that reads water quality and gives a quick **RAG (Red–Amber–Green)** signal. It's not perfect yet, but it's already splashing with potential!

## Things Used in This Project

### Hardware Components
- Seeed Studio XIAO nRF52840 Sense (XIAO BLE Sense) × 1
- TDS Water Sensor × 1
- Water Conductivity Sensor × 1  
- pH Sensor Probe × 1
- WS2812B Led Strip × 1
- DS18B20 Temperature Sensor 1m × 2
- SX1262 Lora module for Xiao series × 1
- Rechargeable Battery, 3.7 V × 1
- 6W Solar Panel × 1

### Software Apps and Online Services
- Arduino IDE
- Edge Impulse Studio

### Hand Tools and Fabrication Machines
- 3D Printer (generic) - I use a Creality CR6-SE
- Mastech MS8217 Autorange Digital Multimeter - Essential so you don't use all your hair at once, on a single project!

## The Journey

### 1. Planning (keep it simple)

I started with the essentials:

- Sensors → pH, TDS, temperature
- Feedback → a NeoPixel ring to give instant *safe / careful / avoid* visual status
- Brains → Seeed XIAO nRF52840 (good for TinyML, low-power, onboard charging)
- Aspirations → SX1262 LoRa, solar charging with LiPo Rider Pro + MPPT, custom PCB (thanks NextPCB), and even the new XIAO nRF54L15 with Zephyr for future exploration

### 2. Building (multimeter = best friend)

I went one component at a time, testing, debugging, and making sure each worked before adding the next.

- First the temperature sensor, then TDS, then pH
- NeoPixel added for simple visual and instant feedback
- Arduino code incrementally updated (see [GitHub repo](https://github.com/danuw/smart-river-guardian))

### 3. Collecting Data (tap, bottle, Coke)

Once the setup worked, I exported data streams into Edge Impulse.

- Trained with three liquids: tap water, mineral water, Coca-Cola
- No real river water yet → so this is just proof-of-concept
- Calibrations were rough, but enough to classify

Edge Impulse project is available at https://studio.edgeimpulse.com/studio/789893

### 4. Training & Inference

- Trained a quick model (yes, the video is wobbly!)
- Deployed back to the XIAO → real-time predictions on the edge
- NeoPixel shows Green = Safe, Amber = Careful, Red = Avoid

### 5. What's Next

- A tidier 3D-printed case (instead of jumper-wire-spaghetti!)
- Collect real river data → refine the model
- PCB design with NextPCB → plug-and-play, solar-ready version
- Add LoRa for remote river health monitoring
- Future exploration with nRF54L15 + Zephyr + TinyML

### Conclusion

The Smart River Guardian is a playful but serious attempt at showing how Edge AI + simple sensors can make our rivers more transparent (literally!). Today it classifies "tap vs Coke," tomorrow it might keep swimmers, rowers, and fishers safe.

Thanks to NextPCB for extending their support — the journey from messy breadboard to sleek PCB has only just begun.

## Wiring Diagram

Shows how the sensors are connected to the Xiao nrf52840 in the middle and how it would connect to LoRa SX1262 module and LiPo Rider Plus to manage the solar charging.

## Useful Links

- [Seeed XIAO BLE Software Setup](https://wiki.seeedstudio.com/XIAO_BLE/#software-setup)
- [XIAO BLE Battery Charging Considerations](https://wiki.seeedstudio.com/XIAO_BLE/#q3-what-are-the-considerations-when-using-xiao-nrf52840-sense-for-battery-charging)
- [Seeed Studio XIAO nRF52840 Sense Overview](https://wiki.seeedstudio.com/XIAO_BLE/#seeed-studio-xiao-nrf52840-sense)
- [TDS Sensor Arduino Interfacing](https://how2electronics.com/tds-sensor-arduino-interfacing-water-quality-monitoring/)
- [Edge Impulse CSV Data Collection Tool](https://github.com/edgeimpulse/tool-data-collection-csv)

## Credits

**Project Creator**: [Dan Benitah](https://www.hackster.io/danuw)

Thanks to Keith Newton and AI contributors.
