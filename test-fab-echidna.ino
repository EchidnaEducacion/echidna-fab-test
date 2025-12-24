/*
 * Test FAB Echidna - Test program for Arduino Nano educational board
 * Sequential test of actuators and sensors in Normal mode and MkMk mode
 */

#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// ============================================
// PIN DEFINITIONS - NORMAL MODE
// ============================================
// Actuators
#define PIN_BUZZER 10
#define PIN_LED_GREEN 11
#define PIN_LED_ORANGE 12
#define PIN_LED_RED 13
#define PIN_RGB_R 9
#define PIN_RGB_G 5
#define PIN_RGB_B 6

// Buttons
#define PIN_BUTTON_SR 2  // Right button (Yes/Correct)
#define PIN_BUTTON_SL 3  // Left button (No/Incorrect)

// Analog sensors
#define PIN_JOY_X A0
#define PIN_JOY_Y A1
#define PIN_LDR A3
#define PIN_TEMP A6
#define PIN_MIC A7

// General purpose IO pins (for digital input testing)
#define PIN_IO_A2 A2
#define PIN_IO_D4 4
#define PIN_IO_D7 7
#define PIN_IO_D8 8

// I2C - Accelerometer
#define LIS3DHTR_ADDR 0x18

// ============================================
// SENSOR THRESHOLDS
// ============================================
// Joystick
#define JOY_THRESHOLD_LOW 5
#define JOY_THRESHOLD_HIGH 1018

// Accelerometer (values in m/s²)
#define ACCEL_THRESHOLD_LOW -7.8
#define ACCEL_THRESHOLD_HIGH 7.8

// LDR
#define LDR_THRESHOLD_DARK 30

// Microphone
#define MIC_THRESHOLD_NOISE 100

// MkMk
#define MKMK_THRESHOLD 100
#define MKMK_MAX_ATTEMPTS 10

// Reading update time
#define SENSOR_READ_DELAY 500

// ============================================
// GLOBAL VARIABLES
// ============================================
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Structure to store results
struct TestResults {
  int normalActuators;
  int normalSensors;
  int mkMk;
  int normalActuatorsFail;
  int normalSensorsFail;
  int mkMkFail;
};

TestResults results = {0, 0, 0, 0, 0, 0};

// Arrays to store failed test names
String failedActuatorTests[10];
int numFailedActuatorTests = 0;
String failedSensorTests[10];
int numFailedSensorTests = 0;
String failedMkMkTests[10];
int numFailedMkMkTests = 0;

// ============================================
// SETUP
// ============================================
void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial connection

  // Configure output pins
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_ORANGE, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_RGB_R, OUTPUT);
  pinMode(PIN_RGB_G, OUTPUT);
  pinMode(PIN_RGB_B, OUTPUT);

  // Configure input pins
  pinMode(PIN_BUTTON_SR, INPUT_PULLUP);
  pinMode(PIN_BUTTON_SL, INPUT_PULLUP);

  // Configure general purpose IO pins for digital input testing
  pinMode(PIN_IO_A2, INPUT_PULLUP);
  pinMode(PIN_IO_D4, INPUT_PULLUP);
  pinMode(PIN_IO_D7, INPUT_PULLUP);
  pinMode(PIN_IO_D8, INPUT_PULLUP);

  // Turn off all actuators
  turnOffActuators();

  // Initialize accelerometer
  if (lis.begin(LIS3DHTR_ADDR)) {
    lis.setRange(LIS3DH_RANGE_2_G);
    lis.setDataRate(LIS3DH_DATARATE_50_HZ);
    Serial.println(F("Accelerometer initialized successfully"));
  } else {
    Serial.println(F("ERROR: Could not initialize accelerometer"));
  }

  delay(1000);
}

// ============================================
// MAIN LOOP
// ============================================
void loop() {
  Serial.println(F("\n========================================"));
  Serial.println(F("FAB ECHIDNA TEST - Start"));
  Serial.println(F("========================================\n"));

  // Test Normal Mode
  testNormalMode();

  // Test MkMk Mode
  testMkMkMode();

  // Final Report
  finalReport();

  // Wait to restart
  Serial.println(F("\n\nTest completed. Press RESET to restart."));
  while (true); // Stop execution
}

// ============================================
// UTILITY FUNCTIONS
// ============================================
void turnOffActuators() {
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_ORANGE, LOW);
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_RGB_R, LOW);
  digitalWrite(PIN_RGB_G, LOW);
  digitalWrite(PIN_RGB_B, LOW);
  digitalWrite(PIN_BUZZER, LOW);
}

bool waitForButtonSR() {
  Serial.println(F("Press SR (right button) to continue..."));
  while (digitalRead(PIN_BUTTON_SR) == HIGH) {
    delay(10);
  }
  delay(200); // Debounce
  while (digitalRead(PIN_BUTTON_SR) == LOW) {
    delay(10);
  }
  return true;
}

bool waitForButtonSL() {
  Serial.println(F("Press SL (left button) to continue..."));
  while (digitalRead(PIN_BUTTON_SL) == HIGH) {
    delay(10);
  }
  delay(200); // Debounce
  while (digitalRead(PIN_BUTTON_SL) == LOW) {
    delay(10);
  }
  return true;
}

bool askYesNo() {
  Serial.println(F("SR=Yes / SL=No"));

  // Wait for stabilization and debounce
  delay(500);

  // Read stable initial state
  int estadoSR_anterior = digitalRead(PIN_BUTTON_SR);
  int previousSL_state = digitalRead(PIN_BUTTON_SL);

  unsigned long tiempoInicio = millis();

  while (true) {
    // Read current state
    int estadoSR_actual = digitalRead(PIN_BUTTON_SR);
    int currentSL_state = digitalRead(PIN_BUTTON_SL);

    // Only detect changes after additional 500ms (avoid initial changes)
    if (millis() - tiempoInicio > 500) {
      // Detect change in SR (pressed)
      if (estadoSR_actual != estadoSR_anterior) {
        delay(50); // Anti-bounce
        estadoSR_actual = digitalRead(PIN_BUTTON_SR);
        if (estadoSR_actual != estadoSR_anterior) {
          delay(300); // Additional delay
          return true;
        }
      }

      // Detect change in SL (pressed)
      if (currentSL_state != previousSL_state) {
        delay(50); // Anti-bounce
        currentSL_state = digitalRead(PIN_BUTTON_SL);
        if (currentSL_state != previousSL_state) {
          delay(300); // Additional delay
          return false;
        }
      }
    }

    // Update previous states
    estadoSR_anterior = estadoSR_actual;
    previousSL_state = currentSL_state;

    delay(10);
  }
}

void waitForButtonsReleased() {
  // Delay to clear bounces and stabilize readings
  delay(500);
}

// ============================================
// TEST NORMAL MODE
// ============================================
void testNormalMode() {
  Serial.println(F("\n****************************************"));
  Serial.println(F("START NORMAL MODE TEST"));
  Serial.println(F("****************************************\n"));

  Serial.println(F("To begin the test:"));
  Serial.println(F("1. Press SR (right button)"));
  Serial.println(F("2. Then press SL (left button)"));

  waitForButtonSR();
  waitForButtonSL();

  Serial.println(F("\nButtons verified!"));
  Serial.println(F("\n--- STARTING ACTUATORS TEST ---\n"));

  // Test Actuators
  testLEDs();
  testLEDRGB();
  testBuzzer();

  Serial.println(F("\n--- STARTING SENSORS TEST ---\n"));

  // Test Sensors
  testJoystick();
  testAccelerometer();
  testLDR();
  testTemperature();
  testMicrophone();
  testIOPins();

  Serial.println(F("\n****************************************"));
  Serial.println(F("END NORMAL MODE TEST"));
  Serial.println(F("****************************************\n"));
}

// ============================================
// ACTUATORS TEST
// ============================================
void testLEDs() {
  Serial.println(F("TEST A: LEDs (Green, Orange, Red)"));
  Serial.println(F("Turning on the 3 LEDs..."));

  digitalWrite(PIN_LED_GREEN, HIGH);
  digitalWrite(PIN_LED_ORANGE, HIGH);
  digitalWrite(PIN_LED_RED, HIGH);

  Serial.println(F("Are the 3 LEDs on?"));
  bool resultado = askYesNo();

  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_ORANGE, LOW);
  digitalWrite(PIN_LED_RED, LOW);

  if (resultado) {
    Serial.println(F("✓ LEDs OK\n"));
    results.normalActuators++;
  } else {
    Serial.println(F("✗ LEDs FAIL\n"));
    results.normalActuatorsFail++;
    failedActuatorTests[numFailedActuatorTests++] = "LEDs (Green, Orange, Red)";
  }
}

void testLEDRGB() {
  Serial.println(F("TEST B: LED RGB"));
  Serial.println(F("Showing 3 colors consecutively..."));

  // Red
  Serial.println(F("Color 1: Red"));
  digitalWrite(PIN_RGB_R, HIGH);
  delay(1500);
  digitalWrite(PIN_RGB_R, LOW);
  delay(500);

  // Green
  Serial.println(F("Color 2: Green"));
  digitalWrite(PIN_RGB_G, HIGH);
  delay(1500);
  digitalWrite(PIN_RGB_G, LOW);
  delay(500);

  // Blue
  Serial.println(F("Color 3: Blue"));
  digitalWrite(PIN_RGB_B, HIGH);
  delay(1500);
  digitalWrite(PIN_RGB_B, LOW);

  Serial.println(F("Have all 3 colors been seen?"));
  bool resultado = askYesNo();

  if (resultado) {
    Serial.println(F("✓ LED RGB OK\n"));
    results.normalActuators++;
  } else {
    Serial.println(F("✗ LED RGB FAIL\n"));
    results.normalActuatorsFail++;
    failedActuatorTests[numFailedActuatorTests++] = "LED RGB";
  }
}

void testBuzzer() {
  Serial.println(F("TEST C: Buzzer"));
  Serial.println(F("Emitting intermittent beep..."));
  Serial.println(F("(Adjust volume with potentiometer if needed)"));

  // Intermittent beeps for 3 seconds
  for (int i = 0; i < 6; i++) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(250);
    digitalWrite(PIN_BUZZER, LOW);
    delay(250);
  }

  Serial.println(F("Can you hear the buzzer?"));
  bool resultado = askYesNo();

  digitalWrite(PIN_BUZZER, LOW);

  if (resultado) {
    Serial.println(F("✓ Buzzer OK\n"));
    results.normalActuators++;
  } else {
    Serial.println(F("✗ Buzzer FAIL\n"));
    results.normalActuatorsFail++;
    failedActuatorTests[numFailedActuatorTests++] = "Buzzer";
  }
}

// ============================================
// SENSORS TEST
// ============================================
void testJoystick() {
  Serial.println(F("TEST A: Joystick"));

  // Ensure buttons are released before starting
  waitForButtonsReleased();

  // Debug: show button state
  Serial.print(F("SR State: "));
  Serial.print(digitalRead(PIN_BUTTON_SR));
  Serial.print(F(" / SL: "));
  Serial.println(digitalRead(PIN_BUTTON_SL));

  // Test Left (JoyX < 5)
  Serial.println(F("\nMove the joystick to the LEFT"));
  Serial.println(F("(Press SL if the test doesn't pass)"));
  bool passLeft = false;
  int previousSL_state = digitalRead(PIN_BUTTON_SL);
  unsigned long startTime = millis();
  while (millis() - startTime < 30000) {
    int valorX = analogRead(PIN_JOY_X);
    Serial.print(F("JoyX: "));
    Serial.println(valorX);

    if (valorX < JOY_THRESHOLD_LOW) {
      Serial.println(F("✓ Left OK"));
      passLeft = true;
      delay(1000);
      break;
    }

    // Detect state change in SL (button press)
    int currentSL_state = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && currentSL_state != previousSL_state) {
      delay(50); // Anti-bounce
      currentSL_state = digitalRead(PIN_BUTTON_SL);
      if (currentSL_state != previousSL_state) {
        Serial.println(F("✗ Left FAIL (indicated by user)"));
        delay(300);
        break;
      }
    }
    previousSL_state = currentSL_state;

    delay(SENSOR_READ_DELAY);
  }
  if (!passLeft && millis() - startTime >= 30000) {
    Serial.println(F("✗ Left TIMEOUT"));
  }

  // Test Right (JoyX > 1018)
  Serial.println(F("\nMove the joystick to the RIGHT"));
  Serial.println(F("(Press SL if the test doesn't pass)"));
  bool passRight = false;
  previousSL_state = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    int valorX = analogRead(PIN_JOY_X);
    Serial.print(F("JoyX: "));
    Serial.println(valorX);

    if (valorX > JOY_THRESHOLD_HIGH) {
      Serial.println(F("✓ Right OK"));
      passRight = true;
      delay(1000);
      break;
    }

    int currentSL_state = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && currentSL_state != previousSL_state) {
      delay(50);
      currentSL_state = digitalRead(PIN_BUTTON_SL);
      if (currentSL_state != previousSL_state) {
        Serial.println(F("✗ Right FAIL (indicated by user)"));
        delay(300);
        break;
      }
    }
    previousSL_state = currentSL_state;

    delay(SENSOR_READ_DELAY);
  }
  if (!passRight && millis() - startTime >= 30000) {
    Serial.println(F("✗ Right TIMEOUT"));
  }

  // Test Up (JoyY > 1018)
  Serial.println(F("\nMove the joystick UP"));
  Serial.println(F("(Press SL if the test doesn't pass)"));
  bool passUp = false;
  previousSL_state = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    int valorY = analogRead(PIN_JOY_Y);
    Serial.print(F("JoyY: "));
    Serial.println(valorY);

    if (valorY > JOY_THRESHOLD_HIGH) {
      Serial.println(F("✓ Up OK"));
      passUp = true;
      delay(1000);
      break;
    }

    int currentSL_state = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && currentSL_state != previousSL_state) {
      delay(50);
      currentSL_state = digitalRead(PIN_BUTTON_SL);
      if (currentSL_state != previousSL_state) {
        Serial.println(F("✗ Up FAIL (indicated by user)"));
        delay(300);
        break;
      }
    }
    previousSL_state = currentSL_state;

    delay(SENSOR_READ_DELAY);
  }
  if (!passUp && millis() - startTime >= 30000) {
    Serial.println(F("✗ Up TIMEOUT"));
  }

  // Test Down (JoyY < 5)
  Serial.println(F("\nMove the joystick DOWN"));
  Serial.println(F("(Press SL if the test doesn't pass)"));
  bool passDown = false;
  previousSL_state = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    int valorY = analogRead(PIN_JOY_Y);
    Serial.print(F("JoyY: "));
    Serial.println(valorY);

    if (valorY < JOY_THRESHOLD_LOW) {
      Serial.println(F("✓ Down OK"));
      passDown = true;
      delay(1000);
      break;
    }

    int currentSL_state = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && currentSL_state != previousSL_state) {
      delay(50);
      currentSL_state = digitalRead(PIN_BUTTON_SL);
      if (currentSL_state != previousSL_state) {
        Serial.println(F("✗ Down FAIL (indicated by user)"));
        delay(300);
        break;
      }
    }
    previousSL_state = currentSL_state;

    delay(SENSOR_READ_DELAY);
  }
  if (!passDown && millis() - startTime >= 30000) {
    Serial.println(F("✗ Down TIMEOUT"));
  }

  bool joystickOK = passLeft && passRight && passUp && passDown;
  if (joystickOK) {
    Serial.println(F("\n✓ Joystick complete OK\n"));
    results.normalSensors++;
  } else {
    Serial.println(F("\n✗ Joystick FAIL\n"));
    results.normalSensorsFail++;
    failedSensorTests[numFailedSensorTests++] = "Joystick";
  }
}

void testAccelerometer() {
  Serial.println(F("TEST B: Accelerometer"));

  // Ensure buttons are released before starting
  waitForButtonsReleased();

  // Test Left (X < -7.8)
  Serial.println(F("\nTilt the board to the LEFT"));
  Serial.println(F("(Press SL if the test doesn't pass)"));
  bool passLeft = false;
  int previousSL_state = digitalRead(PIN_BUTTON_SL);
  unsigned long startTime = millis();
  while (millis() - startTime < 30000) {
    sensors_event_t event;
    lis.getEvent(&event);
    float x = event.acceleration.x;

    Serial.print(F("Accel X: "));
    Serial.println(x, 2);

    if (x < ACCEL_THRESHOLD_LOW) {
      Serial.println(F("✓ Left OK"));
      passLeft = true;
      delay(1000);
      break;
    }

    int currentSL_state = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && currentSL_state != previousSL_state) {
      delay(50);
      currentSL_state = digitalRead(PIN_BUTTON_SL);
      if (currentSL_state != previousSL_state) {
        Serial.println(F("✗ Left FAIL (indicated by user)"));
        delay(300);
        break;
      }
    }
    previousSL_state = currentSL_state;

    delay(SENSOR_READ_DELAY);
  }

  // Test Right (X > 7.8)
  Serial.println(F("\nTilt the board to the RIGHT"));
  Serial.println(F("(Press SL if the test doesn't pass)"));
  bool passRight = false;
  previousSL_state = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    sensors_event_t event;
    lis.getEvent(&event);
    float x = event.acceleration.x;

    Serial.print(F("Accel X: "));
    Serial.println(x, 2);

    if (x > ACCEL_THRESHOLD_HIGH) {
      Serial.println(F("✓ Right OK"));
      passRight = true;
      delay(1000);
      break;
    }

    int currentSL_state = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && currentSL_state != previousSL_state) {
      delay(50);
      currentSL_state = digitalRead(PIN_BUTTON_SL);
      if (currentSL_state != previousSL_state) {
        Serial.println(F("✗ Right FAIL (indicated by user)"));
        delay(300);
        break;
      }
    }
    previousSL_state = currentSL_state;

    delay(SENSOR_READ_DELAY);
  }

  // Test Up (Y > 7.8)
  Serial.println(F("\nTilt the board UP"));
  Serial.println(F("(Press SL if the test doesn't pass)"));
  bool passUp = false;
  previousSL_state = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    sensors_event_t event;
    lis.getEvent(&event);
    float y = event.acceleration.y;

    Serial.print(F("Accel Y: "));
    Serial.println(y, 2);

    if (y > ACCEL_THRESHOLD_HIGH) {
      Serial.println(F("✓ Up OK"));
      passUp = true;
      delay(1000);
      break;
    }

    int currentSL_state = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && currentSL_state != previousSL_state) {
      delay(50);
      currentSL_state = digitalRead(PIN_BUTTON_SL);
      if (currentSL_state != previousSL_state) {
        Serial.println(F("✗ Up FAIL (indicated by user)"));
        delay(300);
        break;
      }
    }
    previousSL_state = currentSL_state;

    delay(SENSOR_READ_DELAY);
  }

  // Test Down (Y < -7.8)
  Serial.println(F("\nTilt the board DOWN"));
  Serial.println(F("(Press SL if the test doesn't pass)"));
  bool passDown = false;
  previousSL_state = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    sensors_event_t event;
    lis.getEvent(&event);
    float y = event.acceleration.y;

    Serial.print(F("Accel Y: "));
    Serial.println(y, 2);

    if (y < ACCEL_THRESHOLD_LOW) {
      Serial.println(F("✓ Down OK"));
      passDown = true;
      delay(1000);
      break;
    }

    int currentSL_state = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && currentSL_state != previousSL_state) {
      delay(50);
      currentSL_state = digitalRead(PIN_BUTTON_SL);
      if (currentSL_state != previousSL_state) {
        Serial.println(F("✗ Down FAIL (indicated by user)"));
        delay(300);
        break;
      }
    }
    previousSL_state = currentSL_state;

    delay(SENSOR_READ_DELAY);
  }

  // Test Face Up (Z > 7.8)
  Serial.println(F("\nPlace the board FACE UP (components facing up)"));
  Serial.println(F("(Press SL if the test doesn't pass)"));
  bool passFaceUp = false;
  previousSL_state = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    sensors_event_t event;
    lis.getEvent(&event);
    float z = event.acceleration.z;

    Serial.print(F("Accel Z: "));
    Serial.println(z, 2);

    if (z > ACCEL_THRESHOLD_HIGH) {
      Serial.println(F("✓ Face Up OK"));
      passFaceUp = true;
      delay(1000);
      break;
    }

    int currentSL_state = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && currentSL_state != previousSL_state) {
      delay(50);
      currentSL_state = digitalRead(PIN_BUTTON_SL);
      if (currentSL_state != previousSL_state) {
        Serial.println(F("✗ Face Up FAIL (indicated by user)"));
        delay(300);
        break;
      }
    }
    previousSL_state = currentSL_state;

    delay(SENSOR_READ_DELAY);
  }

  // Test Face Down (Z < -7.8)
  Serial.println(F("\nPlace the board FACE DOWN (components facing down)"));
  Serial.println(F("(Press SL if the test doesn't pass)"));
  bool passFaceDown = false;
  previousSL_state = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    sensors_event_t event;
    lis.getEvent(&event);
    float z = event.acceleration.z;

    Serial.print(F("Accel Z: "));
    Serial.println(z, 2);

    if (z < ACCEL_THRESHOLD_LOW) {
      Serial.println(F("✓ Face Down OK"));
      passFaceDown = true;
      delay(1000);
      break;
    }

    int currentSL_state = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && currentSL_state != previousSL_state) {
      delay(50);
      currentSL_state = digitalRead(PIN_BUTTON_SL);
      if (currentSL_state != previousSL_state) {
        Serial.println(F("✗ Face Down FAIL (indicated by user)"));
        delay(300);
        break;
      }
    }
    previousSL_state = currentSL_state;

    delay(SENSOR_READ_DELAY);
  }

  bool accelOK = passLeft && passRight && passUp && passDown && passFaceUp && passFaceDown;
  if (accelOK) {
    Serial.println(F("\n✓ Accelerometer complete OK\n"));
    results.normalSensors++;
  } else {
    Serial.println(F("\n✗ Accelerometer FAIL\n"));
    results.normalSensorsFail++;
    failedSensorTests[numFailedSensorTests++] = "Accelerometer";
  }
}

void testLDR() {
  Serial.println(F("TEST C: LDR (Light sensor)"));

  // Ensure buttons are released before starting
  waitForButtonsReleased();

  Serial.println(F("\nCover the LDR completely"));
  Serial.println(F("(Press SL if test doesn't pass)"));

  bool passLDR = false;
  int previousSL_state = digitalRead(PIN_BUTTON_SL);
  unsigned long startTime = millis();
  while (millis() - startTime < 30000) {
    int value = analogRead(PIN_LDR);
    Serial.print(F("LDR: "));
    Serial.println(value);

    if (value < LDR_THRESHOLD_DARK) {
      Serial.println(F("✓ LDR OK"));
      passLDR = true;
      delay(1000);
      break;
    }

    // Detect state change in SL (test FAIL)
    int currentSL_state = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && currentSL_state != previousSL_state) {
      delay(50);
      currentSL_state = digitalRead(PIN_BUTTON_SL);
      if (currentSL_state != previousSL_state) {
        Serial.println(F("✗ LDR FAIL (indicated by user)"));
        delay(300);
        break;
      }
    }
    previousSL_state = currentSL_state;

    delay(SENSOR_READ_DELAY);
  }

  if (passLDR) {
    Serial.println(F("\n✓ LDR complete OK\n"));
    results.normalSensors++;
  } else {
    Serial.println(F("\n✗ LDR FAIL\n"));
    results.normalSensorsFail++;
    failedSensorTests[numFailedSensorTests++] = "LDR (Light sensor)";
  }
}

void testTemperature() {
  Serial.println(F("TEST D: Temperature Sensor"));
  Serial.println(F("\nReading temperature..."));

  // Take multiple readings and average
  long suma = 0;
  for (int i = 0; i < 10; i++) {
    suma += analogRead(PIN_TEMP);
    delay(100);
  }
  int valorPromedio = suma / 10;

  // Convert to degrees using sensor formula
  // Formula: (valorADC * 0.4658) - 50.0
  float temperatura = (valorPromedio * 0.4658) - 50.0;

  Serial.print(F("ADC Reading: "));
  Serial.println(valorPromedio);
  Serial.print(F("Temperature: "));
  Serial.print(temperatura, 2);
  Serial.println(F(" C"));

  Serial.println(F("\nIs the reading correct?"));
  bool resultado = askYesNo();

  if (resultado) {
    Serial.println(F("✓ Temperature OK\n"));
    results.normalSensors++;
  } else {
    Serial.println(F("✗ Temperature FAIL\n"));
    results.normalSensorsFail++;
    failedSensorTests[numFailedSensorTests++] = "Temperature Sensor";
  }
}

void testMicrophone() {
  Serial.println(F("TEST E: Microphone"));

  // Ensure buttons are released before starting
  waitForButtonsReleased();

  Serial.println(F("\nMake noise near the microphone"));
  Serial.println(F("(Average of 10 readings compared to threshold)"));
  Serial.println(F("(Press SL if the test doesn't pass)"));

  bool passMic = false;
  int previousSL_state = digitalRead(PIN_BUTTON_SL);
  unsigned long startTime = millis();
  while (millis() - startTime < 30000) {
    // Take 10 readings and calculate average
    long sum = 0;
    for (int i = 0; i < 10; i++) {
      sum += analogRead(PIN_MIC);
      delay(10);
    }
    int average = sum / 10;

    Serial.print(F("Microphone (avg of 10): "));
    Serial.println(average);

    if (average > MIC_THRESHOLD_NOISE) {
      Serial.println(F("✓ Microphone OK"));
      passMic = true;
      delay(1000);
      break;
    }

    // Detect state change in SL (button press)
    int currentSL_state = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && currentSL_state != previousSL_state) {
      delay(50); // Anti-bounce
      currentSL_state = digitalRead(PIN_BUTTON_SL);
      if (currentSL_state != previousSL_state) {
        Serial.println(F("✗ Microphone FAIL (indicated by user)"));
        delay(300);
        break;
      }
    }
    previousSL_state = currentSL_state;

    delay(SENSOR_READ_DELAY);
  }

  if (passMic) {
    Serial.println(F("\n✓ Microphone complete OK\n"));
    results.normalSensors++;
  } else {
    Serial.println(F("\n✗ Microphone FAIL\n"));
    results.normalSensorsFail++;
    failedSensorTests[numFailedSensorTests++] = "Microphone";
  }
}

void testIOPins() {
  Serial.println(F("TEST F: IO pins input test"));

  // Define the pins to test
  int ioPins[] = {PIN_IO_A2, PIN_IO_D4, PIN_IO_D7, PIN_IO_D8};
  String ioNames[] = {"A2", "D4", "D7", "D8"};
  int numPins = 4;

  int pinsPassedCount = 0;
  String failedPins = "";

  for (int i = 0; i < numPins; i++) {
    // Ensure buttons are released before starting
    waitForButtonsReleased();

    Serial.print(F("\n--- Testing pin "));
    Serial.print(ioNames[i]);
    Serial.println(F(" ---"));
    Serial.print(F("Short pin "));
    Serial.print(ioNames[i]);
    Serial.println(F(" to GND"));
    Serial.println(F("(Press SL if the test doesn't pass)"));

    bool passPin = false;
    int previousSL_state = digitalRead(PIN_BUTTON_SL);
    unsigned long startTime = millis();

    while (millis() - startTime < 30000) {
      // Read digital pin state
      int value = digitalRead(ioPins[i]);

      Serial.print(ioNames[i]);
      Serial.print(F(": "));
      Serial.println(value == HIGH ? "HIGH" : "LOW");

      // Pass if pin reads LOW (shorted to GND)
      if (value == LOW) {
        Serial.print(F("✓ Pin "));
        Serial.print(ioNames[i]);
        Serial.println(F(" OK"));
        passPin = true;
        delay(1000);
        break;
      }

      // Detect state change in SL (button press)
      int currentSL_state = digitalRead(PIN_BUTTON_SL);
      if (millis() - startTime > 1000 && currentSL_state != previousSL_state) {
        delay(50); // Anti-bounce
        currentSL_state = digitalRead(PIN_BUTTON_SL);
        if (currentSL_state != previousSL_state) {
          Serial.print(F("✗ Pin "));
          Serial.print(ioNames[i]);
          Serial.println(F(" FAIL (indicated by user)"));
          delay(300);
          break;
        }
      }
      previousSL_state = currentSL_state;

      delay(SENSOR_READ_DELAY);
    }

    if (passPin) {
      pinsPassedCount++;
    } else {
      if (failedPins.length() > 0) {
        failedPins += ", ";
      }
      failedPins += ioNames[i];
    }
  }

  // Register final result
  if (pinsPassedCount == numPins) {
    Serial.println(F("\n✓ IO pins input test complete OK\n"));
    results.normalSensors++;
  } else {
    Serial.print(F("\n✗ IO pins input test FAIL ("));
    Serial.print(pinsPassedCount);
    Serial.print(F("/"));
    Serial.print(numPins);
    Serial.println(F(" pins passed)"));
    if (failedPins.length() > 0) {
      Serial.print(F("Failed pins: "));
      Serial.println(failedPins);
    }
    Serial.println();
    results.normalSensorsFail++;
    String failMessage = "IO pins (" + failedPins + ")";
    failedSensorTests[numFailedSensorTests++] = failMessage;
  }
}

// ============================================
// TEST MKMK MODE
// ============================================
void testMkMkMode() {
  Serial.println(F("\n****************************************"));
  Serial.println(F("START MKMK MODE TEST"));
  Serial.println(F("****************************************\n"));

  Serial.println(F("SWITCH TO MKMK MODE"));
  Serial.println(F("Press SR when ready..."));
  Serial.print(F("If after "));
  Serial.print(MKMK_MAX_ATTEMPTS);
  Serial.println(F(" readings high level is not detected, test is considered failed"));
  waitForButtonSR();

  // Reinitialize pins D2 and D3 before test
  pinMode(PIN_BUTTON_SR, INPUT_PULLUP);
  pinMode(PIN_BUTTON_SL, INPUT_PULLUP);
  delay(100);

  // Pins to test in MkMk mode
  int pinesAnalogicos[] = {A0, A1, A2, A3, A6, A7};
  String analogNames[] = {"A0", "A1", "A2", "A3", "A6", "A7"};
  int numAnalogicos = 6;

  int digitalPins[] = {2, 3};
  String digitalNames[] = {"D2", "D3"};
  int numDigitales = 2;

  // Test analog pins
  for (int i = 0; i < numAnalogicos; i++) {
    Serial.print(F("\nTest "));
    Serial.println(analogNames[i]);
    Serial.print(F("Touch MkMk "));
    Serial.print(analogNames[i]);
    Serial.println(F(" and MkMk Man"));

    bool pass = false;
    int attempts = 0;

    while (attempts < MKMK_MAX_ATTEMPTS) {
      int value = analogRead(pinesAnalogicos[i]);
      Serial.print(analogNames[i]);
      Serial.print(F(": "));
      Serial.print(value);
      Serial.print(F(" ("));
      Serial.print(attempts + 1);
      Serial.print(F("/"));
      Serial.print(MKMK_MAX_ATTEMPTS);
      Serial.println(F(")"));

      if (value > MKMK_THRESHOLD) {
        Serial.print(F("✓ "));
        Serial.print(analogNames[i]);
        Serial.println(F(" OK"));
        pass = true;
        results.mkMk++;
        delay(1000);
        break;
      }

      attempts++;
      if (attempts < MKMK_MAX_ATTEMPTS) {
        delay(SENSOR_READ_DELAY);
      }
    }

    if (!pass) {
      Serial.print(F("✗ "));
      Serial.print(analogNames[i]);
      Serial.print(F(" FAIL ("));
      Serial.print(MKMK_MAX_ATTEMPTS);
      Serial.println(F(" attempts unsuccessful)"));
      results.mkMkFail++;
      failedMkMkTests[numFailedMkMkTests++] = "Pin " + analogNames[i];
    }
  }

  // Test digital pins
  for (int i = 0; i < numDigitales; i++) {
    Serial.print(F("\nTest "));
    Serial.println(digitalNames[i]);

    // Debug: show state before changing pinMode
    Serial.print(F("State before (with PULLUP): "));
    Serial.println(digitalRead(digitalPins[i]));

    Serial.print(F("Touch MkMk "));
    Serial.print(digitalNames[i]);
    Serial.println(F(" and MkMk Man"));

    pinMode(digitalPins[i], INPUT);
    delay(10); // Small delay to stabilize

    bool pass = false;
    int attempts = 0;

    while (attempts < MKMK_MAX_ATTEMPTS) {
      int value = digitalRead(digitalPins[i]);
      Serial.print(digitalNames[i]);
      Serial.print(F(": "));
      Serial.print(value);
      Serial.print(F(" ("));
      Serial.print(attempts + 1);
      Serial.print(F("/"));
      Serial.print(MKMK_MAX_ATTEMPTS);
      Serial.println(F(")"));

      if (value == HIGH) {
        Serial.print(F("✓ "));
        Serial.print(digitalNames[i]);
        Serial.println(F(" OK"));
        pass = true;
        results.mkMk++;
        delay(1000);
        break;
      }

      attempts++;
      if (attempts < MKMK_MAX_ATTEMPTS) {
        delay(SENSOR_READ_DELAY);
      }
    }

    if (!pass) {
      Serial.print(F("✗ "));
      Serial.print(digitalNames[i]);
      Serial.print(F(" FAIL ("));
      Serial.print(MKMK_MAX_ATTEMPTS);
      Serial.println(F(" attempts unsuccessful)"));
      results.mkMkFail++;
      failedMkMkTests[numFailedMkMkTests++] = "Pin " + digitalNames[i];
    }

    // Restore pin configuration
    pinMode(digitalPins[i], INPUT_PULLUP);
  }

  Serial.println(F("\n****************************************"));
  Serial.println(F("END MKMK MODE TEST"));
  Serial.println(F("****************************************\n"));
}

// ============================================
// FINAL REPORT
// ============================================
void finalReport() {
  Serial.println(F("\n\n"));
  Serial.println(F("========================================"));
  Serial.println(F("       FINAL RESULTS REPORT"));
  Serial.println(F("========================================\n"));

  Serial.println(F("--- NORMAL MODE ---"));
  Serial.print(F("Actuators OK: "));
  Serial.println(results.normalActuators);
  Serial.print(F("Actuators FAIL: "));
  Serial.println(results.normalActuatorsFail);

  if (numFailedActuatorTests > 0) {
    Serial.println(F("  Failed tests:"));
    for (int i = 0; i < numFailedActuatorTests; i++) {
      Serial.print(F("    - "));
      Serial.println(failedActuatorTests[i]);
    }
  }

  Serial.print(F("Sensors OK: "));
  Serial.println(results.normalSensors);
  Serial.print(F("Sensors FAIL: "));
  Serial.println(results.normalSensorsFail);

  if (numFailedSensorTests > 0) {
    Serial.println(F("  Failed tests:"));
    for (int i = 0; i < numFailedSensorTests; i++) {
      Serial.print(F("    - "));
      Serial.println(failedSensorTests[i]);
    }
  }

  int totalNormal = results.normalActuators + results.normalSensors;
  int totalNormalFail = results.normalActuatorsFail + results.normalSensorsFail;
  Serial.print(F("\nTotal Normal Mode OK: "));
  Serial.print(totalNormal);
  Serial.print(F(" / "));
  Serial.println(totalNormal + totalNormalFail);

  Serial.println(F("\n--- MKMK MODE ---"));
  Serial.print(F("Pins OK: "));
  Serial.println(results.mkMk);
  Serial.print(F("Pins FAIL: "));
  Serial.println(results.mkMkFail);

  if (numFailedMkMkTests > 0) {
    Serial.println(F("  Failed pins:"));
    for (int i = 0; i < numFailedMkMkTests; i++) {
      Serial.print(F("    - "));
      Serial.println(failedMkMkTests[i]);
    }
  }

  Serial.print(F("\nTotal MkMk Mode OK: "));
  Serial.print(results.mkMk);
  Serial.print(F(" / "));
  Serial.println(results.mkMk + results.mkMkFail);

  Serial.println(F("\n--- GENERAL SUMMARY ---"));
  int totalTests = totalNormal + totalNormalFail + results.mkMk + results.mkMkFail;
  int totalOK = totalNormal + results.mkMk;
  float porcentaje = (totalOK * 100.0) / totalTests;

  Serial.print(F("Successful tests: "));
  Serial.print(totalOK);
  Serial.print(F(" / "));
  Serial.print(totalTests);
  Serial.print(F(" ("));
  Serial.print(porcentaje, 1);
  Serial.println(F("%)"));

  Serial.println(F("\n========================================"));

  if (totalOK == totalTests) {
    Serial.println(F("     ✓✓✓ ALL TESTS PASSED ✓✓✓"));
  } else if (porcentaje >= 80) {
    Serial.println(F("     ⚠ SOME TESTS FAILED ⚠"));
  } else {
    Serial.println(F("     ✗✗✗ MULTIPLE FAILURES ✗✗✗"));
  }

  Serial.println(F("========================================"));
}
