# Smart River Guardian — Hardware PRD

> **Purpose:** This document summarizes the hardware application entry and the latest component plan for the **Smart River Guardian** river‑monitoring node. It is structured to be copy‑pasteable into your repo and expandable for the Hackster submission.

---

## 1) Executive Summary
**Smart River Guardian** is a low-power, solar-charged, LoRa-connected river monitoring node that measures **water temperature, pH, turbidity, and conductivity (EC/TDS)**, then signals local safety with a **NeoPixel LED strip** (Safe / Careful / Avoid) and pushes data upstream via **LoRa** for mapping and alerts. The device is built around a **Seeed Studio XIAO nRF52840** for ultra-low-power operation and a compact footprint, paired with a purpose-built carrier PCB for clean wiring, waterproof connectors, and robust power management. The design is open, low-cost, and replicable for communities, sailing clubs, and local councils.

**What’s novel**  
- **Truly off-grid**: solar-powered with proper Li-ion charge management and ultra-low sleep current.  
- **Field-friendly**: a single enclosure with IP-rated glands, color-coded “Safe/Careful/Avoid” LED feedback, and quick-swap probes.  
- **LoRa uplink**: works where cellular coverage is poor; optional LoRaWAN for city coverage or point-to-point LoRa for short hops.  
- **Quality-first**: temperature compensation, per-probe calibration, oversampling, and on-device sanity checks to reduce garbage data.

**Community impact**  
- Real-time local readability (LED) improves safety for swimmers/rowers/sailors.  
- Open design enables citizen science and scalable deployments along long river sections.  
- Data can support remediation actions and track improvement efforts.

---

## 2) Problem & Use Case
Untreated overflows and diffuse pollution events are often invisible. Recreational users don’t have live, local water quality information. Existing monitoring is sparse or delayed. We need **cheap, robust, low-power** nodes that can be deployed in multiple locations and work **off-grid** while providing **instant local feedback** and **uplinking** data for dashboards and alerts.

**Primary user stories**
- **Sailor / club volunteer** checks the mast-mounted LED bar: green = sail, amber = caution (high turbidity or elevated EC/TDS), red = avoid (out-of-range pH or very high turbidity).  
- **River trust** deploys a chain of nodes and reviews LoRa uplink data to identify hotspots and trends.  
- **Community scientist** calibrates probes monthly and contributes data to an open map.

---

## 3) Goals, Non-Goals, and Success Criteria
**Goals**
- Off-grid sensing and uplink of temperature, pH, turbidity, and EC/TDS.  
- Local, glanceable status via NeoPixel strip (Safe/Careful/Avoid).  
- Open-source hardware, firmware, and documentation.

**Non-Goals (initial MVP)**
- No cellular backhaul in MVP (can be added via optional bridge).  
- No on-node ML beyond thresholds/filters.  
- No automated anti-fouling hardware (manual maintenance first).

**Success Criteria**
- ≥ 2 weeks autonomous operation in mixed UK weather.  
- LoRa uplink success rate ≥ 95% for scheduled transmissions.  
- Field calibration repeatability: pH ±0.1, EC cell-constant within spec, stable turbidity zero.  
- End-to-end: sensor → LED category → LoRa payload received and decoded.

---

## 4) Latest Component Plan (Preferred Parts & Rationale)

### 4.1 Core MCU & IO
- **MCU:** *Seeed Studio XIAO nRF52840* (or XIAO nRF52840 Sense if you want IMU/mic for future features)  
  - Tiny footprint, 3.3V logic, very low sleep current, Arduino/Zephyr compatible.
- **Carrier PCB (custom, NextPCB):**  
  - XIAO “socket” footprint  
  - Grove/JST-PH headers for sensors and power  
  - BNC panel footprints or pigtail headers for pH & EC probes (depending on sensor boards)  
  - Battery voltage divider to read state of charge  
  - MOSFET power-gating for analog sensor boards & LED strip  
  - TVS diodes, reverse-polarity protection, ESD on sensor inputs

### 4.2 Comms (LoRa)
- **Option A (LoRaWAN):** *Seeed Grove Wio-E5* module (UART AT command set) → easiest WAN uplink to TTN/Helium  
- **Option B (Raw LoRa / Meshtastic-ready):** SX1262 module (e.g., Ebyte E22-900M30S for EU868) on SPI → flexible firmware (RadioLib), P2P links
- **Antenna:** External 868 MHz whip with SMA + bulkhead to enclosure

### 4.3 Power & Solar
- **Battery:** 3.7V Li-ion/LiPo, **2000–5000 mAh** (pick based on duty cycle and winter margin)  
- **Solar Panel:** **6V, 2–6W** (UK winter margin favors ≥3W)  
- **Charger:** Dedicated **solar Li-ion charger** (with MPPT-like behavior or solar-aware input), e.g.:  
  - Seeed/compatible “Solar LiPo charger” boards (CN3065/TP4056-derived with solar optimization) or BQ24074-based board  
- **Regulation:**  
  - 5V boost (for sensors/LED if required)  
  - 3.3V LDO or buck for logic  
- **Power switching:** High-side P-MOSFET to completely disable analog boards & LED strip during sleep

### 4.4 Sensing (Water)
- **Temperature (water):** *Waterproof DS18B20* (1-Wire), simple & reliable; used for temp compensation  
- **pH:** Analog pH interface board + BNC probe (Grove/Gravity-style)  
- **EC/TDS:** Analog EC interface board + K=1 probe (Grove/Gravity-style). Report both **EC** (µS/cm) and derived **TDS** (ppm)  
- **Turbidity:** Analog turbidity sensor (Grove/Gravity-style, IR LED + photodiode)  

> *Rationale:* Gravity/Grove-style analog conditioner boards handle probe impedance and buffering so the XIAO’s ADC sees stable, scaled voltages. They also simplify field wiring.

### 4.5 Visual Feedback
- **LED strip:** *NeoPixel/WS2812B* (short 8–16 LED segment) for at-a-glance status  
  - **Safe** (green), **Careful** (amber), **Avoid** (red)  
  - Data level-shift from 3.3V → 5V (74AHCT125 or similar) for signal integrity  
  - Gated power to cut idle draw

### 4.6 Enclosure & Cabling
- **Enclosure:** IP67/IP68 polycarbonate box with clear lid (optional)  
- **Glands:** IP68 cable glands for probe cables & solar leads  
- **Mounting:** 316 stainless bracket/strap for handrails or posts  
- **Connectors:** JST-PH/Grove inside; BNC panel for pH/EC if supported by sensor board

---

## 5) Reference Thresholds (initial; to be tuned with data)
- **pH**: Safe 6.5–8.5 | Careful 6.0–6.5 or 8.5–9.0 | Avoid <6.0 or >9.0  
- **Turbidity (NTU)**: Safe <10 | Careful 10–50 | Avoid >50  
- **EC (µS/cm)**: Site-specific; begin with Safe <1500 | Careful 1500–3000 | Avoid >3000  
- **Temperature (°C)**: Mostly for compensation & context; optional flags for heat spikes

*LED shows the worst category among active metrics.*

---

## 6) Power Budget (indicative)
- **Sleep** (MCU + RTC): ~20–60 µA (XIAO + gated sensors off)  
- **Sense burst** (~2–3 s): 20–80 mA (analog boards on; ADC sampling/averaging)  
- **LoRa TX** (1–2 s): 60–120 mA (duty ≤ 1%)  
- **LED burst** (status 2–3 s at low brightness): 20–150 mA depending on LED count/brightness

**Duty cycle example (every 5–10 minutes):**  
Daily energy ≈ 20–80 mWh (node-only, conservative). A **3–6W panel** with a few hours of UK winter sun provides ample margin for self-sufficiency with a **2–5Ah** cell.

---

## 7) Firmware Plan (Arduino/PlatformIO on XIAO nRF52840)
1. **Boot & self-test**: read battery, check probe presence, brief LED pattern.  
2. **Sampling cycle** (interval configurable):  
   - Power on analog rails → settle 500–1000 ms  
   - Read DS18B20; read raw pH, EC, turbidity multiple times → median/mean filter  
   - Apply **temperature compensation**:  
     - EC at 25 °C: EC25 = ECmeasured / (1 + α(T−25)), α≈0.02/°C (tune per probe)  
     - Optional pH temperature slope adjustment  
   - Convert EC→TDS using factor (site-tuned, start at 0.5–0.7)  
   - Compute **status category** (Safe/Careful/Avoid) and drive LED (short, low-brightness burst)  
3. **Uplink**:  
   - **LoRaWAN (Wio-E5)**: send compact payload (see schema below)  
   - **or Raw LoRa**: send ASCII/CBOR/Proto packet to a nearby bridge  
4. **Sleep**: turn off sensor rail & LED, enter deep sleep.

**Libraries:** OneWire/DS18B20, Adafruit_NeoPixel, simple median filter, TinyCRC, Wio-E5 or RadioLib (depending on LoRa option).

---

## 8) Payload Schema (compact example)
```json
{
  "t": 23.7,         "ph": 7.62,      "ec": 945,
  "tds": 520,        "ntu": 13.4,     "vbat": 3.99,
  "stat": 1,         "fw": 3
}
```
- `stat`: 0=Safe, 1=Careful, 2=Avoid  
- LoRaWAN payload packed to bytes (e.g., int16/int32 scaled); **FPort** indicates schema version.

---

## 9) Calibration & Data Quality
- **pH**: 2- or 3-point using buffer solutions (pH 4.0, 7.0, 10.0). Store offsets/slopes in EEPROM/flash.  
- **EC**: 1413 µS/cm solution to set **cell constant**; temperature compensate to 25 °C.  
- **Turbidity**: zero in clean water; optional NTU reference standards.  
- **Warm-up & settling**: ignore first samples after sensor rail on; oversample thereafter.  
- **Outlier handling**: median filtering; reject sudden deltas beyond physical plausibility; flag probe faults.  
- **Drift**: monthly quick recalibration recommended.

---

## 10) Mechanical & Waterproofing
- IP67 enclosure; probes pass through glands (strain relief).  
- Keep analog boards inside enclosure; only send probe cables outside.  
- Desiccant pouch, conformal coating on PCB, gaskets checked annually.  
- Add drip loop on cables, keep enclosure above splash line.

---

## 11) Manufacturing Plan (with NextPCB vouchers)
> Voucher: **$300 PCB + $200 SMT** (~2 PCBA prototypes; shipping/customs not covered).

**Board contents**
- XIAO footprint, Wio-E5/SX1262 footprint (or header), battery gauge divider  
- 5V boost + 3.3V LDO/buck, solar charger footprint, power-gating MOSFETs  
- Headers for DS18B20, turbidity, pH, EC (Grove/JST), BNC pads or pigtails  
- ESD/TVS on external lines; test pads/UART SWD pads

**Assembly split**
- **SMT**: regulators, passives, MOSFETs, level-shifter, ESD, headers (as feasible)  
- **Hand**: through-hole connectors, BNCs, antenna SMA, standoffs

---

## 12) Project Timeline (minimum viable → fieldable)
1. **Week 1–2**: Breadboard bring-up (XIAO + sensors + Wio-E5), basic readings, LED status, serial logs.  
2. **Week 3**: LoRa uplink (join & send), payload packing, gateway/dashboard mock.  
3. **Week 4**: Carrier PCB design & DFM, submit to NextPCB.  
4. **Week 5**: Enclosure build, waterproofing, dry runs; calibration rig prep.  
5. **Week 6**: Field pilots (2 nodes), data validation, threshold tuning, write-up for Hackster.

---

## 13) Risks & Mitigations
- **Probe fouling/biofilm** → monthly rinse; data QC; turbidity spike heuristics.  
- **Power shortfall in winter** → larger panel (≥3W), longer reporting interval, deeper sleep.  
- **LoRa coverage gaps** → raw LoRa hop to a nearby bridge or place gateways at clubhouses.  
- **Analog noise/offsets** → shielded cables, proper grounding, median filter, per-probe calibration.  
- **LED power draw** → short, low-brightness pulses; power-gate strip.

---

## 14) Open Source & Docs
- **License**: MIT/Apache-2.0 (choose).  
- **Docs**: pin map, wiring guides, calibration procedure, payload decoder examples for TTN.  
- **Repeatability**: full BOM + Gerbers + firmware in repo; Hackster post for assembly steps.

---

## 15) Bill of Materials (indicative; customize per supplier)
- Seeed **XIAO nRF52840** (MCU)  
- **LoRa**: Seeed **Grove Wio-E5** (LoRaWAN via UART) *or* SX1262 (E22-900M30S) + SPI  
- **Solar panel**: 6V, 3–6W (poly/mono)  
- **Solar Li-ion charger**: solar-aware LiPo charger (CN3065/BQ24074-class)  
- **Battery**: 3.7V Li-ion/LiPo 2000–5000 mAh with JST-PH  
- **Power**: 5V boost; 3.3V LDO/buck; P-MOSFET high-side switch  
- **Sensors**:  
  - DS18B20 waterproof temp probe  
  - pH analog board + pH probe (BNC)  
  - EC analog board + EC probe (K=1, BNC)  
  - Turbidity analog sensor board (IR)  
- **LED**: NeoPixel (WS2812B) 8–16 LEDs + 74AHCT125 level shifter  
- **Enclosure**: IP67 box, glands, SMA bulkhead, antenna  
- **PCB**: custom carrier (NextPCB), standoffs, screws  
- **Calibration kits**: pH buffers (4/7/10), EC 1413 µS/cm, turbidity standards (optional)

---

## 16) Quick Pin Map (example; confirm in schematic)
- **DS18B20** → D2 (with 4.7k pull-up)  
- **Turbidity** → A0 (analog)  
- **pH** → A1 (analog)  
- **EC** → A2 (analog)  
- **NeoPixel** → D6 (via 74AHCT125 to 5V rail)  
- **Wio-E5** → UART (D0/D1 or Serial1), plus 3.3V and EN pin if used  
- **Battery sense** → A3 (through divider, e.g., 100k/330k)  
- **Sensor Power Enable** → D9 (to P-MOSFET gate driver)

---

## 17) Assembly & Field Bring-Up (minimum steps)
1. Flash base firmware → verify serial output & sleep current.  
2. Calibrate pH (two-point), EC (1413 µS/cm), check turbidity zero.  
3. Join LoRaWAN → confirm uplink on gateway/TTN; validate decoder.  
4. Mount in enclosure → route probes through glands; strain-relief cables.  
5. Expose to sunlight; confirm charge current and nightly continuity.  
6. Tune thresholds from first week of field data; set LED bands.

---

*End of PRD.*
