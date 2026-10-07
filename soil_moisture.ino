#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ======================================================
// WiFi Configuration
// ======================================================

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ======================================================
// Soil Moisture Configuration
// ======================================================

#define SOIL_SENSOR_PIN A0

// Calibration values
// Change these after testing your particular sensor.
const int DRY_VALUE = 850;
const int WET_VALUE = 350;

// ======================================================
// Web Server
// ======================================================

ESP8266WebServer server(80);

// ======================================================
// Sensor Data
// ======================================================

int soilRaw = 0;
int soilPercent = 0;

// ======================================================
// Read Soil Moisture
// ======================================================

void readSoilMoisture() {

  soilRaw = analogRead(SOIL_SENSOR_PIN);

  /*
    Most resistive soil sensors work approximately like this:

    Dry soil  -> higher ADC value
    Wet soil  -> lower ADC value

    Therefore we map:
    DRY_VALUE -> 0%
    WET_VALUE -> 100%
  */

  soilPercent = map(
    soilRaw,
    DRY_VALUE,
    WET_VALUE,
    0,
    100
  );

  soilPercent = constrain(soilPercent, 0, 100);
}

// ======================================================
// Get Soil Status
// ======================================================

String getSoilStatus() {

  if (soilPercent <= 20) {
    return "Very Dry";
  }

  if (soilPercent <= 40) {
    return "Dry";
  }

  if (soilPercent <= 70) {
    return "Moist";
  }

  return "Very Moist";
}

// ======================================================
// API Endpoint
// ======================================================

void handleAPI() {

  readSoilMoisture();

  String json = "{";

  json += "\"soilRaw\":";
  json += soilRaw;

  json += ",\"soilPercent\":";
  json += soilPercent;

  json += ",\"status\":\"";
  json += getSoilStatus();
  json += "\"";

  json += ",\"uptime\":";
  json += millis();

  json += "}";

  server.send(
    200,
    "application/json",
    json
  );
}

// ======================================================
// Main Web Page
// ======================================================

void handleRoot() {

  String html = R"rawliteral(

<!DOCTYPE html>
<html lang="en">

<head>

<meta charset="UTF-8">

<meta
    name="viewport"
    content="width=device-width, initial-scale=1.0"
>

<title>Soil Moisture Monitor</title>

<style>

* {
    box-sizing: border-box;
    margin: 0;
    padding: 0;
}

body {

    font-family:
        -apple-system,
        BlinkMacSystemFont,
        "Segoe UI",
        Roboto,
        Arial,
        sans-serif;

    min-height: 100vh;

    color: #e5e7eb;

    background:
        radial-gradient(
            circle at top left,
            rgba(34, 197, 94, 0.12),
            transparent 35%
        ),
        radial-gradient(
            circle at bottom right,
            rgba(14, 165, 233, 0.10),
            transparent 35%
        ),
        #090d12;

    padding: 20px;
}

.container {

    width: 100%;
    max-width: 850px;

    margin: auto;
}

.header {

    text-align: center;

    margin-bottom: 24px;
}

.header h1 {

    font-size: clamp(25px, 5vw, 36px);

    font-weight: 700;

    letter-spacing: -0.5px;

    margin-bottom: 8px;
}

.header p {

    color: #94a3b8;

    font-size: 14px;
}

.status-dot {

    display: inline-block;

    width: 8px;
    height: 8px;

    margin-right: 6px;

    border-radius: 50%;

    background: #22c55e;

    box-shadow:
        0 0 10px rgba(34, 197, 94, 0.8);

    animation: pulse 2s infinite;
}

@keyframes pulse {

    0%, 100% {
        opacity: 1;
    }

    50% {
        opacity: 0.4;
    }
}

.card {

    background:
        rgba(255, 255, 255, 0.045);

    border:
        1px solid rgba(255, 255, 255, 0.08);

    border-radius: 22px;

    padding: 28px;

    margin-bottom: 18px;

    backdrop-filter: blur(15px);
    -webkit-backdrop-filter: blur(15px);

    box-shadow:
        0 20px 50px rgba(0, 0, 0, 0.25);
}

.main-reading {

    text-align: center;

    padding: 35px 25px;
}

.label {

    color: #94a3b8;

    font-size: 13px;

    text-transform: uppercase;

    letter-spacing: 1.5px;

    margin-bottom: 10px;
}

.moisture-value {

    font-size: clamp(65px, 15vw, 110px);

    line-height: 1;

    font-weight: 700;

    background:
        linear-gradient(
            135deg,
            #4ade80,
            #22c55e,
            #38bdf8
        );

    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;

    background-clip: text;

    margin: 8px 0;
}

.status {

    display: inline-flex;

    align-items: center;

    padding: 8px 16px;

    border-radius: 30px;

    margin-top: 15px;

    font-size: 14px;

    color: #86efac;

    background:
        rgba(34, 197, 94, 0.10);

    border:
        1px solid rgba(34, 197, 94, 0.20);
}

.progress-container {

    margin-top: 30px;
}

.progress-track {

    width: 100%;

    height: 12px;

    overflow: hidden;

    border-radius: 20px;

    background:
        rgba(255, 255, 255, 0.08);
}

.progress-bar {

    height: 100%;

    width: 0%;

    border-radius: inherit;

    background:
        linear-gradient(
            90deg,
            #f97316,
            #eab308,
            #22c55e,
            #38bdf8
        );

    transition:
        width 0.6s ease;
}

.progress-labels {

    display: flex;

    justify-content: space-between;

    margin-top: 8px;

    color: #64748b;

    font-size: 11px;
}

.grid {

    display: grid;

    grid-template-columns:
        repeat(2, 1fr);

    gap: 16px;
}

.info-card {

    padding: 22px;
}

.info-title {

    color: #64748b;

    font-size: 12px;

    text-transform: uppercase;

    letter-spacing: 1px;

    margin-bottom: 10px;
}

.info-value {

    font-size: 25px;

    font-weight: 600;
}

.info-unit {

    color: #64748b;

    font-size: 12px;

    margin-left: 4px;
}

.footer {

    text-align: center;

    padding: 20px 10px 10px;

    color: #64748b;

    font-size: 12px;
}

.github-link {

    display: inline-block;

    margin-top: 8px;

    color: #94a3b8;

    text-decoration: none;

    transition:
        color 0.25s ease,
        transform 0.25s ease;
}

.github-link:hover {

    color: #ffffff;

    transform: translateY(-2px);

    text-decoration: underline;
}

@media (max-width: 600px) {

    body {
        padding: 12px;
    }

    .card {
        padding: 20px;
    }

    .grid {
        grid-template-columns: 1fr;
    }

}

</style>

</head>

<body>

<div class="container">

    <div class="header">

        <h1>🌱 Soil Moisture Monitor</h1>

        <p>
            <span class="status-dot"></span>
            NodeMCU • Live Monitoring
        </p>

    </div>


    <div class="card main-reading">

        <div class="label">
            Soil Moisture
        </div>

        <div
            class="moisture-value"
            id="moisture"
        >
            --%
        </div>

        <div
            class="status"
            id="status"
        >
            Reading...
        </div>


        <div class="progress-container">

            <div class="progress-track">

                <div
                    class="progress-bar"
                    id="progress"
                ></div>

            </div>

            <div class="progress-labels">

                <span>Dry</span>

                <span>Moist</span>

                <span>Wet</span>

            </div>

        </div>

    </div>


    <div class="grid">

        <div class="card info-card">

            <div class="info-title">
                Sensor Reading
            </div>

            <div class="info-value">

                <span id="raw">
                    --
                </span>

                <span class="info-unit">
                    ADC
                </span>

            </div>

        </div>


        <div class="card info-card">

            <div class="info-title">
                Sensor Status
            </div>

            <div
                class="info-value"
                id="sensorStatus"
            >
                Online
            </div>

        </div>

    </div>


    <div class="footer">

        <div>
            ESP8266 Soil Moisture Monitoring System
        </div>

        <a
            href="https://github.com/unoanirban"
            target="_blank"
            rel="noopener noreferrer"
            class="github-link"
        >
            github.com/unoanirban
        </a>

    </div>

</div>


<script>

async function updateData() {

    try {

        const response =
            await fetch("/api/status");

        const data =
            await response.json();


        // Moisture percentage

        document.getElementById("moisture")
            .textContent =
            data.soilPercent + "%";


        // Progress bar

        document.getElementById("progress")
            .style.width =
            data.soilPercent + "%";


        // Status

        document.getElementById("status")
            .textContent =
            data.status;


        // Raw ADC value

        document.getElementById("raw")
            .textContent =
            data.soilRaw;


        // Connection status

        document.getElementById("sensorStatus")
            .textContent =
            "Online";

    }

    catch (error) {

        document.getElementById("sensorStatus")
            .textContent =
            "Offline";

    }

}


// Initial reading

updateData();


// Update every 2 seconds

setInterval(
    updateData,
    2000
);

</script>

</body>

</html>

)rawliteral";

  server.send(
    200,
    "text/html",
    html
  );
}

// ======================================================
// Setup
// ======================================================

void setup() {

  Serial.begin(115200);

  delay(100);

  Serial.println();
  Serial.println("================================");
  Serial.println(" Soil Moisture Monitoring System");
  Serial.println("================================");


  pinMode(
    SOIL_SENSOR_PIN,
    INPUT
  );


  // ----------------------------------------------------
  // Connect to WiFi
  // ----------------------------------------------------

  WiFi.mode(WIFI_STA);

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  Serial.print("Connecting to WiFi");

  while (
    WiFi.status() != WL_CONNECTED
  ) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("WiFi connected!");

  Serial.print("IP Address: ");

  Serial.println(
    WiFi.localIP()
  );


  // ----------------------------------------------------
  // Web Server Routes
  // ----------------------------------------------------

  server.on(
    "/",
    handleRoot
  );

  server.on(
    "/api/status",
    handleAPI
  );


  server.begin();

  Serial.println(
    "Web server started."
  );
}

// ======================================================
// Main Loop
// ======================================================

void loop() {

  server.handleClient();

}
