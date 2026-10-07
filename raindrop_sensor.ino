#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ======================================================
// WiFi Configuration
// ======================================================

const char* WIFI_SSID = "AndroidAP_4123";
const char* WIFI_PASSWORD = "12345678";

// ======================================================
// Raindrop Sensor Pins
// ======================================================

#define RAIN_ANALOG_PIN A0
#define RAIN_DIGITAL_PIN D5

// ======================================================
// Rain Calibration
// ======================================================
//
// Typical raindrop modules:
//
// Dry  -> HIGH ADC value
// Wet  -> LOW ADC value
//
// Adjust these according to your sensor.
//

const int DRY_VALUE = 900;
const int WET_VALUE = 300;

// ======================================================
// Digital Sensor Logic
// ======================================================
//
// Most FC-37 / YL-83 modules output:
//
// LOW  = Rain detected
// HIGH = No rain
//

#define RAIN_DETECTED LOW

// ======================================================
// Web Server
// ======================================================

ESP8266WebServer server(80);

// ======================================================
// Sensor Variables
// ======================================================

int rainRaw = 0;
int rainPercent = 0;

bool rainDetected = false;

String rainStatus = "No Rain";

// ======================================================
// Read Rain Sensor
// ======================================================

void readRainSensor() {

  // Read analog value
  rainRaw = analogRead(RAIN_ANALOG_PIN);

  // Convert raw value to rain intensity
  rainPercent = map(
    rainRaw,
    DRY_VALUE,
    WET_VALUE,
    0,
    100
  );

  rainPercent = constrain(
    rainPercent,
    0,
    100
  );


  // Read digital output
  rainDetected =
    (digitalRead(RAIN_DIGITAL_PIN) == RAIN_DETECTED);


  // Determine status
  if (!rainDetected) {

    rainStatus = "No Rain";

  }
  else if (rainPercent <= 30) {

    rainStatus = "Light Rain";

  }
  else if (rainPercent <= 65) {

    rainStatus = "Moderate Rain";

  }
  else {

    rainStatus = "Heavy Rain";

  }
}

// ======================================================
// API Endpoint
// ======================================================

void handleAPI() {

  readRainSensor();

  String json = "{";

  json += "\"rainRaw\":";
  json += rainRaw;

  json += ",\"rainPercent\":";
  json += rainPercent;

  json += ",\"rainDetected\":";
  json += rainDetected ? "true" : "false";

  json += ",\"status\":\"";
  json += rainStatus;
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

<title>Rain Monitor</title>


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
            rgba(59, 130, 246, 0.10),
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
        clamp(26px, 6vw, 38px);

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

    background: #38bdf8;

    box-shadow:
        0 0 10px rgba(56, 189, 248, 0.8);

    animation:
        pulse 2s infinite;
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
   Main Reading
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


.rain-value {

    font-size:
        clamp(65px, 15vw, 105px);

    line-height: 1;

    font-weight: 700;

    background:

        linear-gradient(
            135deg,
            #38bdf8,
            #60a5fa,
            #818cf8
        );

    -webkit-background-clip: text;

    -webkit-text-fill-color: transparent;

    background-clip: text;

    margin: 10px 0;
}


/* =====================================================
   Rain Status
   ===================================================== */

.status {

    display: inline-flex;

    align-items: center;

    padding: 8px 17px;

    border-radius: 30px;

    margin-top: 15px;

    font-size: 14px;

    color: #7dd3fc;

    background:
        rgba(56, 189, 248, 0.10);

    border:
        1px solid rgba(56, 189, 248, 0.20);

    transition:
        all 0.3s ease;
}


.status.raining {

    color: #93c5fd;

    background:
        rgba(59, 130, 246, 0.13);

    border-color:
        rgba(59, 130, 246, 0.30);

    box-shadow:
        0 0 18px rgba(59, 130, 246, 0.12);
}


/* =====================================================
   Progress
   ===================================================== */

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
            #93c5fd,
            #38bdf8,
            #2563eb
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

    font-size: 25px;

    font-weight: 600;
}


.info-unit {

    color: #64748b;

    font-size: 12px;

    margin-left: 4px;
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
            🌧️ Rain Monitor
        </h1>

        <p>

            <span class="status-dot"></span>

            NodeMCU • Live Monitoring

        </p>

    </div>


    <!-- Main Reading -->

    <div class="card main-reading">

        <div class="label">
            Rain Intensity
        </div>


        <div
            class="rain-value"
            id="rainPercent"
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

                <span>
                    Dry
                </span>

                <span>
                    Moderate
                </span>

                <span>
                    Heavy
                </span>

            </div>

        </div>

    </div>


    <!-- Information -->

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
                Rain Detection
            </div>

            <div
                class="info-value"
                id="rainDetection"
            >
                Checking
            </div>

        </div>


    </div>


    <!-- Footer -->

    <div class="footer">

        <div>
            ESP8266 Rain Monitoring System
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


        // Rain percentage

        document.getElementById(
            "rainPercent"
        ).textContent =
            data.rainPercent + "%";


        // Progress

        document.getElementById(
            "progress"
        ).style.width =
            data.rainPercent + "%";


        // Status

        const statusElement =
            document.getElementById(
                "status"
            );


        statusElement.textContent =
            data.status;


        // Toggle rain styling

        if (data.rainDetected) {

            statusElement.classList.add(
                "raining"
            );

        }
        else {

            statusElement.classList.remove(
                "raining"
            );

        }


        // Raw ADC

        document.getElementById(
            "raw"
        ).textContent =
            data.rainRaw;


        // Digital detection

        document.getElementById(
            "rainDetection"
        ).textContent =
            data.rainDetected
                ? "Detected"
                : "No Rain";

    }

    catch (error) {

        document.getElementById(
            "rainDetection"
        ).textContent =
            "Offline";

    }

}


// Initial update

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

  Serial.println(
    "================================"
  );

  Serial.println(
    " ESP8266 Rain Monitoring System"
  );

  Serial.println(
    "================================"
  );


  // Sensor pins

  pinMode(
    RAIN_ANALOG_PIN,
    INPUT
  );

  pinMode(
    RAIN_DIGITAL_PIN,
    INPUT
  );


  // ----------------------------------------------------
  // WiFi
  // ----------------------------------------------------

  WiFi.mode(WIFI_STA);

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
// Loop
// ======================================================

void loop() {

  server.handleClient();

}
