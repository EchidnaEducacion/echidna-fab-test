/*
 * Test FAB Echidna - Programa de test para placa educativa Arduino Nano
 * Prueba secuencial de actuadores y sensores en modo Normal y modo MkMk
 */

#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// ============================================
// DEFINICIÓN DE PINES - MODO NORMAL
// ============================================
// Actuadores
#define PIN_BUZZER 10
#define PIN_LED_VERDE 11
#define PIN_LED_NARANJA 12
#define PIN_LED_ROJO 13
#define PIN_RGB_R 9
#define PIN_RGB_G 6
#define PIN_RGB_B 5

// Pulsadores
#define PIN_BUTTON_SR 2  // Pulsador derecho (Sí/Correcto)
#define PIN_BUTTON_SL 3  // Pulsador izquierdo (No/Incorrecto)

// Sensores analógicos
#define PIN_JOY_X A0
#define PIN_JOY_Y A1
#define PIN_LDR A3
#define PIN_TEMP A6
#define PIN_MIC A7

// I2C - Acelerómetro
#define LIS3DHTR_ADDR 0x18

// ============================================
// UMBRALES DE SENSORES
// ============================================
// Joystick
#define JOY_THRESHOLD_LOW 5
#define JOY_THRESHOLD_HIGH 1018

// Acelerómetro (valores en m/s²)
#define ACCEL_THRESHOLD_LOW -7.8
#define ACCEL_THRESHOLD_HIGH 7.8

// LDR
#define LDR_THRESHOLD_DARK 30

// Micrófono
#define MIC_THRESHOLD_NOISE 50

// MkMk
#define MKMK_THRESHOLD 100

// Tiempo de actualización de lecturas
#define SENSOR_READ_DELAY 500

// ============================================
// VARIABLES GLOBALES
// ============================================
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Estructura para guardar resultados
struct TestResults {
  int normalActuadores;
  int normalSensores;
  int mkMk;
  int normalActuadoresFail;
  int normalSensoresFail;
  int mkMkFail;
};

TestResults results = {0, 0, 0, 0, 0, 0};

// ============================================
// SETUP
// ============================================
void setup() {
  Serial.begin(9600);
  while (!Serial); // Esperar conexión serial

  // Configurar pines de salida
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_NARANJA, OUTPUT);
  pinMode(PIN_LED_ROJO, OUTPUT);
  pinMode(PIN_RGB_R, OUTPUT);
  pinMode(PIN_RGB_G, OUTPUT);
  pinMode(PIN_RGB_B, OUTPUT);

  // Configurar pines de entrada
  pinMode(PIN_BUTTON_SR, INPUT_PULLUP);
  pinMode(PIN_BUTTON_SL, INPUT_PULLUP);

  // Apagar todos los actuadores
  apagarActuadores();

  // Inicializar acelerómetro
  if (lis.begin(LIS3DHTR_ADDR)) {
    lis.setRange(LIS3DH_RANGE_2_G);
    lis.setDataRate(LIS3DH_DATARATE_50_HZ);
    Serial.println(F("Acelerometro inicializado correctamente"));
  } else {
    Serial.println(F("ERROR: No se pudo inicializar el acelerometro"));
  }

  delay(1000);
}

// ============================================
// LOOP PRINCIPAL
// ============================================
void loop() {
  Serial.println(F("\n========================================"));
  Serial.println(F("TEST FAB ECHIDNA - Inicio"));
  Serial.println(F("========================================\n"));

  // Test Modo Normal
  testModoNormal();

  // Test Modo MkMk
  testModoMkMk();

  // Reporte final
  reporteFinal();

  // Esperar para reiniciar
  Serial.println(F("\n\nTest completado. Presione RESET para reiniciar."));
  while (true); // Detener ejecución
}

// ============================================
// FUNCIONES DE UTILIDAD
// ============================================
void apagarActuadores() {
  digitalWrite(PIN_LED_VERDE, LOW);
  digitalWrite(PIN_LED_NARANJA, LOW);
  digitalWrite(PIN_LED_ROJO, LOW);
  digitalWrite(PIN_RGB_R, LOW);
  digitalWrite(PIN_RGB_G, LOW);
  digitalWrite(PIN_RGB_B, LOW);
  digitalWrite(PIN_BUZZER, LOW);
}

bool esperarBotonSR() {
  Serial.println(F("Presione SR (pulsador derecho) para continuar..."));
  while (digitalRead(PIN_BUTTON_SR) == HIGH) {
    delay(10);
  }
  delay(200); // Debounce
  while (digitalRead(PIN_BUTTON_SR) == LOW) {
    delay(10);
  }
  return true;
}

bool esperarBotonSL() {
  Serial.println(F("Presione SL (pulsador izquierdo) para continuar..."));
  while (digitalRead(PIN_BUTTON_SL) == HIGH) {
    delay(10);
  }
  delay(200); // Debounce
  while (digitalRead(PIN_BUTTON_SL) == LOW) {
    delay(10);
  }
  return true;
}

bool preguntarSiNo() {
  Serial.println(F("SR=Si / SL=No"));

  // Esperar estabilización y limpiar rebotes
  delay(500);

  // Leer estado inicial estable
  int estadoSR_anterior = digitalRead(PIN_BUTTON_SR);
  int estadoSL_anterior = digitalRead(PIN_BUTTON_SL);

  unsigned long tiempoInicio = millis();

  while (true) {
    // Leer estado actual
    int estadoSR_actual = digitalRead(PIN_BUTTON_SR);
    int estadoSL_actual = digitalRead(PIN_BUTTON_SL);

    // Solo detectar cambios después de 500ms adicionales (evitar cambios iniciales)
    if (millis() - tiempoInicio > 500) {
      // Detectar cambio en SR (presionado)
      if (estadoSR_actual != estadoSR_anterior) {
        delay(50); // Anti-rebote
        estadoSR_actual = digitalRead(PIN_BUTTON_SR);
        if (estadoSR_actual != estadoSR_anterior) {
          delay(300); // Delay adicional
          return true;
        }
      }

      // Detectar cambio en SL (presionado)
      if (estadoSL_actual != estadoSL_anterior) {
        delay(50); // Anti-rebote
        estadoSL_actual = digitalRead(PIN_BUTTON_SL);
        if (estadoSL_actual != estadoSL_anterior) {
          delay(300); // Delay adicional
          return false;
        }
      }
    }

    // Actualizar estados anteriores
    estadoSR_anterior = estadoSR_actual;
    estadoSL_anterior = estadoSL_actual;

    delay(10);
  }
}

void esperarBotonesLibres() {
  // Delay para limpiar rebotes y estabilizar lecturas
  delay(500);
}

// ============================================
// TEST MODO NORMAL
// ============================================
void testModoNormal() {
  Serial.println(F("\n****************************************"));
  Serial.println(F("INICIO TEST MODO NORMAL"));
  Serial.println(F("****************************************\n"));

  Serial.println(F("Para comenzar el test:"));
  Serial.println(F("1. Presione SR (pulsador derecho)"));
  Serial.println(F("2. Luego presione SL (pulsador izquierdo)"));

  esperarBotonSR();
  esperarBotonSL();

  Serial.println(F("\nPulsadores verificados!"));
  Serial.println(F("\n--- COMENZANDO TEST DE ACTUADORES ---\n"));

  // Test Actuadores
  testLEDs();
  testLEDRGB();
  testZumbador();

  Serial.println(F("\n--- COMENZANDO TEST DE SENSORES ---\n"));

  // Test Sensores
  testJoystick();
  testAcelerometro();
  testLDR();
  testTemperatura();
  testMicrofono();

  Serial.println(F("\n****************************************"));
  Serial.println(F("FIN TEST MODO NORMAL"));
  Serial.println(F("****************************************\n"));
}

// ============================================
// TEST DE ACTUADORES
// ============================================
void testLEDs() {
  Serial.println(F("TEST A: LEDs (Verde, Naranja, Rojo)"));
  Serial.println(F("Encendiendo los 3 LEDs..."));

  digitalWrite(PIN_LED_VERDE, HIGH);
  digitalWrite(PIN_LED_NARANJA, HIGH);
  digitalWrite(PIN_LED_ROJO, HIGH);

  Serial.println(F("¿Estan los 3 LEDs encendidos?"));
  bool resultado = preguntarSiNo();

  digitalWrite(PIN_LED_VERDE, LOW);
  digitalWrite(PIN_LED_NARANJA, LOW);
  digitalWrite(PIN_LED_ROJO, LOW);

  if (resultado) {
    Serial.println(F("✓ LEDs OK\n"));
    results.normalActuadores++;
  } else {
    Serial.println(F("✗ LEDs FAIL\n"));
    results.normalActuadoresFail++;
  }
}

void testLEDRGB() {
  Serial.println(F("TEST B: LED RGB"));
  Serial.println(F("Mostrando 3 colores consecutivamente..."));

  // Rojo
  Serial.println(F("Color 1: Rojo"));
  digitalWrite(PIN_RGB_R, HIGH);
  delay(1500);
  digitalWrite(PIN_RGB_R, LOW);
  delay(500);

  // Verde
  Serial.println(F("Color 2: Verde"));
  digitalWrite(PIN_RGB_G, HIGH);
  delay(1500);
  digitalWrite(PIN_RGB_G, LOW);
  delay(500);

  // Azul
  Serial.println(F("Color 3: Azul"));
  digitalWrite(PIN_RGB_B, HIGH);
  delay(1500);
  digitalWrite(PIN_RGB_B, LOW);

  Serial.println(F("¿Se han visto los 3 colores?"));
  bool resultado = preguntarSiNo();

  if (resultado) {
    Serial.println(F("✓ LED RGB OK\n"));
    results.normalActuadores++;
  } else {
    Serial.println(F("✗ LED RGB FAIL\n"));
    results.normalActuadoresFail++;
  }
}

void testZumbador() {
  Serial.println(F("TEST C: Zumbador"));
  Serial.println(F("Emitiendo pitido intermitente..."));
  Serial.println(F("(Ajustar volumen con potenciometro si es necesario)"));

  // Pitidos intermitentes durante 3 segundos
  for (int i = 0; i < 6; i++) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(250);
    digitalWrite(PIN_BUZZER, LOW);
    delay(250);
  }

  Serial.println(F("¿Se escucha el zumbador?"));
  bool resultado = preguntarSiNo();

  digitalWrite(PIN_BUZZER, LOW);

  if (resultado) {
    Serial.println(F("✓ Zumbador OK\n"));
    results.normalActuadores++;
  } else {
    Serial.println(F("✗ Zumbador FAIL\n"));
    results.normalActuadoresFail++;
  }
}

// ============================================
// TEST DE SENSORES
// ============================================
void testJoystick() {
  Serial.println(F("TEST A: Joystick"));

  // Asegurar que los botones estén liberados antes de comenzar
  esperarBotonesLibres();

  // Debug: mostrar estado de botones
  Serial.print(F("Estado SR: "));
  Serial.print(digitalRead(PIN_BUTTON_SR));
  Serial.print(F(" / SL: "));
  Serial.println(digitalRead(PIN_BUTTON_SL));

  // Test Izquierda (JoyX < 5)
  Serial.println(F("\nMueva el joystick a la IZQUIERDA"));
  Serial.println(F("(Presione SL si el test no pasa)"));
  bool passIzq = false;
  int estadoSL_anterior = digitalRead(PIN_BUTTON_SL);
  unsigned long startTime = millis();
  while (millis() - startTime < 30000) {
    int valorX = analogRead(PIN_JOY_X);
    Serial.print(F("JoyX: "));
    Serial.println(valorX);

    if (valorX < JOY_THRESHOLD_LOW) {
      Serial.println(F("✓ Izquierda OK"));
      passIzq = true;
      delay(1000);
      break;
    }

    // Detectar cambio de estado en SL (presión del botón)
    int estadoSL_actual = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && estadoSL_actual != estadoSL_anterior) {
      delay(50); // Anti-rebote
      estadoSL_actual = digitalRead(PIN_BUTTON_SL);
      if (estadoSL_actual != estadoSL_anterior) {
        Serial.println(F("✗ Izquierda FAIL (indicado por usuario)"));
        delay(300);
        break;
      }
    }
    estadoSL_anterior = estadoSL_actual;

    delay(SENSOR_READ_DELAY);
  }
  if (!passIzq && millis() - startTime >= 30000) {
    Serial.println(F("✗ Izquierda TIMEOUT"));
  }

  // Test Derecha (JoyX > 1018)
  Serial.println(F("\nMueva el joystick a la DERECHA"));
  Serial.println(F("(Presione SL si el test no pasa)"));
  bool passDer = false;
  estadoSL_anterior = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    int valorX = analogRead(PIN_JOY_X);
    Serial.print(F("JoyX: "));
    Serial.println(valorX);

    if (valorX > JOY_THRESHOLD_HIGH) {
      Serial.println(F("✓ Derecha OK"));
      passDer = true;
      delay(1000);
      break;
    }

    int estadoSL_actual = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && estadoSL_actual != estadoSL_anterior) {
      delay(50);
      estadoSL_actual = digitalRead(PIN_BUTTON_SL);
      if (estadoSL_actual != estadoSL_anterior) {
        Serial.println(F("✗ Derecha FAIL (indicado por usuario)"));
        delay(300);
        break;
      }
    }
    estadoSL_anterior = estadoSL_actual;

    delay(SENSOR_READ_DELAY);
  }
  if (!passDer && millis() - startTime >= 30000) {
    Serial.println(F("✗ Derecha TIMEOUT"));
  }

  // Test Arriba (JoyY > 1018)
  Serial.println(F("\nMueva el joystick ARRIBA"));
  Serial.println(F("(Presione SL si el test no pasa)"));
  bool passArr = false;
  estadoSL_anterior = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    int valorY = analogRead(PIN_JOY_Y);
    Serial.print(F("JoyY: "));
    Serial.println(valorY);

    if (valorY > JOY_THRESHOLD_HIGH) {
      Serial.println(F("✓ Arriba OK"));
      passArr = true;
      delay(1000);
      break;
    }

    int estadoSL_actual = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && estadoSL_actual != estadoSL_anterior) {
      delay(50);
      estadoSL_actual = digitalRead(PIN_BUTTON_SL);
      if (estadoSL_actual != estadoSL_anterior) {
        Serial.println(F("✗ Arriba FAIL (indicado por usuario)"));
        delay(300);
        break;
      }
    }
    estadoSL_anterior = estadoSL_actual;

    delay(SENSOR_READ_DELAY);
  }
  if (!passArr && millis() - startTime >= 30000) {
    Serial.println(F("✗ Arriba TIMEOUT"));
  }

  // Test Abajo (JoyY < 5)
  Serial.println(F("\nMueva el joystick ABAJO"));
  Serial.println(F("(Presione SL si el test no pasa)"));
  bool passAba = false;
  estadoSL_anterior = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    int valorY = analogRead(PIN_JOY_Y);
    Serial.print(F("JoyY: "));
    Serial.println(valorY);

    if (valorY < JOY_THRESHOLD_LOW) {
      Serial.println(F("✓ Abajo OK"));
      passAba = true;
      delay(1000);
      break;
    }

    int estadoSL_actual = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && estadoSL_actual != estadoSL_anterior) {
      delay(50);
      estadoSL_actual = digitalRead(PIN_BUTTON_SL);
      if (estadoSL_actual != estadoSL_anterior) {
        Serial.println(F("✗ Abajo FAIL (indicado por usuario)"));
        delay(300);
        break;
      }
    }
    estadoSL_anterior = estadoSL_actual;

    delay(SENSOR_READ_DELAY);
  }
  if (!passAba && millis() - startTime >= 30000) {
    Serial.println(F("✗ Abajo TIMEOUT"));
  }

  bool joystickOK = passIzq && passDer && passArr && passAba;
  if (joystickOK) {
    Serial.println(F("\n✓ Joystick completo OK\n"));
    results.normalSensores++;
  } else {
    Serial.println(F("\n✗ Joystick FAIL\n"));
    results.normalSensoresFail++;
  }
}

void testAcelerometro() {
  Serial.println(F("TEST B: Acelerometro"));

  // Asegurar que los botones estén liberados antes de comenzar
  esperarBotonesLibres();

  // Test Izquierda (X < -7.8)
  Serial.println(F("\nIncline la placa hacia la IZQUIERDA"));
  Serial.println(F("(Presione SL si el test no pasa)"));
  bool passIzq = false;
  int estadoSL_anterior = digitalRead(PIN_BUTTON_SL);
  unsigned long startTime = millis();
  while (millis() - startTime < 30000) {
    sensors_event_t event;
    lis.getEvent(&event);
    float x = event.acceleration.x;

    Serial.print(F("Accel X: "));
    Serial.println(x, 2);

    if (x < ACCEL_THRESHOLD_LOW) {
      Serial.println(F("✓ Izquierda OK"));
      passIzq = true;
      delay(1000);
      break;
    }

    int estadoSL_actual = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && estadoSL_actual != estadoSL_anterior) {
      delay(50);
      estadoSL_actual = digitalRead(PIN_BUTTON_SL);
      if (estadoSL_actual != estadoSL_anterior) {
        Serial.println(F("✗ Izquierda FAIL (indicado por usuario)"));
        delay(300);
        break;
      }
    }
    estadoSL_anterior = estadoSL_actual;

    delay(SENSOR_READ_DELAY);
  }

  // Test Derecha (X > 7.8)
  Serial.println(F("\nIncline la placa hacia la DERECHA"));
  Serial.println(F("(Presione SL si el test no pasa)"));
  bool passDer = false;
  estadoSL_anterior = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    sensors_event_t event;
    lis.getEvent(&event);
    float x = event.acceleration.x;

    Serial.print(F("Accel X: "));
    Serial.println(x, 2);

    if (x > ACCEL_THRESHOLD_HIGH) {
      Serial.println(F("✓ Derecha OK"));
      passDer = true;
      delay(1000);
      break;
    }

    int estadoSL_actual = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && estadoSL_actual != estadoSL_anterior) {
      delay(50);
      estadoSL_actual = digitalRead(PIN_BUTTON_SL);
      if (estadoSL_actual != estadoSL_anterior) {
        Serial.println(F("✗ Derecha FAIL (indicado por usuario)"));
        delay(300);
        break;
      }
    }
    estadoSL_anterior = estadoSL_actual;

    delay(SENSOR_READ_DELAY);
  }

  // Test Arriba (Y > 7.8)
  Serial.println(F("\nIncline la placa ARRIBA"));
  Serial.println(F("(Presione SL si el test no pasa)"));
  bool passArr = false;
  estadoSL_anterior = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    sensors_event_t event;
    lis.getEvent(&event);
    float y = event.acceleration.y;

    Serial.print(F("Accel Y: "));
    Serial.println(y, 2);

    if (y > ACCEL_THRESHOLD_HIGH) {
      Serial.println(F("✓ Arriba OK"));
      passArr = true;
      delay(1000);
      break;
    }

    int estadoSL_actual = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && estadoSL_actual != estadoSL_anterior) {
      delay(50);
      estadoSL_actual = digitalRead(PIN_BUTTON_SL);
      if (estadoSL_actual != estadoSL_anterior) {
        Serial.println(F("✗ Arriba FAIL (indicado por usuario)"));
        delay(300);
        break;
      }
    }
    estadoSL_anterior = estadoSL_actual;

    delay(SENSOR_READ_DELAY);
  }

  // Test Abajo (Y < -7.8)
  Serial.println(F("\nIncline la placa ABAJO"));
  Serial.println(F("(Presione SL si el test no pasa)"));
  bool passAba = false;
  estadoSL_anterior = digitalRead(PIN_BUTTON_SL);
  startTime = millis();
  while (millis() - startTime < 30000) {
    sensors_event_t event;
    lis.getEvent(&event);
    float y = event.acceleration.y;

    Serial.print(F("Accel Y: "));
    Serial.println(y, 2);

    if (y < ACCEL_THRESHOLD_LOW) {
      Serial.println(F("✓ Abajo OK"));
      passAba = true;
      delay(1000);
      break;
    }

    int estadoSL_actual = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && estadoSL_actual != estadoSL_anterior) {
      delay(50);
      estadoSL_actual = digitalRead(PIN_BUTTON_SL);
      if (estadoSL_actual != estadoSL_anterior) {
        Serial.println(F("✗ Abajo FAIL (indicado por usuario)"));
        delay(300);
        break;
      }
    }
    estadoSL_anterior = estadoSL_actual;

    delay(SENSOR_READ_DELAY);
  }

  bool accelOK = passIzq && passDer && passArr && passAba;
  if (accelOK) {
    Serial.println(F("\n✓ Acelerometro completo OK\n"));
    results.normalSensores++;
  } else {
    Serial.println(F("\n✗ Acelerometro FAIL\n"));
    results.normalSensoresFail++;
  }
}

void testLDR() {
  Serial.println(F("TEST C: LDR (Sensor de luz)"));

  // Asegurar que los botones estén liberados antes de comenzar
  esperarBotonesLibres();

  Serial.println(F("\nCubra completamente el LDR"));
  Serial.println(F("(Presione SL si el test no pasa)"));

  bool passLDR = false;
  int estadoSL_anterior = digitalRead(PIN_BUTTON_SL);
  unsigned long startTime = millis();
  while (millis() - startTime < 30000) {
    int valor = analogRead(PIN_LDR);
    Serial.print(F("LDR: "));
    Serial.println(valor);

    if (valor < LDR_THRESHOLD_DARK) {
      Serial.println(F("✓ LDR OK"));
      passLDR = true;
      delay(1000);
      break;
    }

    // Detectar cambio de estado en SL
    int estadoSL_actual = digitalRead(PIN_BUTTON_SL);
    if (millis() - startTime > 1000 && estadoSL_actual != estadoSL_anterior) {
      delay(50);
      estadoSL_actual = digitalRead(PIN_BUTTON_SL);
      if (estadoSL_actual != estadoSL_anterior) {
        Serial.println(F("✗ LDR FAIL (indicado por usuario)"));
        delay(300);
        break;
      }
    }
    estadoSL_anterior = estadoSL_actual;

    delay(SENSOR_READ_DELAY);
  }

  if (passLDR) {
    Serial.println(F("\n✓ LDR completo OK\n"));
    results.normalSensores++;
  } else {
    Serial.println(F("\n✗ LDR FAIL\n"));
    results.normalSensoresFail++;
  }
}

void testTemperatura() {
  Serial.println(F("TEST D: Sensor de Temperatura"));
  Serial.println(F("\nLeyendo temperatura..."));

  // Hacer varias lecturas y promediar
  long suma = 0;
  for (int i = 0; i < 10; i++) {
    suma += analogRead(PIN_TEMP);
    delay(100);
  }
  int valorPromedio = suma / 10;

  // Convertir a grados (aproximado, depende del sensor específico)
  // Asumiendo un sensor tipo TMP36: (valor * 5000 / 1024 - 500) / 10
  float voltaje = valorPromedio * 5000.0 / 1024.0;
  float temperatura = (voltaje - 500) / 10.0;

  Serial.print(F("Lectura ADC: "));
  Serial.println(valorPromedio);
  Serial.print(F("Temperatura aprox: "));
  Serial.print(temperatura);
  Serial.println(F(" C"));

  Serial.println(F("\n¿Es la lectura correcta?"));
  bool resultado = preguntarSiNo();

  if (resultado) {
    Serial.println(F("✓ Temperatura OK\n"));
    results.normalSensores++;
  } else {
    Serial.println(F("✗ Temperatura FAIL\n"));
    results.normalSensoresFail++;
  }
}

void testMicrofono() {
  Serial.println(F("TEST E: Microfono"));

  // Asegurar que los botones estén liberados antes de comenzar
  esperarBotonesLibres();

  Serial.println(F("\nHaga ruido cerca del microfono"));

  bool passMic = false;
  unsigned long startTime = millis();
  while (millis() - startTime < 30000) {
    int valor = analogRead(PIN_MIC);
    int baseline = 512; // Valor medio esperado
    int amplitud = abs(valor - baseline);

    Serial.print(F("Microfono: "));
    Serial.print(valor);
    Serial.print(F(" (Amplitud: "));
    Serial.print(amplitud);
    Serial.println(F(")"));

    if (amplitud > MIC_THRESHOLD_NOISE) {
      Serial.println(F("✓ Microfono OK"));
      passMic = true;
      delay(1000);
      break;
    }

    delay(SENSOR_READ_DELAY);
  }

  if (passMic) {
    Serial.println(F("\n✓ Microfono completo OK\n"));
    results.normalSensores++;
  } else {
    Serial.println(F("\n✗ Microfono FAIL\n"));
    results.normalSensoresFail++;
  }
}

// ============================================
// TEST MODO MKMK
// ============================================
void testModoMkMk() {
  Serial.println(F("\n****************************************"));
  Serial.println(F("INICIO TEST MODO MKMK"));
  Serial.println(F("****************************************\n"));

  Serial.println(F("CAMBIE EL INTERRUPTOR A MODO MKMK"));
  Serial.println(F("Presione SR cuando este listo..."));
  esperarBotonSR();

  // Reinicializar pines D2 y D3 antes del test
  pinMode(PIN_BUTTON_SR, INPUT_PULLUP);
  pinMode(PIN_BUTTON_SL, INPUT_PULLUP);
  delay(100);

  // Pines a probar en modo MkMk
  int pinesAnalogicos[] = {A0, A1, A2, A3, A6, A7};
  String nombresAnalogicos[] = {"A0", "A1", "A2", "A3", "A6", "A7"};
  int numAnalogicos = 6;

  int pinesDigitales[] = {2, 3};
  String nombresDigitales[] = {"D2", "D3"};
  int numDigitales = 2;

  // Test pines analógicos
  for (int i = 0; i < numAnalogicos; i++) {
    Serial.print(F("\nTest "));
    Serial.println(nombresAnalogicos[i]);
    Serial.print(F("Toque MkMk "));
    Serial.print(nombresAnalogicos[i]);
    Serial.println(F(" y Hombre MkMk"));

    bool pass = false;
    unsigned long startTime = millis();
    while (millis() - startTime < 30000) {
      int valor = analogRead(pinesAnalogicos[i]);
      Serial.print(nombresAnalogicos[i]);
      Serial.print(F(": "));
      Serial.println(valor);

      if (valor > MKMK_THRESHOLD) {
        Serial.print(F("✓ "));
        Serial.print(nombresAnalogicos[i]);
        Serial.println(F(" OK"));
        pass = true;
        results.mkMk++;
        delay(1000);
        break;
      }

      delay(SENSOR_READ_DELAY);
    }

    if (!pass) {
      Serial.print(F("✗ "));
      Serial.print(nombresAnalogicos[i]);
      Serial.println(F(" TIMEOUT"));
      results.mkMkFail++;
    }
  }

  // Test pines digitales
  for (int i = 0; i < numDigitales; i++) {
    Serial.print(F("\nTest "));
    Serial.println(nombresDigitales[i]);

    // Debug: mostrar estado antes de cambiar pinMode
    Serial.print(F("Estado antes (con PULLUP): "));
    Serial.println(digitalRead(pinesDigitales[i]));

    Serial.print(F("Toque MkMk "));
    Serial.print(nombresDigitales[i]);
    Serial.println(F(" y Hombre MkMk"));

    pinMode(pinesDigitales[i], INPUT);
    delay(10); // Pequeño delay para estabilizar

    bool pass = false;
    unsigned long startTime = millis();
    while (millis() - startTime < 30000) {
      int valor = digitalRead(pinesDigitales[i]);
      Serial.print(nombresDigitales[i]);
      Serial.print(F(": "));
      Serial.println(valor);

      if (valor == HIGH) {
        Serial.print(F("✓ "));
        Serial.print(nombresDigitales[i]);
        Serial.println(F(" OK"));
        pass = true;
        results.mkMk++;
        delay(1000);
        break;
      }

      delay(SENSOR_READ_DELAY);
    }

    if (!pass) {
      Serial.print(F("✗ "));
      Serial.print(nombresDigitales[i]);
      Serial.println(F(" TIMEOUT"));
      results.mkMkFail++;
    }

    // Restaurar configuración de pines
    pinMode(pinesDigitales[i], INPUT_PULLUP);
  }

  Serial.println(F("\n****************************************"));
  Serial.println(F("FIN TEST MODO MKMK"));
  Serial.println(F("****************************************\n"));
}

// ============================================
// REPORTE FINAL
// ============================================
void reporteFinal() {
  Serial.println(F("\n\n"));
  Serial.println(F("========================================"));
  Serial.println(F("       REPORTE FINAL DE RESULTADOS"));
  Serial.println(F("========================================\n"));

  Serial.println(F("--- MODO NORMAL ---"));
  Serial.print(F("Actuadores OK: "));
  Serial.println(results.normalActuadores);
  Serial.print(F("Actuadores FAIL: "));
  Serial.println(results.normalActuadoresFail);
  Serial.print(F("Sensores OK: "));
  Serial.println(results.normalSensores);
  Serial.print(F("Sensores FAIL: "));
  Serial.println(results.normalSensoresFail);

  int totalNormal = results.normalActuadores + results.normalSensores;
  int totalNormalFail = results.normalActuadoresFail + results.normalSensoresFail;
  Serial.print(F("\nTotal Modo Normal OK: "));
  Serial.print(totalNormal);
  Serial.print(F(" / "));
  Serial.println(totalNormal + totalNormalFail);

  Serial.println(F("\n--- MODO MKMK ---"));
  Serial.print(F("Pines OK: "));
  Serial.println(results.mkMk);
  Serial.print(F("Pines FAIL: "));
  Serial.println(results.mkMkFail);

  Serial.print(F("\nTotal Modo MkMk OK: "));
  Serial.print(results.mkMk);
  Serial.print(F(" / "));
  Serial.println(results.mkMk + results.mkMkFail);

  Serial.println(F("\n--- RESUMEN GENERAL ---"));
  int totalTests = totalNormal + totalNormalFail + results.mkMk + results.mkMkFail;
  int totalOK = totalNormal + results.mkMk;
  float porcentaje = (totalOK * 100.0) / totalTests;

  Serial.print(F("Tests exitosos: "));
  Serial.print(totalOK);
  Serial.print(F(" / "));
  Serial.print(totalTests);
  Serial.print(F(" ("));
  Serial.print(porcentaje, 1);
  Serial.println(F("%)"));

  Serial.println(F("\n========================================"));

  if (totalOK == totalTests) {
    Serial.println(F("     ✓✓✓ TODOS LOS TESTS PASARON ✓✓✓"));
  } else if (porcentaje >= 80) {
    Serial.println(F("     ⚠ ALGUNOS TESTS FALLARON ⚠"));
  } else {
    Serial.println(F("     ✗✗✗ MULTIPLES FALLOS ✗✗✗"));
  }

  Serial.println(F("========================================"));
}
