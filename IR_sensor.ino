#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// =====================================================
// WiFi Configuration
// =====================================================

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// =====================================================
// Pin Configuration
// =====================================================

#define IR_PIN D5
#define BUZZER_PIN D6

// Most IR obstacle sensors:
// LOW  = Object detected
// HIGH = No object
#define OBJECT_DETECTED LOW

// =====================================================
// Web Server
// =====================================================

ESP8266WebServer server(80);

// =====================================================
// Variables
// =====================================================

bool objectDetected = false;
bool previousDetection = false;

unsigned long detectionCount = 0;
unsigned long lastDetectionTime = 0;

const unsigned long buzzerInterval = 180;

// =====================================================
// HTML Dashboard
// =====================================================

const char MAIN_PAGE[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html lang="en">

<head>

<meta charset="UTF-8">

<meta name="viewport"
      content="width=device-width, initial-scale=1.0">

<title>Presence Monitor</title>

<style>

/* =====================================================
   GLOBAL
   ===================================================== */

* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {

    min-height: 100vh;

    font-family:
        -apple-system,
        BlinkMacSystemFont,
        "Segoe UI",
        Roboto,
        Arial,
        sans-serif;

    color: #f8fafc;

    background:
        radial-gradient(
            circle at 20% 0%,
            #1e293b,
            #0f172a 45%,
            #020617
        );

    padding: 20px;

}

/* =====================================================
   CONTAINER
   ===================================================== */

.container {

    width: 100%;

    max-width: 900px;

    margin: auto;

}

/* =====================================================
   HEADER
   ===================================================== */

.header {

    display: flex;

    justify-content: space-between;

    align-items: center;

    margin-bottom: 25px;

}

.title {

    font-size:
        clamp(26px, 6vw, 40px);

    font-weight: 750;

    letter-spacing: -1.5px;

}

.subtitle {

    margin-top: 5px;

    color: #94a3b8;

    font-size: 14px;

}

/* =====================================================
   CONNECTION STATUS
   ===================================================== */

.connection {

    display: flex;

    align-items: center;

    gap: 8px;

    padding: 8px 14px;

    border-radius: 30px;

    background:
        rgba(255,255,255,0.05);

    border:
        1px solid
        rgba(255,255,255,0.08);

    font-size: 13px;

}

.connection-dot {

    width: 9px;

    height: 9px;

    border-radius: 50%;

    background: #22c55e;

    box-shadow:
        0 0 10px #22c55e;

}

/* =====================================================
   MAIN STATUS CARD
   ===================================================== */

.status-card {

    position: relative;

    overflow: hidden;

    padding: 35px 25px;

    border-radius: 28px;

    background:
        rgba(255,255,255,0.055);

    border:
        1px solid
        rgba(255,255,255,0.1);

    backdrop-filter:
        blur(20px);

    -webkit-backdrop-filter:
        blur(20px);

    box-shadow:
        0 25px 70px
        rgba(0,0,0,0.35);

    text-align: center;

    transition:
        0.35s ease;

}

/* =====================================================
   STATUS ICON
   ===================================================== */

.status-icon {

    width: 110px;

    height: 110px;

    margin:
        0 auto 20px;

    border-radius: 50%;

    display: flex;

    justify-content: center;

    align-items: center;

    font-size: 50px;

    background:
        rgba(34,197,94,0.1);

    border:
        1px solid
        rgba(34,197,94,0.2);

    box-shadow:
        0 0 35px
        rgba(34,197,94,0.12);

    transition:
        0.35s ease;

}

.status-icon.detected {

    background:
        rgba(239,68,68,0.12);

    border-color:
        rgba(239,68,68,0.4);

    box-shadow:
        0 0 50px
        rgba(239,68,68,0.25);

    animation:
        pulse 1s infinite;

}

@keyframes pulse {

    0% {
        transform: scale(1);
    }

    50% {
        transform: scale(1.06);
    }

    100% {
        transform: scale(1);
    }

}

/* =====================================================
   STATUS TEXT
   ===================================================== */

.status-title {

    font-size: 28px;

    font-weight: 700;

}

.status-description {

    margin-top: 8px;

    color: #94a3b8;

    font-size: 14px;

}

/* =====================================================
   STAT GRID
   ===================================================== */

.stats {

    display: grid;

    grid-template-columns:
        repeat(2, 1fr);

    gap: 18px;

    margin-top: 20px;

}

.stat {

    padding: 22px;

    border-radius: 20px;

    background:
        rgba(255,255,255,0.045);

    border:
        1px solid
        rgba(255,255,255,0.07);

}

.stat-label {

    color: #64748b;

    font-size: 12px;

    text-transform:
        uppercase;

    letter-spacing: 1px;

}

.stat-value {

    margin-top: 8px;

    font-size: 28px;

    font-weight: 700;

}

/* =====================================================
   EVENT CARD
   ===================================================== */

.event-card {

    margin-top: 20px;

    padding: 22px;

    border-radius: 22px;

    background:
        rgba(255,255,255,0.045);

    border:
        1px solid
        rgba(255,255,255,0.07);

}

.event-title {

    font-size: 16px;

    font-weight: 600;

}

.event {

    margin-top: 15px;

    display: flex;

    justify-content: space-between;

    align-items: center;

    padding: 13px 15px;

    border-radius: 13px;

    background:
        rgba(255,255,255,0.04);

}

.event-left {

    display: flex;

    align-items: center;

    gap: 10px;

}

.event-dot {

    width: 8px;

    height: 8px;

    border-radius: 50%;

    background: #ef4444;

}

.event-time {

    color: #94a3b8;

    font-size: 12px;

}

/* =====================================================
   FOOTER
   ===================================================== */

.footer {

    text-align: center;

    margin-top: 20px;

    color: #475569;

    font-size: 12px;

}

/* =====================================================
   MOBILE
   ===================================================== */

@media(max-width:600px) {

    body {

        padding: 14px;

    }

    .header {

        align-items:
            flex-start;

    }

    .connection {

        font-size: 11px;

        padding:
            7px 10px;

    }

    .status-card {

        padding:
            30px 18px;

    }

    .stats {

        grid-template-columns:
            1fr;

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
                Presence Monitor
            </div>

            <div class="subtitle">
                ESP8266 • IR Detection System
            </div>

        </div>


        <div class="connection">

            <span
                class="connection-dot">
            </span>

            <span id="connection">
                Connected
            </span>

        </div>

    </div>


    <!-- MAIN STATUS -->

    <div class="status-card">

        <div
            id="statusIcon"
            class="status-icon">

            🛡️

        </div>


        <div
            id="statusTitle"
            class="status-title">

            Area Clear

        </div>


        <div
            id="statusDescription"
            class="status-description">

            No object detected

        </div>

    </div>


    <!-- STATS -->

    <div class="stats">


        <div class="stat">

            <div class="stat-label">
                Detection Count
            </div>

            <div
                id="count"
                class="stat-value">

                0

            </div>

        </div>


        <div class="stat">

            <div class="stat-label">
                Last Detection
            </div>

            <div
                id="lastDetection"
                class="stat-value">

                Never

            </div>

        </div>


    </div>


    <!-- EVENT -->

    <div class="event-card">

        <div class="event-title">
            Latest Event
        </div>


        <div class="event">

            <div class="event-left">

                <span
                    id="eventDot"
                    class="event-dot">
                </span>

                <span id="eventText">
                    Monitoring area
                </span>

            </div>


            <span
                id="eventTime"
                class="event-time">

                --

            </span>

        </div>

    </div>


    <div class="footer">

        ESP8266 Presence Detection System

    </div>


</div>


<script>

/* =====================================================
   ELEMENTS
   ===================================================== */

const statusIcon =
    document.getElementById(
        "statusIcon"
    );

const statusTitle =
    document.getElementById(
        "statusTitle"
    );

const statusDescription =
    document.getElementById(
        "statusDescription"
    );

const count =
    document.getElementById(
        "count"
    );

const lastDetection =
    document.getElementById(
        "lastDetection"
    );

const eventText =
    document.getElementById(
        "eventText"
    );

const eventTime =
    document.getElementById(
        "eventTime"
    );

const connection =
    document.getElementById(
        "connection"
    );


/* =====================================================
   FETCH API
   ===================================================== */

async function updateStatus() {

    try {

        const response =
            await fetch(
                "/api/status",
                {
                    cache: "no-store"
                }
            );


        const data =
            await response.json();


        /* Connection */

        connection.textContent =
            "Connected";


        /* Detection count */

        count.textContent =
            data.count;


        /* Detection state */

        if(data.detected) {


            statusIcon.classList.add(
                "detected"
            );


            statusIcon.textContent =
                "⚠️";


            statusTitle.textContent =
                "Object Detected";


            statusDescription.textContent =
                "Presence detected nearby";


            eventText.textContent =
                "Object / Person detected";


            lastDetection.textContent =
                formatTime(
                    data.lastDetection
                );


            eventTime.textContent =
                formatTime(
                    data.lastDetection
                );

        }

        else {


            statusIcon.classList.remove(
                "detected"
            );


            statusIcon.textContent =
                "🛡️";


            statusTitle.textContent =
                "Area Clear";


            statusDescription.textContent =
                "No object detected";


            eventText.textContent =
                "Monitoring area";

        }

    }

    catch(error) {

        console.log(error);

        connection.textContent =
            "Disconnected";

    }

}


/* =====================================================
   FORMAT TIME
   ===================================================== */

function formatTime(timestamp) {

    if(timestamp === 0) {

        return "Never";

    }


    const date =
        new Date(timestamp);


    return date.toLocaleTimeString();

}


/* =====================================================
   UPDATE
   ===================================================== */

updateStatus();


setInterval(
    updateStatus,
    500
);

</script>


</body>

</html>

)rawliteral";


// =====================================================
// API: STATUS
// =====================================================

void handleStatus() {

    String json = "{";

    json += "\"detected\":";
    json += objectDetected ? "true" : "false";

    json += ",";

    json += "\"count\":";
    json += String(detectionCount);

    json += ",";

    json += "\"lastDetection\":";
    json += String(lastDetectionTime);

    json += "}";


    server.send(
        200,
        "application/json",
        json
    );
}


// =====================================================
// ROOT PAGE
// =====================================================

void handleRoot() {

    server.send_P(
        200,
        "text/html",
        MAIN_PAGE
    );

}


// =====================================================
// 404
// =====================================================

void handleNotFound() {

    server.send(
        404,
        "text/plain",
        "404 - Not Found"
    );

}


// =====================================================
// SETUP
// =====================================================

void setup() {

    Serial.begin(115200);

    delay(100);


    Serial.println();
    Serial.println(
        "================================"
    );
    Serial.println(
        "ESP8266 Presence Detection"
    );
    Serial.println(
        "================================"
    );


    // -------------------------------------------------
    // Pins
    // -------------------------------------------------

    pinMode(
        IR_PIN,
        INPUT
    );

    pinMode(
        BUZZER_PIN,
        OUTPUT
    );


    digitalWrite(
        BUZZER_PIN,
        LOW
    );


    // -------------------------------------------------
    // WiFi
    // -------------------------------------------------

    WiFi.mode(
        WIFI_STA
    );

    WiFi.begin(
        ssid,
        password
    );


    Serial.print(
        "Connecting to WiFi"
    );


    while(
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


    // -------------------------------------------------
    // Web Server Routes
    // -------------------------------------------------

    server.on(
        "/",
        handleRoot
    );


    server.on(
        "/api/status",
        handleStatus
    );


    server.onNotFound(
        handleNotFound
    );


    server.begin();


    Serial.println(
        "Web server started."
    );


    Serial.print(
        "Open: http://"
    );

    Serial.print(
        WiFi.localIP()
    );

    Serial.println();

}


// =====================================================
// LOOP
// =====================================================

void loop() {

    // -------------------------------------------------
    // Read IR sensor
    // -------------------------------------------------

    bool currentDetection =
        digitalRead(IR_PIN)
        == OBJECT_DETECTED;


    // -------------------------------------------------
    // New detection event
    // -------------------------------------------------

    if(
        currentDetection &&
        !previousDetection
    ) {

        detectionCount++;

        lastDetectionTime =
            millis();


        Serial.println(
            "OBJECT DETECTED!"
        );

    }


    objectDetected =
        currentDetection;


    previousDetection =
        currentDetection;


    // -------------------------------------------------
    // Buzzer
    // -------------------------------------------------

    if(objectDetected) {

        // Simple intermittent beep

        if(
            millis() %
            (buzzerInterval * 2)
            < buzzerInterval
        ) {

            digitalWrite(
                BUZZER_PIN,
                HIGH
            );

        }

        else {

            digitalWrite(
                BUZZER_PIN,
                LOW
            );

        }

    }

    else {

        digitalWrite(
            BUZZER_PIN,
            LOW
        );

    }


    // -------------------------------------------------
    // Web server
    // -------------------------------------------------

    server.handleClient();

}
