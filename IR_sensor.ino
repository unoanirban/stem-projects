#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <time.h>

// =====================================================
// WiFi Configuration
// =====================================================

const char* ssid = "AndroidAP_4123";
const char* password = "12345678";

// =====================================================
// Pin Configuration
// =====================================================

#define IR_PIN D5
#define BUZZER_PIN D6

// Most IR obstacle sensors:
// LOW  = Object detected
// HIGH = No object detected
#define OBJECT_DETECTED LOW

// =====================================================
// Web Server
// =====================================================

ESP8266WebServer server(80);

// =====================================================
// NTP Configuration
// =====================================================

// India Standard Time = UTC + 5:30
const long GMT_OFFSET_SEC = 19800;
const int DAYLIGHT_OFFSET_SEC = 0;

// =====================================================
// Detection Variables
// =====================================================

bool objectDetected = false;
bool previousDetection = false;

unsigned long detectionCount = 0;

// Stores the actual Unix timestamp
time_t lastDetectionTimestamp = 0;

// =====================================================
// Buzzer
// =====================================================

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
   RESET
   ===================================================== */

* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}


/* =====================================================
   BODY
   ===================================================== */

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
            circle at 15% 0%,
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

    max-width: 950px;

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

    padding: 38px 25px;

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

    width: 115px;

    height: 115px;

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


/* =====================================================
   DETECTED STATE
   ===================================================== */

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

    font-size: 29px;

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

    padding: 23px;

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

    margin-top: 9px;

    font-size: 26px;

    font-weight: 700;

}


.stat-sub {

    margin-top: 5px;

    color: #64748b;

    font-size: 12px;

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

    padding: 15px;

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

    background: #64748b;

}


.event-dot.active {

    background: #ef4444;

    box-shadow:
        0 0 10px #ef4444;

}


.event-time {

    color: #94a3b8;

    font-size: 12px;

}


/* =====================================================
   NTP CARD
   ===================================================== */

.ntp-card {

    margin-top: 20px;

    display: flex;

    justify-content: space-between;

    align-items: center;

    padding: 17px 20px;

    border-radius: 18px;

    background:
        rgba(255,255,255,0.04);

    border:
        1px solid
        rgba(255,255,255,0.07);

}


.ntp-label {

    color: #94a3b8;

    font-size: 13px;

}


.ntp-value {

    font-size: 13px;

    font-weight: 600;

}


.ntp-dot {

    display: inline-block;

    width: 7px;

    height: 7px;

    margin-right: 6px;

    border-radius: 50%;

    background: #22c55e;

    box-shadow:
        0 0 8px #22c55e;

}


/* =====================================================
   FOOTER
   ===================================================== */

.footer {

    text-align: center;

    margin-top: 22px;

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

    .ntp-card {

        flex-direction:
            column;

        align-items:
            flex-start;

        gap: 8px;

    }

}

</style>

</head>


<body>


<div class="container">


    <!-- ============================================
         HEADER
         ============================================ -->

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


    <!-- ============================================
         MAIN STATUS
         ============================================ -->

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

            No object detected nearby

        </div>


    </div>


    <!-- ============================================
         STATISTICS
         ============================================ -->

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

            <div class="stat-sub">
                Detection events
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

            <div
                id="lastDate"
                class="stat-sub">

                No detection recorded

            </div>

        </div>


    </div>


    <!-- ============================================
         LATEST EVENT
         ============================================ -->

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


    <!-- ============================================
         NTP STATUS
         ============================================ -->

    <div class="ntp-card">


        <div class="ntp-label">
            System Clock
        </div>


        <div
            id="ntpStatus"
            class="ntp-value">

            <span class="ntp-dot"></span>

            NTP synchronized • IST

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

const lastDate =
    document.getElementById(
        "lastDate"
    );

const eventText =
    document.getElementById(
        "eventText"
    );

const eventTime =
    document.getElementById(
        "eventTime"
    );

const eventDot =
    document.getElementById(
        "eventDot"
    );

const connection =
    document.getElementById(
        "connection"
    );

const ntpStatus =
    document.getElementById(
        "ntpStatus"
    );


/* =====================================================
   GET API DATA
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


        if(!response.ok) {

            throw new Error(
                "API error"
            );

        }


        const data =
            await response.json();


        /* ---------------------------------------------
           CONNECTION
           --------------------------------------------- */

        connection.textContent =
            "Connected";


        /* ---------------------------------------------
           NTP STATUS
           --------------------------------------------- */

        if(data.timeSynced) {

            ntpStatus.innerHTML =
                '<span class="ntp-dot"></span>' +
                'NTP synchronized • IST';

        }

        else {

            ntpStatus.innerHTML =
                '⚠️ Waiting for NTP';

        }


        /* ---------------------------------------------
           COUNT
           --------------------------------------------- */

        count.textContent =
            data.count;


        /* ---------------------------------------------
           DETECTION
           --------------------------------------------- */

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


            eventDot.classList.add(
                "active"
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
                "No object detected nearby";


            eventText.textContent =
                "Monitoring area";


            eventDot.classList.remove(
                "active"
            );

        }


        /* ---------------------------------------------
           LAST DETECTION
           --------------------------------------------- */

        if(data.lastDetectionTime !== "") {

            lastDetection.textContent =
                data.lastDetectionTime;


            lastDate.textContent =
                data.lastDetectionDate;


            eventTime.textContent =
                data.lastDetectionTime;

        }

        else {

            lastDetection.textContent =
                "Never";


            lastDate.textContent =
                "No detection recorded";


            eventTime.textContent =
                "--";

        }

    }

    catch(error) {

        console.log(error);

        connection.textContent =
            "Disconnected";

    }

}


/* =====================================================
   INITIAL UPDATE
   ===================================================== */

updateStatus();


/* =====================================================
   UPDATE EVERY 500ms
   ===================================================== */

setInterval(
    updateStatus,
    500
);

</script>

<div class="footer">

    ESP8266 Presence Detection System

    <div style="margin-top: 8px;">
        <a
            href="https://github.com/unoanirban"
            target="_blank"
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

</body>

</html>

)rawliteral";


// =====================================================
// NTP TIME SYNCHRONIZATION
// =====================================================

void setupTime() {

    Serial.println();
    Serial.println("Starting NTP synchronization...");

    configTime(
        GMT_OFFSET_SEC,
        DAYLIGHT_OFFSET_SEC,
        "pool.ntp.org",
        "time.nist.gov"
    );


    struct tm timeinfo;


    int attempts = 0;


    while(
        !getLocalTime(
            &timeinfo
        )
        &&
        attempts < 30
    ) {

        delay(500);

        Serial.print(".");

        attempts++;

    }


    Serial.println();


    if(attempts < 30) {

        Serial.println(
            "NTP time synchronized!"
        );


        Serial.print(
            "Current time: "
        );


        char timeString[50];


        strftime(
            timeString,
            sizeof(timeString),
            "%d-%m-%Y %I:%M:%S %p",
            &timeinfo
        );


        Serial.println(
            timeString
        );

    }

    else {

        Serial.println(
            "NTP synchronization failed."
        );

    }

}


// =====================================================
// CHECK IF NTP TIME IS VALID
// =====================================================

bool isTimeSynced() {

    time_t now = time(nullptr);

    // Unix timestamp must be greater than
    // approximately Jan 1, 2021

    return now > 1609459200;

}


// =====================================================
// FORMAT LAST DETECTION TIME
// =====================================================

String getLastDetectionTime() {

    if(
        lastDetectionTimestamp == 0
    ) {

        return "";

    }


    struct tm timeinfo;


    localtime_r(
        &lastDetectionTimestamp,
        &timeinfo
    );


    char buffer[20];


    strftime(
        buffer,
        sizeof(buffer),
        "%I:%M:%S %p",
        &timeinfo
    );


    return String(buffer);

}


// =====================================================
// FORMAT LAST DETECTION DATE
// =====================================================

String getLastDetectionDate() {

    if(
        lastDetectionTimestamp == 0
    ) {

        return "";

    }


    struct tm timeinfo;


    localtime_r(
        &lastDetectionTimestamp,
        &timeinfo
    );


    char buffer[20];


    strftime(
        buffer,
        sizeof(buffer),
        "%d %b %Y",
        &timeinfo
    );


    return String(buffer);

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
// STATUS API
// =====================================================

void handleStatus() {

    bool synced =
        isTimeSynced();


    String json = "{";


    // Detection state

    json += "\"detected\":";
    json +=
        objectDetected
        ? "true"
        : "false";


    // Detection count

    json += ",";

    json += "\"count\":";
    json += String(
        detectionCount
    );


    // NTP state

    json += ",";

    json += "\"timeSynced\":";
    json +=
        synced
        ? "true"
        : "false";


    // Last detection time

    json += ",";

    json += "\"lastDetectionTime\":\"";

    json +=
        getLastDetectionTime();

    json += "\"";


    // Last detection date

    json += ",";

    json += "\"lastDetectionDate\":\"";

    json +=
        getLastDetectionDate();

    json += "\"";


    json += "}";


    server.send(
        200,
        "application/json",
        json
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
        "===================================="
    );

    Serial.println(
        "ESP8266 PRESENCE MONITOR"
    );

    Serial.println(
        "IR + BUZZER + NTP"
    );

    Serial.println(
        "===================================="
    );


    // -------------------------------------------------
    // GPIO
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
    // NTP
    // -------------------------------------------------

    setupTime();


    // -------------------------------------------------
    // Web Server
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
        "HTTP server started."
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
        digitalRead(
            IR_PIN
        )
        == OBJECT_DETECTED;


    // -------------------------------------------------
    // NEW DETECTION EVENT
    // -------------------------------------------------

    if(
        currentDetection &&
        !previousDetection
    ) {


        detectionCount++;


        // Get REAL current time

        time_t now =
            time(nullptr);


        if(
            now > 1609459200
        ) {

            lastDetectionTimestamp =
                now;

        }


        Serial.println();

        Serial.println(
            "================================"
        );

        Serial.println(
            "OBJECT DETECTED!"
        );


        if(
            lastDetectionTimestamp != 0
        ) {

            Serial.print(
                "Detection time: "
            );


            Serial.print(
                getLastDetectionDate()
            );


            Serial.print(
                " "
            );


            Serial.println(
                getLastDetectionTime()
            );

        }


        Serial.print(
            "Detection count: "
        );


        Serial.println(
            detectionCount
        );


        Serial.println(
            "================================"
        );

    }


    // Update state

    objectDetected =
        currentDetection;


    previousDetection =
        currentDetection;


    // -------------------------------------------------
    // BUZZER
    // -------------------------------------------------

    if(
        objectDetected
    ) {


        unsigned long phase =
            millis()
            %
            (buzzerInterval * 2);


        if(
            phase <
            buzzerInterval
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
    // WEB SERVER
    // -------------------------------------------------

    server.handleClient();

}
