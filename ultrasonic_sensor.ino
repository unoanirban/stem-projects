#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ======================================================
// WiFi Configuration
// ======================================================

const char* WIFI_SSID = "AndroidAP_4123";
const char* WIFI_PASSWORD = "12345678";

// ======================================================
// Pin Configuration
// ======================================================

#define TRIG_PIN D5
#define ECHO_PIN D6
#define BUZZER_PIN D7

// ======================================================
// Detection Threshold
// ======================================================

const float DETECTION_DISTANCE = 10.0;

// ======================================================
// Buzzer Configuration
// ======================================================

const unsigned long BEEP_ON_TIME = 120;
const unsigned long BEEP_OFF_TIME = 180;

bool buzzerState = false;
unsigned long buzzerTimer = 0;

// ======================================================
// Sensor Variables
// ======================================================

float distanceCM = 0.0;

bool objectDetected = false;

// ======================================================
// Web Server
// ======================================================

ESP8266WebServer server(80);

// ======================================================
// Measure Distance
// ======================================================

float getDistance() {

  // Make sure trigger is LOW
  digitalWrite(TRIG_PIN, LOW);

  delayMicroseconds(2);

  // Send 10us trigger pulse
  digitalWrite(TRIG_PIN, HIGH);

  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  // Read echo
  unsigned long duration = pulseIn(
    ECHO_PIN,
    HIGH,
    30000
  );

  // No echo received
  if (duration == 0) {
    return -1;
  }

  // Speed of sound:
  // distance = duration / 58.0
  float distance = duration / 58.0;

  return distance;
}

// ======================================================
// Update Sensor
// ======================================================

void updateSensor() {

  float measuredDistance = getDistance();

  if (measuredDistance < 0) {

    distanceCM = 0;

    objectDetected = false;

    return;
  }

  distanceCM = measuredDistance;

  objectDetected =
    (distanceCM > 0 &&
     distanceCM < DETECTION_DISTANCE);
}

// ======================================================
// Buzzer Control
// ======================================================

void updateBuzzer() {

  if (!objectDetected) {

    if (buzzerState) {

      noTone(BUZZER_PIN);

      buzzerState = false;
    }

    return;
  }


  unsigned long currentMillis = millis();


  if (buzzerState) {

    // Currently beeping

    if (
      currentMillis - buzzerTimer >=
      BEEP_ON_TIME
    ) {

      noTone(BUZZER_PIN);

      buzzerState = false;

      buzzerTimer = currentMillis;
    }

  }
  else {

    // Currently silent

    if (
      currentMillis - buzzerTimer >=
      BEEP_OFF_TIME
    ) {

      tone(
        BUZZER_PIN,
        2000
      );

      buzzerState = true;

      buzzerTimer = currentMillis;
    }
  }
}

// ======================================================
// API Endpoint
// ======================================================

void handleAPI() {

  updateSensor();

  String json = "{";

  json += "\"distance\":";

  if (distanceCM > 0) {
    json += String(distanceCM, 1);
  }
  else {
    json += "null";
  }

  json += ",\"objectDetected\":";
  json += objectDetected ? "true" : "false";

  json += ",\"buzzer\":";
  json += buzzerState ? "true" : "false";

  json += ",\"threshold\":";
  json += String(DETECTION_DISTANCE, 1);

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

<title>Ultrasonic Object Detector</title>


<style>

/* =====================================================
   Base
   ===================================================== */

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

    padding: 20px;

    color: #e5e7eb;

    background:

        radial-gradient(
            circle at top left,
            rgba(56, 189, 248, 0.14),
            transparent 35%
        ),

        radial-gradient(
            circle at bottom right,
            rgba(139, 92, 246, 0.10),
            transparent 35%
        ),

        #080c12;
}


.container {

    width: 100%;

    max-width: 850px;

    margin: auto;
}


/* =====================================================
   Header
   ===================================================== */

.header {

    text-align: center;

    margin-bottom: 24px;
}


.header h1 {

    font-size:
        clamp(25px, 6vw, 38px);

    font-weight: 700;

    letter-spacing: -0.6px;

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
        opacity: 0.35;
    }
}


/* =====================================================
   Cards
   ===================================================== */

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


/* =====================================================
   Main Distance
   ===================================================== */

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


.distance-value {

    font-size:
        clamp(65px, 15vw, 105px);

    line-height: 1;

    font-weight: 700;

    background:

        linear-gradient(
            135deg,
            #38bdf8,
            #818cf8
        );

    -webkit-background-clip: text;

    -webkit-text-fill-color: transparent;

    background-clip: text;

    margin: 10px 0;
}


.distance-unit {

    color: #64748b;

    font-size: 15px;

    margin-top: 5px;
}


/* =====================================================
   Detection Status
   ===================================================== */

.status {

    display: inline-flex;

    align-items: center;

    justify-content: center;

    padding: 9px 18px;

    border-radius: 30px;

    margin-top: 18px;

    font-size: 14px;

    color: #86efac;

    background:
        rgba(34, 197, 94, 0.10);

    border:
        1px solid rgba(34, 197, 94, 0.20);

    transition:
        all 0.3s ease;
}


.status.detected {

    color: #fca5a5;

    background:
        rgba(239, 68, 68, 0.12);

    border-color:
        rgba(239, 68, 68, 0.30);

    box-shadow:
        0 0 22px rgba(239, 68, 68, 0.12);
}


/* =====================================================
   Distance Indicator
   ===================================================== */

.distance-bar-container {

    margin-top: 30px;
}


.distance-track {

    width: 100%;

    height: 10px;

    overflow: hidden;

    border-radius: 20px;

    background:
        rgba(255, 255, 255, 0.08);
}


.distance-bar {

    height: 100%;

    width: 0%;

    border-radius: inherit;

    background:

        linear-gradient(
            90deg,
            #22c55e,
            #eab308,
            #f97316,
            #ef4444
        );

    transition:
        width 0.35s ease;
}


.distance-labels {

    display: flex;

    justify-content: space-between;

    margin-top: 8px;

    color: #64748b;

    font-size: 11px;
}


/* =====================================================
   Info Grid
   ===================================================== */

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

    font-size: 24px;

    font-weight: 600;
}


.info-unit {

    color: #64748b;

    font-size: 12px;

    margin-left: 4px;
}


/* =====================================================
   Buzzer Indicator
   ===================================================== */

.buzzer-indicator {

    display: inline-flex;

    align-items: center;

    gap: 7px;
}


.buzzer-light {

    width: 8px;

    height: 8px;

    border-radius: 50%;

    background: #475569;

    transition: all 0.2s ease;
}


.buzzer-light.active {

    background: #ef4444;

    box-shadow:
        0 0 12px rgba(239, 68, 68, 0.9);

    animation:
        buzzerPulse 0.25s infinite alternate;
}


@keyframes buzzerPulse {

    from {
        transform: scale(1);
    }

    to {
        transform: scale(1.35);
    }
}


/* =====================================================
   Footer
   ===================================================== */

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

    transform:
        translateY(-2px);

    text-decoration: underline;
}


/* =====================================================
   Mobile
   ===================================================== */

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


    <!-- Header -->

    <div class="header">

        <h1>
            📡 Ultrasonic Object Detector
        </h1>

        <p>

            <span class="status-dot"></span>

            NodeMCU • Live Monitoring

        </p>

    </div>


    <!-- Main Distance Card -->

    <div class="card main-reading">

        <div class="label">
            Current Distance
        </div>


        <div
            class="distance-value"
            id="distance"
        >
            --.- 
        </div>


        <div class="distance-unit">
            centimeters
        </div>


        <div
            class="status"
            id="status"
        >
            Checking...
        </div>


        <div class="distance-bar-container">

            <div class="distance-track">

                <div
                    class="distance-bar"
                    id="distanceBar"
                ></div>

            </div>


            <div class="distance-labels">

                <span>
                    0 cm
                </span>

                <span>
                    10 cm
                </span>

                <span>
                    20+ cm
                </span>

            </div>

        </div>

    </div>


    <!-- Information Cards -->

    <div class="grid">


        <div class="card info-card">

            <div class="info-title">
                Detection Range
            </div>

            <div class="info-value">

                10

                <span class="info-unit">
                    cm
                </span>

            </div>

        </div>


        <div class="card info-card">

            <div class="info-title">
                Buzzer
            </div>

            <div
                class="info-value"
                id="buzzer"
            >

                <span class="buzzer-indicator">

                    <span
                        class="buzzer-light"
                        id="buzzerLight"
                    ></span>

                    <span id="buzzerText">
                        OFF
                    </span>

                </span>

            </div>

        </div>


    </div>


    <!-- Footer -->

    <div class="footer">

        <div>
            ESP8266 Ultrasonic Object Detection System
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
            await fetch(
                "/api/status"
            );


        const data =
            await response.json();


        // =============================================
        // Distance
        // =============================================

        const distanceElement =
            document.getElementById(
                "distance"
            );


        if (data.distance !== null) {

            distanceElement.textContent =
                data.distance.toFixed(1);

        }
        else {

            distanceElement.textContent =
                "--.-";

        }


        // =============================================
        // Detection Status
        // =============================================

        const statusElement =
            document.getElementById(
                "status"
            );


        if (data.objectDetected) {

            statusElement.textContent =
                "⚠ Object Detected";

            statusElement.classList.add(
                "detected"
            );

        }
        else {

            statusElement.textContent =
                "✓ Area Clear";

            statusElement.classList.remove(
                "detected"
            );

        }


        // =============================================
        // Distance Bar
        // =============================================

        let barWidth = 0;


        if (data.distance !== null) {

            // 20 cm = full bar

            barWidth =
                Math.min(
                    data.distance / 20 * 100,
                    100
                );
        }


        document.getElementById(
            "distanceBar"
        ).style.width =
            barWidth + "%";


        // =============================================
        // Buzzer
        // =============================================

        const buzzerText =
            document.getElementById(
                "buzzerText"
            );

        const buzzerLight =
            document.getElementById(
                "buzzerLight"
            );


        if (data.buzzer) {

            buzzerText.textContent =
                "BEEPING";

            buzzerLight.classList.add(
                "active"
            );

        }
        else {

            buzzerText.textContent =
                "OFF";

            buzzerLight.classList.remove(
                "active"
            );

        }

    }

    catch (error) {

        document.getElementById(
            "status"
        ).textContent =
            "Connection Error";

    }

}


// Initial update

updateData();


// Update dashboard every 500ms

setInterval(
    updateData,
    500
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

  Serial.println(
    "========================================"
  );

  Serial.println(
    " ESP8266 Ultrasonic Object Detector"
  );

  Serial.println(
    "========================================"
  );


  // ----------------------------------------------------
  // Pin Configuration
  // ----------------------------------------------------

  pinMode(
    TRIG_PIN,
    OUTPUT
  );

  pinMode(
    ECHO_PIN,
    INPUT
  );

  pinMode(
    BUZZER_PIN,
    OUTPUT
  );


  digitalWrite(
    TRIG_PIN,
    LOW
  );

  noTone(
    BUZZER_PIN
  );


  // ----------------------------------------------------
  // WiFi
  // ----------------------------------------------------

  WiFi.mode(
    WIFI_STA
  );

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );


  Serial.print(
    "Connecting to WiFi"
  );


  while (
    WiFi.status() != WL_CONNECTED
  ) {

    delay(500);

    Serial.print(".");
  }


  Serial.println();

  Serial.println(
    "WiFi connected!"
  );


  Serial.print(
    "IP Address: "
  );

  Serial.println(
    WiFi.localIP()
  );


  // ----------------------------------------------------
  // Web Server
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

  // Handle web requests

  server.handleClient();


  // Update sensor

  updateSensor();


  // Update buzzer

  updateBuzzer();


  // Small delay for stability

  delay(30);
}
