# Secure ESP-to-ESP Communication System Architecture

## 1. Project Overview

This project enables **secure communication between two ESP32 devices** via a backend server. All messages are **encrypted** before storage and transmission, ensuring only the intended receiver can read them. A web dashboard provides role-based access:

- **Camp Leader:** Monitor all ESP communications.
- **Soldier:** Access only their own chat history.

**Tech Stack (Free & Hackathon-Friendly):**
- **Backend:** Node.js + Express (MERN stack)
- **Database:** Firebase (Free Plan)
- **Frontend:** React
- **Realtime:** HTTP requests or WebSocket (Socket.IO)

---

## 2. System Flow

**ESP Transmitter → Server → Encrypt → Firebase DB → Server → Decrypt → ESP Receiver**

**Steps:**
1. **ESP Transmitter:**  
   - Sends Morse code (converted to text) to backend via Wi-Fi (HTTP POST or WebSocket).
2. **Server (Node.js + Express):**  
   - Receives message, encrypts it, stores in Firebase, and forwards to receiver ESP.
3. **ESP Receiver:**  
   - Receives encrypted message, decrypts using shared key.
4. **Web Dashboard (React):**  
   - **Camp Leader:** Access to all messages.  
   - **Soldier:** Access to own chat history only.

---

## 3. User Roles & Access

| User Type      | Access Level                        |
|----------------|-------------------------------------|
| **Soldier**    | Only their own chat history         |
| **Camp Leader**| Full access to all ESP messages     |

---

## 4. Backend Architecture

### Components

- **Node.js + Express Server**
  - Handles authentication (JWT)
  - Receives, encrypts, and stores ESP messages in Firebase
  - Provides REST APIs for dashboard and ESPs

- **Firebase**
  - Stores encrypted messages and user info

- **Encryption Layer**
  - Symmetric encryption (AES) or custom lightweight algorithm
  - Shared key known only to server and receiver ESP

---

### Suggested Folder Structure

```
/backend
├── config/                # Firebase and environment configs
│   └── firebase.js
├── controllers/           # Route logic
│   ├── auth.controller.js
│   └── message.controller.js
├── middleware/            # Express middleware
│   └── auth.middleware.js
├── models/                # Data models (if needed)
│   └── user.model.js
├── routes/                # API routes
│   ├── auth.routes.js
│   └── message.routes.js
├── utils/                 # Utility functions
│   └── encryption.js
├── server.js              # Express entry point
└── .env                   # Environment variables
```

---

### API Design

| Route                       | Method | Description                                         | Access      |
|-----------------------------|--------|-----------------------------------------------------|-------------|
| `/api/auth/signup`          | POST   | Register soldier/leader                             | Public      |
| `/api/auth/login`           | POST   | Login and get JWT                                   | Public      |
| `/api/messages/send`        | POST   | Receive message from ESP, encrypt, store in DB      | JWT         |
| `/api/messages/fetch`       | GET    | Fetch messages for a soldier or leader              | JWT         |
| `/api/messages/fetch/all`   | GET    | Fetch all messages (leader only)                    | JWT (Leader)|

---

## 5. Security Measures

- **JWT Authentication** for dashboard and ESP endpoints
- **Password Hashing** using bcrypt
- **Message Encryption** for all ESP messages before storing in Firebase
- Only the **intended receiver ESP** can decrypt the message
- **Optional:** Periodic encryption key rotation

---

## 6. Free Deployment Plan

- **Backend:** Render / Railway (Free Plan)
- **Database:** Firebase (Free Plan)
- **Frontend:** Vercel / Netlify (Free Plan)

> **Result:** Fully free, deployable, and hackathon-ready system.

---

## 7. Additional Notes

- Real-time updates via **Socket.IO** or polling
- Easily scalable: add more ESPs as needed
- Encryption ensures **secure communication** even if the database is compromised

---
