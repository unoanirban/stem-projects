// ============================================================
// Arduino Nano - IR Alarm with Alarm Tone
// ============================================================
//
// IR Sensor:
// VCC -> 5V
// GND -> GND
// OUT -> D2
//
// Buzzer:
// + -> D8
// - -> GND
//
// Logic:
// Object detected -> Alarm tone plays
// No object       -> Alarm OFF
// ============================================================

#define IR_SENSOR_PIN 2
#define BUZZER_PIN 8

// Most IR obstacle sensors:
// LOW = Object detected
#define OBJECT_DETECTED LOW

void setup() {

  Serial.begin(9600);

  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  noTone(BUZZER_PIN);

  Serial.println();
  Serial.println("==============================");
  Serial.println("      IR ALARM SYSTEM");
  Serial.println("==============================");
  Serial.println("System Armed");
  Serial.println();
}

void loop() {

  int sensorState = digitalRead(IR_SENSOR_PIN);

  if (sensorState == OBJECT_DETECTED) {

    Serial.println("!!! ALARM !!! OBJECT DETECTED");

    // Alarm tone 1
    tone(BUZZER_PIN, 1200);
    delay(200);

    // Alarm tone 2
    tone(BUZZER_PIN, 1800);
    delay(200);

  }
  else {

    noTone(BUZZER_PIN);

    Serial.println("Area Clear");
    delay(200);
  }
}
