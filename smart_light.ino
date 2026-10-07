/*
   ============================================================
   SMART STREET LIGHT
   ============================================================

   Compatible with:
   - Arduino Nano
   - ESP8266

   Components:
   - LDR sensor
   - LED / Street Light

   Working:
   - Bright environment -> Light OFF
   - Dark environment   -> Light ON
   - Hysteresis prevents flickering
   - Smooth fading is used for the light

   LDR:
   AO -> A0

   LED:
   Nano     -> D9
   ESP8266  -> D5

   ============================================================
*/


// ============================================================
// Board-specific configuration
// ============================================================

#if defined(ESP8266)

  // ESP8266
  #define LDR_PIN A0
  #define LIGHT_PIN D5

  const int ADC_MAX = 1023;

#elif defined(ARDUINO_ARCH_AVR)

  // Arduino Nano / ATmega328P
  #define LDR_PIN A0
  #define LIGHT_PIN 9

  const int ADC_MAX = 1023;

#else

  #error "This code supports ESP8266 and Arduino Nano only."

#endif


// ============================================================
// LDR Configuration
// ============================================================
//
// You should calibrate these values according to your LDR.
//
// The example assumes:
//
// Higher ADC value = brighter
// Lower ADC value  = darker
//
// If your sensor behaves opposite, see the note at the
// bottom of the code.
// ============================================================

const int DARK_THRESHOLD = 400;
const int BRIGHT_THRESHOLD = 550;


// ============================================================
// Light brightness
// ============================================================

const int MIN_BRIGHTNESS = 0;
const int MAX_BRIGHTNESS = 255;


// Current brightness

int currentBrightness = 0;


// Target brightness

int targetBrightness = 0;


// ============================================================
// Timing
// ============================================================

unsigned long lastSensorRead = 0;

const unsigned long SENSOR_INTERVAL = 100;


// ============================================================
// Street Light State
// ============================================================

bool streetLightON = false;


// ============================================================
// Setup
// ============================================================

void setup() {

  Serial.begin(115200);

  delay(500);


  // --------------------------------------------------------
  // Pin configuration
  // --------------------------------------------------------

  pinMode(
    LDR_PIN,
    INPUT
  );

  pinMode(
    LIGHT_PIN,
    OUTPUT
  );


  // Start with light OFF

  analogWrite(
    LIGHT_PIN,
    0
  );


  Serial.println();
  Serial.println("======================================");
  Serial.println("       SMART STREET LIGHT");
  Serial.println("======================================");


#if defined(ESP8266)

  Serial.println("Board: ESP8266");

#elif defined(ARDUINO_ARCH_AVR)

  Serial.println("Board: Arduino Nano");

#endif


  Serial.println();

  Serial.println("LDR Pin   : A0");


#if defined(ESP8266)

  Serial.println("Light Pin : D5");

#elif defined(ARDUINO_ARCH_AVR)

  Serial.println("Light Pin : D9");

#endif


  Serial.println();

  Serial.println("System ready.");
  Serial.println("--------------------------------------");
}


// ============================================================
// Read LDR
// ============================================================

int readLDR() {

  return analogRead(
    LDR_PIN
  );
}


// ============================================================
// Determine whether it is dark
// ============================================================

void updateStreetLight(
  int ldrValue
) {

  /*
     HYSTERESIS

     If currently OFF:
     -----------------
     Light turns ON only when
     LDR <= DARK_THRESHOLD


     If currently ON:
     ----------------
     Light turns OFF only when
     LDR >= BRIGHT_THRESHOLD


     This prevents rapid ON/OFF switching
     when the LDR value fluctuates.
  */


  if (!streetLightON) {

    if (
      ldrValue <= DARK_THRESHOLD
    ) {

      streetLightON = true;

      Serial.println(
        ">> DARK detected - Street light ON"
      );
    }

  }
  else {

    if (
      ldrValue >= BRIGHT_THRESHOLD
    ) {

      streetLightON = false;

      Serial.println(
        ">> BRIGHT detected - Street light OFF"
      );
    }
  }


  // --------------------------------------------------------
  // Set target brightness
  // --------------------------------------------------------

  if (streetLightON) {

    targetBrightness =
      MAX_BRIGHTNESS;

  }
  else {

    targetBrightness =
      MIN_BRIGHTNESS;
  }
}


// ============================================================
// Smooth brightness control
// ============================================================

void updateBrightness() {

  if (
    currentBrightness <
    targetBrightness
  ) {

    currentBrightness++;

  }
  else if (
    currentBrightness >
    targetBrightness
  ) {

    currentBrightness--;
  }


  analogWrite(
    LIGHT_PIN,
    currentBrightness
  );
}


// ============================================================
// Main Loop
// ============================================================

void loop() {

  unsigned long currentMillis =
    millis();


  // --------------------------------------------------------
  // Read LDR periodically
  // --------------------------------------------------------

  if (
    currentMillis - lastSensorRead >=
    SENSOR_INTERVAL
  ) {

    lastSensorRead =
      currentMillis;


    int ldrValue =
      readLDR();


    updateStreetLight(
      ldrValue
    );


    // ------------------------------------------------------
    // Serial Monitor
    // ------------------------------------------------------

    Serial.print(
      "LDR: "
    );

    Serial.print(
      ldrValue
    );

    Serial.print(
      " | Light: "
    );


    if (streetLightON) {

      Serial.print(
        "ON"
      );

    }
    else {

      Serial.print(
        "OFF"
      );
    }


    Serial.print(
      " | Brightness: "
    );

    Serial.println(
      currentBrightness
    );
  }


  // --------------------------------------------------------
  // Smooth LED transition
  // --------------------------------------------------------

  updateBrightness();

  delay(5);
}
