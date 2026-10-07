#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// =====================================================
// WiFi Credentials
// =====================================================

const char* ssid = "AndroidAP_4123";
const char* password = "12345678";

// =====================================================
// DHT11 Configuration
// =====================================================

#define DHTPIN D5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// =====================================================
// Web Server
// =====================================================

ESP8266WebServer server(80);

// =====================================================
// Sensor Variables
// =====================================================

float temperature = 0.0;
float humidity = 0.0;

unsigned long lastSensorRead = 0;
const unsigned long sensorInterval = 2000;

// =====================================================
// Read DHT11
// =====================================================

void readSensor() {

  if (millis() - lastSensorRead < sensorInterval) {
    return;
  }

  lastSensorRead = millis();

  float newHumidity = dht.readHumidity();
  float newTemperature = dht.readTemperature();

  // Check if reading failed
  if (isnan(newHumidity) || isnan(newTemperature)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return;
  }

  humidity = newHumidity;
  temperature = newTemperature;

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
}

// =====================================================
// Main Web Page
// =====================================================

void handleRoot() {

  String html = R"rawliteral(

<!DOCTYPE html>
<html lang="en">

<head>

<meta charset="UTF-8">
<meta name="viewport"
      content="width=device-width, initial-scale=1.0">

<title>ESP8266 Environment Monitor</title>

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

    background:
        radial-gradient(
            circle at top left,
            #1e293b,
            #0f172a 45%,
            #020617
        );

    color: #f8fafc;

    padding: 20px;

}

/* ==============================
   Main Container
   ============================== */

.container {

    width: 100%;
    max-width: 1000px;

    margin: auto;

}

/* ==============================
   Header
   ============================== */

.header {

    display: flex;

    justify-content: space-between;

    align-items: center;

    margin-bottom: 25px;

}

.title {

    font-size: clamp(24px, 5vw, 36px);

    font-weight: 700;

    letter-spacing: -1px;

}

.subtitle {

    margin-top: 5px;

    color: #94a3b8;

    font-size: 14px;

}

/* ==============================
   Status
   ============================== */

.status {

    display: flex;

    align-items: center;

    gap: 8px;

    padding: 8px 13px;

    border-radius: 50px;

    background: rgba(255,255,255,0.06);

    border: 1px solid rgba(255,255,255,0.1);

    font-size: 13px;

    color: #cbd5e1;

}

.status-dot {

    width: 9px;

    height: 9px;

    border-radius: 50%;

    background: #22c55e;

    box-shadow: 0 0 10px #22c55e;

}

/* ==============================
   Cards
   ============================== */

.grid {

    display: grid;

    grid-template-columns:
        repeat(auto-fit, minmax(280px, 1fr));

    gap: 20px;

}

.card {

    position: relative;

    overflow: hidden;

    padding: 25px;

    border-radius: 24px;

    background:
        rgba(255,255,255,0.055);

    border:
        1px solid rgba(255,255,255,0.09);

    backdrop-filter: blur(18px);

    -webkit-backdrop-filter: blur(18px);

    box-shadow:
        0 20px 50px rgba(0,0,0,0.25);

}

/* Decorative glow */

.card::before {

    content: "";

    position: absolute;

    width: 160px;

    height: 160px;

    border-radius: 50%;

    filter: blur(60px);

    opacity: 0.25;

    right: -70px;

    top: -70px;

}

.temperature::before {

    background: #f97316;

}

.humidity::before {

    background: #38bdf8;

}

/* ==============================
   Card Header
   ============================== */

.card-header {

    display: flex;

    justify-content: space-between;

    align-items: center;

    color: #cbd5e1;

    font-size: 14px;

}

.icon {

    font-size: 26px;

}

/* ==============================
   Value
   ============================== */

.value {

    margin-top: 20px;

    font-size: clamp(48px, 10vw, 72px);

    font-weight: 700;

    letter-spacing: -3px;

}

.unit {

    font-size: 22px;

    color: #94a3b8;

    letter-spacing: 0;

}

/* ==============================
   Progress Bar
   ============================== */

.progress-container {

    margin-top: 20px;

}

.progress {

    width: 100%;

    height: 7px;

    background: rgba(255,255,255,0.08);

    border-radius: 20px;

    overflow: hidden;

}

.progress-bar {

    height: 100%;

    border-radius: 20px;

    transition: width 0.6s ease;

}

.temp-bar {

    background:
        linear-gradient(90deg,#fb923c,#ef4444);

}

.humidity-bar {

    background:
        linear-gradient(90deg,#38bdf8,#2563eb);

}

/* ==============================
   Graph
   ============================== */

.graph-card {

    margin-top: 20px;

}

.graph-title {

    font-size: 17px;

    font-weight: 600;

}

.graph-subtitle {

    margin-top: 5px;

    color: #64748b;

    font-size: 13px;

}

canvas {

    width: 100%;

    height: 180px;

    margin-top: 20px;

}

/* ==============================
   Footer
   ============================== */

.footer {

    margin-top: 20px;

    text-align: center;

    color: #64748b;

    font-size: 12px;

}

.last-update {

    margin-top: 8px;

    color: #94a3b8;

}

/* ==============================
   Mobile
   ============================== */

@media(max-width:600px) {

    body {

        padding: 14px;

    }

    .header {

        align-items: flex-start;

    }

    .status {

        font-size: 11px;

    }

    .card {

        padding: 20px;

    }

}

</style>

</head>


<body>

<div class="container">

    <!-- HEADER -->

    <div class="header">

        <div>

            <div class="title">
                Environment Monitor
            </div>

            <div class="subtitle">
                ESP8266 • DHT11 Sensor
            </div>

        </div>

        <div class="status">

            <span class="status-dot"></span>

            Online

        </div>

    </div>


    <!-- SENSOR CARDS -->

    <div class="grid">

        <!-- TEMPERATURE -->

        <div class="card temperature">

            <div class="card-header">

                <span>
                    TEMPERATURE
                </span>

                <span class="icon">
                    🌡️
                </span>

            </div>

            <div class="value">

                <span id="temperature">
                    --
                </span>

                <span class="unit">
                    °C
                </span>

            </div>

            <div class="progress-container">

                <div class="progress">

                    <div
                        id="tempBar"
                        class="progress-bar temp-bar"
                        style="width:0%">
                    </div>

                </div>

            </div>

        </div>


        <!-- HUMIDITY -->

        <div class="card humidity">

            <div class="card-header">

                <span>
                    HUMIDITY
                </span>

                <span class="icon">
                    💧
                </span>

            </div>

            <div class="value">

                <span id="humidity">
                    --
                </span>

                <span class="unit">
                    %
                </span>

            </div>

            <div class="progress-container">

                <div class="progress">

                    <div
                        id="humidityBar"
                        class="progress-bar humidity-bar"
                        style="width:0%">
                    </div>

                </div>

            </div>

        </div>

    </div>


    <!-- GRAPH -->

    <div class="card graph-card">

        <div class="graph-title">
            Temperature History
        </div>

        <div class="graph-subtitle">
            Recent sensor readings
        </div>

        <canvas id="graph"></canvas>

    </div>


    <div class="footer">

        <div>
            ESP8266 Environmental Monitoring System
        </div>

        <div class="last-update">

            Last updated:
            <span id="lastUpdate">
                --
            </span>

        </div>

    <div style="margin-top: 8px;">
        <a
            href="https://github.com/unoanirban"
            target="_blank"
            rel="noopener noreferrer"
            style="
                color: #94a3b8;
                text-decoration: none;
                font-size: 12px;
            "
        >
            github.com/unoanirban
        </a>

    </div>

</div>


<script>

const temperatureElement =
    document.getElementById("temperature");

const humidityElement =
    document.getElementById("humidity");

const tempBar =
    document.getElementById("tempBar");

const humidityBar =
    document.getElementById("humidityBar");

const lastUpdate =
    document.getElementById("lastUpdate");

const canvas =
    document.getElementById("graph");

const ctx =
    canvas.getContext("2d");


let temperatureHistory = [];


// ========================================
// Get Sensor Data
// ========================================

async function updateData() {

    try {

        const response =
            await fetch("/data");

        const data =
            await response.json();


        // Temperature

        temperatureElement.textContent =
            data.temperature.toFixed(1);


        // Humidity

        humidityElement.textContent =
            data.humidity.toFixed(1);


        // Progress bars

        let tempPercent =
            ((data.temperature + 10) / 60) * 100;

        tempPercent =
            Math.max(0, Math.min(100, tempPercent));

        tempBar.style.width =
            tempPercent + "%";


        humidityBar.style.width =
            data.humidity + "%";


        // History

        temperatureHistory.push(
            data.temperature
        );


        if (temperatureHistory.length > 30) {

            temperatureHistory.shift();

        }


        drawGraph();


        // Time

        const now =
            new Date();

        lastUpdate.textContent =
            now.toLocaleTimeString();

    }

    catch(error) {

        console.log(
            "Connection error:",
            error
        );

    }

}


// ========================================
// Draw Graph
// ========================================

function drawGraph() {

    const width =
        canvas.clientWidth;

    const height =
        canvas.clientHeight;


    const dpr =
        window.devicePixelRatio || 1;


    canvas.width =
        width * dpr;

    canvas.height =
        height * dpr;


    ctx.scale(dpr,dpr);


    ctx.clearRect(
        0,
        0,
        width,
        height
    );


    if (
        temperatureHistory.length < 2
    ) {

        return;

    }


    // Find min/max

    const min =
        Math.min(...temperatureHistory) - 2;

    const max =
        Math.max(...temperatureHistory) + 2;


    // Grid

    ctx.strokeStyle =
        "rgba(255,255,255,0.07)";

    ctx.lineWidth = 1;


    for(let i = 1; i < 4; i++) {

        const y =
            (height / 4) * i;

        ctx.beginPath();

        ctx.moveTo(0,y);

        ctx.lineTo(width,y);

        ctx.stroke();

    }


    // Line

    ctx.beginPath();


    temperatureHistory.forEach(
        (value,index) => {

            const x =
                (index /
                (temperatureHistory.length - 1))
                * width;


            const y =
                height -
                ((value - min) /
                (max - min))
                * height;


            if(index === 0) {

                ctx.moveTo(x,y);

            }

            else {

                ctx.lineTo(x,y);

            }

        }
    );


    ctx.strokeStyle =
        "#fb923c";

    ctx.lineWidth = 3;

    ctx.lineJoin = "round";

    ctx.lineCap = "round";

    ctx.stroke();


    // Fill below graph

    const lastIndex =
        temperatureHistory.length - 1;


    const lastX =
        width;


    ctx.lineTo(
        lastX,
        height
    );

    ctx.lineTo(
        0,
        height
    );

    ctx.closePath();


    ctx.fillStyle =
        "rgba(251,146,60,0.08)";

    ctx.fill();

}


// ========================================
// Update every 2 seconds
// ========================================

updateData();

setInterval(
    updateData,
    2000
);


window.addEventListener(
    "resize",
    drawGraph
);

</script>


</body>

</html>

)rawliteral";

  server.send(200, "text/html", html);
}

// =====================================================
// JSON API
// =====================================================

void handleData() {

  String json = "{";

  json += "\"temperature\":";
  json += String(temperature, 1);

  json += ",";

  json += "\"humidity\":";
  json += String(humidity, 1);

  json += "}";

  server.send(
    200,
    "application/json",
    json
  );
}

// =====================================================
// 404 Handler
// =====================================================

void handleNotFound() {

  server.send(
    404,
    "text/plain",
    "404 - Page Not Found"
  );
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(100);

  Serial.println();
  Serial.println("==============================");
  Serial.println("ESP8266 DHT11 Web Server");
  Serial.println("==============================");

  // Start DHT

  dht.begin();

  Serial.println("DHT11 initialized.");


  // Connect WiFi

  WiFi.mode(WIFI_STA);

  WiFi.begin(
    ssid,
    password
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


  // Initial sensor reading

  delay(2000);

  float h =
      dht.readHumidity();

  float t =
      dht.readTemperature();


  if (!isnan(h) && !isnan(t)) {

    humidity = h;

    temperature = t;

  }


  // Web routes

  server.on(
    "/",
    handleRoot
  );

  server.on(
    "/data",
    handleData
  );

  server.onNotFound(
    handleNotFound
  );


  // Start server

  server.begin();

  Serial.println(
    "HTTP server started."
  );

  Serial.print(
    "Open http://"
  );

  Serial.print(
    WiFi.localIP()
  );

  Serial.println(
    " in your browser."
  );

}

// =====================================================
// LOOP
// =====================================================

void loop() {

  readSensor();

  server.handleClient();

}
