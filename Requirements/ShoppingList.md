# Project Hardware Requirements

## Must-Have (Required for a Working Demo)
- ESP32 DevKit V1 — 30-pin CP2102 (WROOM-32) ×2  
- Micro-USB Data Cable (USB-A to Micro-B) ×2  
- Breadboard (solderless, medium size, ~830 tie-points) ×1  
- Jumper wire kit (mixed: Male-Male, Male-Female, Female-Female, ~40–65 wires)  
- Tactile push button (momentary) ×2  
- Resistors:  
  - 10 kΩ ×5 (for pull-down/pull-up)  
  - 220 Ω ×5 (for LED series)  
  - 1 kΩ ×5 (for transistor base)  
- LEDs (5mm assorted) ×5  

## Strongly Recommended (Better Demo Clarity)
- Buzzer (active) OR Vibration motor (small) ×1  
- Transistors (e.g., 2N2222 or BC547) ×2  
- Flyback diodes (e.g., 1N4007) ×2  
- Multimeter (basic) ×1  

## Optional (Extra Features / Neat Assembly)
- Female header / header wires  
- USB Power Bank (for portable demo) ×1  
- Small OLED Display 0.96" (SSD1306 I2C) ×1  
- Small protoboard / tape / screws / double-sided foam tape  





---
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>

# Why Extra Components Are Needed

## 🔌 Resistors
1. **10 kΩ (pull-up / pull-down):**
   - **Reason:** Prevents the input pin from "floating" and giving random HIGH/LOW signals.
   - Without it → ESP32 may randomly detect button presses (false signals).

2. **220 Ω (for LEDs):**
   - **Reason:** Limits the current flowing through the LED and protects the ESP32 pin.
   - Without it → LED may burn out or the ESP32 pin may overheat.

3. **1 kΩ (for transistor base):**
   - **Reason:** Controls the current going into the transistor from the ESP32.
   - Without it → transistor or ESP32 pin can get damaged.

---

## 🔄 Flyback Diode (with transistor + motor/buzzer)
- **Reason:** Protects the ESP32 from voltage spikes caused when motors/buzzers are switched OFF.
- Without it → ESP32 can **get damaged or die instantly** due to high-voltage kickbacks.

---

## 📟 Multimeter
- **Reason:** Helps check voltages, continuity, and connections to quickly find problems.
- Without it → debugging takes hours of guesswork instead of minutes.
