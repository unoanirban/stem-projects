# 🔧 Arduino & NodeMCU Sensor Projects

A collection of practical **Arduino Nano/UNO and NodeMCU (ESP8266)**
projects built for learning, experimentation, robotics, automation, and
IoT applications.

This repository contains a set of standalone projects covering **IR
sensing, environmental monitoring, rain detection, soil moisture
monitoring, security alarms, smart lighting, and ultrasonic distance
detection**.

------------------------------------------------------------------------

## 📂 Projects

  --------------------------------------------------------------------------------
  Project           File                       Platform          Description
  ----------------- -------------------------- ----------------- -----------------
  🚨 IR Sensor      `IR_sensor.ino`            Arduino           Basic IR sensor
                                                                 detection project

  🌡️ DHT11 Web      `dht11_web.ino`            NodeMCU / ESP8266 Temperature &
  Monitor                                                        humidity
                                                                 monitoring
                                                                 through a web
                                                                 dashboard

  🌧️ Raindrop       `raindrop_sensor.ino`      NodeMCU / ESP8266 Rain detection
  Sensor                                                         and intensity
                                                                 monitoring

  🚨 Security Alarm `security_alarm.ino`       Arduino           Sensor-based
                                                                 security/alarm
                                                                 system

  💡 Smart Light    `smart_light.ino`          Arduino /         Automatic smart
                                               compatible        lighting control

  🌱 Soil Moisture  `soil_moisture.ino`        NodeMCU / ESP8266 Soil moisture
                                                                 monitoring
                                                                 through a web
                                                                 interface

  📏 Ultrasonic     `ultrasonic_arduino.ino`   Arduino Nano/UNO  Distance
  Arduino                                                        measurement with
                                                                 buzzer alarm

  📡 Ultrasonic     `ultrasonic_sensor.ino`    NodeMCU / ESP8266 Ultrasonic object
  Sensor                                                         detection with
                                                                 web dashboard
  --------------------------------------------------------------------------------

------------------------------------------------------------------------

## 🧰 Hardware Used

Depending on the project, the repository uses components such as:

-   Arduino Nano / Arduino UNO
-   NodeMCU ESP8266
-   IR obstacle sensor
-   DHT11 temperature & humidity sensor
-   Raindrop sensor
-   Soil moisture sensor
-   HC-SR04 ultrasonic sensor
-   Buzzer
-   LEDs
-   Resistors
-   Breadboard
-   Jumper wires

------------------------------------------------------------------------

## 🚨 IR Sensor

**File:** `IR_sensor.ino`

A simple project demonstrating how to read an IR obstacle sensor using
an Arduino.

### Basic operation

``` text
Object detected → Sensor triggered
No object       → Normal state
```

The project can be used as the foundation for:

-   Object detection
-   Intrusion detection
-   Automatic barriers
-   Robotics
-   Counting systems

------------------------------------------------------------------------

## 🌡️ DHT11 Web Monitoring

**File:** `dht11_web.ino`

A NodeMCU/ESP8266-based environmental monitoring system.

It reads:

-   🌡️ Temperature
-   💧 Humidity

The ESP8266 hosts a lightweight web dashboard that displays the sensor
data in real time.

### Features

-   Responsive web interface
-   Temperature display
-   Humidity display
-   Live temperature graph
-   Automatic data updates
-   ESP8266 web server
-   NTP-based time support where applicable

------------------------------------------------------------------------

## 🌧️ Raindrop Sensor

**File:** `raindrop_sensor.ino`

A NodeMCU-based rain monitoring project using a raindrop sensor module.

### Features

-   Detects rain
-   Reads analog rain intensity
-   Displays rain status
-   Provides a web dashboard
-   Updates sensor information automatically

### Important note

The displayed rain percentage represents **relative sensor intensity**,
not an actual rainfall measurement such as mm/hour. Accurate rainfall
measurement requires a calibrated rain gauge or tipping-bucket sensor.

------------------------------------------------------------------------

## 🚨 Security Alarm

**File:** `security_alarm.ino`

A simple security-oriented alarm system using a sensor and buzzer.

When the sensor detects the configured trigger condition, the buzzer
generates an alarm.

### Possible applications

-   Door/window security
-   Intrusion detection
-   Object detection
-   Basic home-security prototypes

------------------------------------------------------------------------

## 💡 Smart Light

**File:** `smart_light.ino`

A simple smart-light automation project designed to demonstrate
automatic control of a light based on sensor input.

This type of project can be extended with:

-   LDR-based automatic lighting
-   Motion detection
-   Manual override
-   Timer-based control
-   IoT control using ESP8266/ESP32

------------------------------------------------------------------------

## 🌱 Soil Moisture Monitoring

**File:** `soil_moisture.ino`

A NodeMCU/ESP8266 project for monitoring soil moisture.

The analog sensor reading is converted into a relative moisture
percentage and displayed through a lightweight web dashboard.

### Example status levels

``` text
0–20%    → Very Dry
21–40%   → Dry
41–70%   → Moist
71–100%  → Very Moist
```

### Possible applications

-   Smart irrigation
-   Plant monitoring
-   Automatic watering systems
-   Agriculture automation
-   IoT gardening

> Sensor calibration is important because the raw values vary between
> different soil-moisture sensors and soil types.

------------------------------------------------------------------------

## 📏 Ultrasonic Distance Detector --- Arduino

**File:** `ultrasonic_arduino.ino`

An Arduino Nano/UNO project using an **HC-SR04 ultrasonic sensor** and a
buzzer.

### Logic

``` text
Distance < 10 cm
        ↓
Object detected
        ↓
Alarm tone
```

When the measured distance is below the configured threshold, the buzzer
produces a repeating alarm tone.

The measured distance is also displayed on the **Serial Monitor**.

### Typical connections

  HC-SR04   Arduino
  --------- ---------
  VCC       5V
  GND       GND
  TRIG      D5
  ECHO      D6

  Buzzer   Arduino
  -------- ---------
  \+       D7
  \-       GND

------------------------------------------------------------------------

## 📡 Ultrasonic Sensor --- NodeMCU

**File:** `ultrasonic_sensor.ino`

An ESP8266/NodeMCU version of the ultrasonic object detection system.

The project combines:

-   HC-SR04 ultrasonic sensing
-   Distance calculation
-   Buzzer alarm
-   ESP8266 web server
-   Live browser dashboard

### Detection logic

``` text
Distance < 10 cm → Object Detected → Buzzer ON
Distance ≥ 10 cm → Area Clear     → Buzzer OFF
```

### ⚠️ Important ESP8266 Safety Note

The HC-SR04 **ECHO pin normally outputs 5V**, while ESP8266 GPIO pins
are not 5V tolerant.

Use a voltage divider between the HC-SR04 ECHO pin and the ESP8266 GPIO.

Example:

``` text
HC-SR04 ECHO
     │
    1kΩ
     │
     ├────────→ ESP8266 GPIO
     │
    2kΩ
     │
    GND
```

------------------------------------------------------------------------

# 🌐 ESP8266 Web Projects

The ESP8266-based projects in this repository use the microcontroller's
built-in Wi-Fi capability to host lightweight web interfaces.

The general architecture is:

``` text
┌──────────────┐
│    Sensor    │
└──────┬───────┘
       │
       ▼
┌──────────────┐
│   NodeMCU    │
│   ESP8266    │
└──────┬───────┘
       │ Wi-Fi
       ▼
┌──────────────┐
│ Web Browser  │
│  Dashboard   │
└──────────────┘
```

No separate server or computer is required for the basic dashboard
functionality.

------------------------------------------------------------------------

# 🛠️ Software Requirements

## Arduino IDE

Install the latest Arduino IDE:

https://www.arduino.cc/en/software

### For Arduino Nano / UNO

Select the appropriate Arduino board from:

``` text
Tools → Board
```

### For NodeMCU ESP8266

Install the ESP8266 board package and select:

``` text
NodeMCU 1.0 (ESP-12E Module)
```

------------------------------------------------------------------------

# 📚 Libraries

Depending on the project, libraries may include:

### DHT11

**Adafruit DHT Sensor Library**

https://github.com/adafruit/DHT-sensor-library

**Adafruit Unified Sensor**

https://github.com/adafruit/Adafruit_Sensor

### ESP8266

**ESP8266 Arduino Core**

https://github.com/esp8266/Arduino

Most Arduino projects such as the ultrasonic and basic IR projects use
the standard Arduino core functions and do not require additional
libraries.

------------------------------------------------------------------------

# 🚀 How to Use

1.  Clone or download this repository.
2.  Open the required `.ino` file in Arduino IDE.
3.  Connect the required hardware according to the project's wiring.
4.  Select the correct board and COM port.
5.  Update Wi-Fi credentials in ESP8266 projects where required.
6.  Upload the sketch.
7.  Open Serial Monitor if the project provides serial output.
8.  For web-server projects, open the ESP8266's displayed IP address in
    a browser.

------------------------------------------------------------------------

# ⚙️ Calibration

Sensor readings can vary depending on:

-   Sensor model
-   Supply voltage
-   Environment
-   Soil type
-   Sensor placement
-   Component tolerance

Therefore, values such as moisture and rain intensity should be
calibrated for the actual hardware before using them in a real
application.

------------------------------------------------------------------------

# 📌 Repository Structure

``` text
.
├── IR_sensor.ino
├── dht11_web.ino
├── raindrop_sensor.ino
├── security_alarm.ino
├── smart_light.ino
├── soil_moisture.ino
├── ultrasonic_arduino.ino
├── ultrasonic_sensor.ino
└── README.md
```

------------------------------------------------------------------------

# 🎯 Purpose

These projects are primarily intended for:

-   Arduino learning
-   Embedded systems practice
-   IoT experimentation
-   Sensor interfacing
-   Electronics prototyping
-   Robotics projects
-   STEM education
-   Rapid proof-of-concept development

The individual projects can also be used as building blocks for larger
automation and robotics systems.

------------------------------------------------------------------------

# 🔮 Future Improvements

Possible future additions include:

-   ESP32 versions
-   MQTT integration
-   Mobile notifications
-   Blynk integration
-   OLED/LCD displays
-   Data logging
-   Cloud dashboards
-   Automatic irrigation
-   Advanced security systems
-   Multiple sensor integration
-   OTA firmware updates
-   Mobile app control

------------------------------------------------------------------------

## 👨‍💻 Author

**Anirban Midya**

GitHub: https://github.com/unoanirban

------------------------------------------------------------------------

## ⭐ Support

If these projects are useful for your learning or development, consider
giving the repository a ⭐ on GitHub.
