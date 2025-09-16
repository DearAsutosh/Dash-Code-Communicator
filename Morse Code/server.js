const express = require("express");
const bodyParser = require("body-parser");
const cors = require("cors");
const { fromDashCode } = require("./utils/dashcodeMap");

const app = express();
const PORT = 5000;

app.use(cors());
app.use(bodyParser.json());
app.use(express.static("public"));

// Store clients (for SSE)
let clients = [];
// Store decoded messages
let messages = [];

// Broadcast helper for SSE
function broadcast(data) {
  clients.forEach(client =>
    client.res.write(`data: ${JSON.stringify(data)}\n\n`)
  );
}

// Handle ESP32 POST requests
app.post("/dash", (req, res) => {
  const { espId, username, code } = req.body;

  if (!espId || !username || !code) {
    return res.status(400).json({ error: "espId, username, and code required" });
  }

  const decoded = fromDashCode(code);
  const timestamp = new Date().toISOString();

  const msg = {
    espId,
    username,
    raw: code,
    decoded,
    timestamp
  };

  messages.push(msg);
  broadcast(msg);

  console.log(`[${timestamp}] ${username} (${espId}) → ${decoded}`);
  res.json({ status: "ok", msg });
});

// SSE endpoint for live updates
app.get("/events", (req, res) => {
  res.setHeader("Content-Type", "text/event-stream");
  res.setHeader("Cache-Control", "no-cache");
  res.setHeader("Connection", "keep-alive");
  res.flushHeaders();

  // Send all previous messages on connect
  messages.forEach(m => {
    res.write(`data: ${JSON.stringify(m)}\n\n`);
  });

  clients.push({ res });
  req.on("close", () => {
    clients = clients.filter(c => c.res !== res);
  });
});

// Reset endpoint
app.get("/reset", (req, res) => {
  messages = [];
  res.json({ status: "reset done" });
});

app.listen(PORT, () => {
  console.log(`✅ DashCode Server running on http://localhost:${PORT}`);
});
