
## 1. Problem Statement:

 In military and rescue operations, silent communication is often critical.
   Conventional wireless communication devices can be detected, jammed, or may
   draw unnecessary attention. A simple Morse code-based wearable communicator
   can provide a low-cost, silent, and effective solution for transmitting
   short messages.

## 2. Core Concept:
   - A soldier wears a small wrist device with a single push button.
   - Short press = dot (·), Long press = dash (–).
   - Each tap is transmitted wirelessly in real-time to a central system.
   - The central receiver (Node.js server) decodes the Morse code into
     human-readable text.
   - The decoded message is displayed on a web dashboard accessible to other
     soldiers or commanders.

## 3. System Architecture:

### a) Wearable Transmitter:
* ESP8266 microcontroller
* Push button input
* LED feedback (blinks for each tap)
* Powered by small LiPo battery or power bank

### b) Transmission:
* ESP8266 sends data (press duration + gap) to central server over Wi-Fi.

### c) Central Receiver & Decoder:
* Node.js server receives tap events
* Converts press durations into dots/dashes
* Decodes dots/dashes into letters/words
* Broadcasts decoded message to web clients

### d) Web Dashboard:
* Displays decoded messages in real-time
* Can be viewed on laptop, tablet, or phone