# 🪖 SILENTLINK - Dash Code Communicator 🚀✨

![Project Badge](https://img.shields.io/badge/status-active-brightgreen) 
![Tech Stack](https://img.shields.io/badge/tech-ESP32%20|%20Node.js%20|%20MongoDB-blue)

---

## 🌟 Project Overview
**Dash Code Communicator** is an innovative wireless communication system designed for quick, efficient, and secure message transmission using a unique dash-based coding system. It combines hardware 🛠️ and software 💻 components to provide a seamless messaging experience for IoT and real-world applications, especially where conventional typing or texting may not be practical.

---

## ❗ Problem Statements
- ⏳ **Slow manual communication:** Typing long messages can be time-consuming in constrained or high-pressure environments.  
- 📶 **Offline communication limitations:** Standard messaging apps require internet connectivity.  
- ⚡ **Signal ambiguity:** Existing wireless text communication systems can suffer from delays or misinterpretation.  
- 🔧 **Hardware constraints:** Many setups require multiple input devices or complex configurations.  
- 📈 **Scalability challenges:** Real-time communication across multiple devices while keeping it simple is difficult.

---

## 💡 Our Solutions
- 📝 Developed a **custom Dash Code mapping** system for faster transmission (2–3 symbols per character).  
- 🔘 Built a **single-button hardware transmitter** using ESP32 for taps and long presses.  
- 📡 Implemented a **wireless receiver** to decode messages in real-time.  
- 🖥️ Integrated a **Node.js + Express backend** for message handling and storage.  
- ☁️ Used **MongoDB Atlas** for persistent device and message data.  
- ⚡ Added **Server-Sent Events (SSE)** for live updates on the frontend.  
- 🏗️ Designed a **modular architecture** for easy scalability and maintainability.

---

## 🛠️ Components
### Hardware
- ESP32 Microcontroller 🤖  
- Double Push Button 🔘  
- LED/Display (optional for feedback 💡)

### Software
- Node.js + Express backend 🖥️  
- MongoDB Atlas database ☁️  
- Frontend: HTML, CSS, JS 🌐  
- Server-Sent Events (SSE) for real-time updates ⚡  
- Dash Code Mapping system 📝

---

## ⚙️ Setup & Installation

### 1. Clone the Repository
```bash
git clone https://github.com/yourusername/silentlink.git
cd silentlink
```
### 2. Frontend Setup

The frontend is served via Express at http://localhost:5000.

Open the browser to view live messages and interact with the receiver interface.

## 3. Hardware (ESP32 Transmitter)

Flash the ESP32 with the provided Arduino/ESP32 sketch from Dash_code folder.

Connect the push buttons to the designated GPIO pins.

Connect ESP32 to WiFi (update ssid & password in the code).

## 4. Database (MongoDB Atlas)

Create a free cluster on MongoDB Atlas
.

Update .env or config.js with your MongoDB connection string.

🏃 Usage Instructions
---------------------

1.  **Power on ESP32 transmitter** and press the buttons to send dash-coded messages.
    
2.  **Receiver (web interface)** will automatically display messages in real-time using SSE.
    
3.  **Message storage** is handled in MongoDB Atlas; all past messages can be retrieved from the database.
    
4.  **Decoding dash code:** The frontend automatically converts dash sequences into readable text.
    
5.  **Multiple devices:** Each ESP32 transmitter can be registered with the backend for separate identification.

🎯 Use Case Scenarios
---------------------

1.  🚨 **Emergency Communication:** Quick messages in offline areas.
    
2.  📟 **IoT Device Messaging:** Device status updates using dash code.
    
3.  ♿ **Accessible Communication:** Single-button messaging for limited mobility users.
    
4.  🏫 **Offline Social Messaging:** Classrooms, workshops, or events.
    
5.  📚 **Learning Tool:** Understanding Morse-like coding and wireless communication.

🔮 Future Scope
---------------

*   🔘 Multi-button input for faster encoding.
    
*   📱 Mobile app integration for direct interface.
    
*   🤖 AI-based predictive decoding for error correction.
    
*   🔒 Encrypted dash code transmission for security.
    
*   🌐 Network scaling for multi-device peer-to-peer communication.
    
*   🎮 Gamification and educational platforms using dash code.

🏁 Conclusion
-------------

**Dash Code Communicator** addresses the need for fast, reliable, and accessible communication in offline or constrained environments. Its modular hardware-software ecosystem provides a foundation for future expansion into AI, mobile, and educational applications.

💖 Made by Code Commandos
