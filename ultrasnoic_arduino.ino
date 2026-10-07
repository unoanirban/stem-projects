// ============================================================
// Arduino Ultrasonic Object Detector
// ============================================================
//
// HC-SR04:
// TRIG -> D5
// ECHO -> D6
//
// Buzzer:
// + -> D7
// - -> GND
//
// Logic:
// Distance < 10 cm -> Buzzer beeps
// Distance >= 10 cm -> Buzzer OFF
//
// ============================================================

#define TRIG_PIN 5
#define ECHO_PIN 6
#define BUZZER_PIN 7

// Detection threshold
const float DETECTION_DISTANCE = 10.0;

// Buzzer timing
const unsigned long BEEP_ON_TIME = 120;
const unsigned long BEEP_OFF_TIME = 180;

bool buzzerState = false;

unsigned long buzzerTimer = 0;


// ============================================================
// Measure Distance
// ============================================================

float getDistance() {

  // Ensure trigger starts LOW
  digitalWrite(TRIG_PIN, LOW);

  delayMicroseconds(2);

  // Send 10 microsecond trigger pulse
  digitalWrite(TRIG_PIN, HIGH);

  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  // Read echo pulse
  unsigned long duration = pulseIn(
    ECHO_PIN,
    HIGH,
    30000
  );

  // No echo received
  if (duration == 0) {
    return -1;
  }

  // Convert time to distance in cm
  float distance =
    duration / 58.0;

  return distance;
}


// ============================================================
// Buzzer Control
// ============================================================

void updateBuzzer(
  bool objectDetected
) {

  // ----------------------------------------------------------
  // No object -> Buzzer OFF
  // ----------------------------------------------------------

  if (!objectDetected) {

    if (buzzerState) {

      noTone(BUZZER_PIN);

      buzzerState = false;
    }

    return;
  }


  // ----------------------------------------------------------
  // Object detected -> Beep repeatedly
  // ----------------------------------------------------------

  unsigned long currentMillis =
    millis();


  if (buzzerState) {

    // Currently beeping

    if (
      currentMillis - buzzerTimer >=
      BEEP_ON_TIME
    ) {

      noTone(BUZZER_PIN);

      buzzerState = false;

      buzzerTimer =
        currentMillis;
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

      buzzerTimer =
        currentMillis;
    }
  }
}


// ============================================================
// Setup
// ============================================================

void setup() {

  Serial.begin(9600);

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


  Serial.println();
  Serial.println(
    "================================"
  );
  Serial.println(
    "  Ultrasonic Object Detector"
  );
  Serial.println(
    "================================"
  );

  Serial.println(
    "Detection distance: < 10 cm"
  );

  Serial.println();
}


// ============================================================
// Main Loop
// ============================================================

void loop() {

  // Read distance

  float distance =
    getDistance();


  // ----------------------------------------------------------
  // Print distance
  // ----------------------------------------------------------

  if (distance < 0) {

    Serial.println(
      "Distance: No Echo"
    );

    updateBuzzer(false);

  }
  else {

    Serial.print(
      "Distance: "
    );

    Serial.print(
      distance,
      1
    );

    Serial.print(
      " cm"
    );


    // --------------------------------------------------------
    // Check detection
    // --------------------------------------------------------

    bool objectDetected =
      (distance < DETECTION_DISTANCE);


    if (objectDetected) {

      Serial.println(
        "  |  OBJECT DETECTED"
      );

    }
    else {

      Serial.println(
        "  |  Area Clear"
      );
    }


    // Update buzzer

    updateBuzzer(
      objectDetected
    );
  }


  // Small delay between measurements

  delay(100);
}
