# Test FAB Echidna

Programa de test automatizado para placas de robótica educativa FAB Echidna basadas en Arduino Nano. Permite verificar el funcionamiento de todos los actuadores y sensores de la placa en dos modos: Normal y MkMk.

## Descripción

Este programa realiza pruebas secuenciales de todos los componentes de la placa FAB Echidna:
- Actuadores: LEDs (Verde, Naranja, Rojo), LED RGB, Zumbador
- Sensores: Joystick, Acelerómetro, LDR, Temperatura, Micrófono
- Modo MkMk: Verificación de conectividad de pines para conexiones externas

El programa guía al usuario a través de cada test mostrando instrucciones por el Monitor Serial y proporciona un reporte final con estadísticas de éxitos y fallos.

## Requisitos

### Hardware
- Placa FAB Echidna con Arduino Nano
- Cable USB para programación y monitor serial
- Computadora con Arduino IDE

### Software
- Arduino IDE 1.8.x o superior (o Arduino IDE 2.x)
- Librería **Adafruit LIS3DH** para el acelerómetro
- Librería **Adafruit Unified Sensor** (dependencia automática)

## Instalación

### 1. Instalar Arduino IDE
Si no lo tienes instalado, descarga Arduino IDE desde [arduino.cc](https://www.arduino.cc/en/software)

### 2. Instalar las librerías necesarias

**Opción A: Desde el Gestor de Librerías (recomendado)**
1. Abre Arduino IDE
2. Ve a `Herramientas > Administrar Bibliotecas...` (o `Tools > Manage Libraries...`)
3. Busca "Adafruit LIS3DH" en el buscador
4. Instala la librería **Adafruit LIS3DH** de Adafruit
5. Cuando te pregunte si deseas instalar las dependencias, haz clic en "Instalar todas"
   - Esto instalará automáticamente **Adafruit Unified Sensor** y otras dependencias necesarias

**Opción B: Manual**
1. Descarga las librerías desde GitHub:
   - [Adafruit LIS3DH](https://github.com/adafruit/Adafruit_LIS3DH)
   - [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor)
2. En Arduino IDE ve a `Programa > Incluir Librería > Añadir biblioteca .ZIP`
3. Instala primero Adafruit Unified Sensor, luego Adafruit LIS3DH

### 3. Cargar el programa

1. Abre el archivo `test-fab-echidna.ino` en Arduino IDE
2. Conecta la placa FAB Echidna al computador vía USB
3. Selecciona la placa: `Herramientas > Placa > Arduino Nano`
4. Selecciona el procesador: `Herramientas > Procesador > ATmega328P` (o ATmega328P Old Bootloader si falla)
5. Selecciona el puerto correcto: `Herramientas > Puerto > [Puerto de tu Arduino]`
6. Haz clic en el botón "Subir" (flecha derecha) o presiona `Ctrl+U`

## Uso

### Preparación
1. Una vez cargado el programa, abre el Monitor Serial: `Herramientas > Monitor Serie`
2. Configura la velocidad a **9600 baudios** en la esquina inferior derecha
3. Asegúrate de tener la placa en un espacio donde puedas:
   - Ver los LEDs claramente
   - Escuchar el zumbador (ajusta el potenciómetro de volumen si es necesario)
   - Mover el joystick
   - Inclinar la placa
   - Cubrir el sensor LDR
   - Hacer ruido cerca del micrófono

### Secuencia de Test

#### 1. Test Modo Normal

El programa comenzará mostrando instrucciones. Deberás:

**A. Verificación de pulsadores**
- Presiona el pulsador **SR** (derecho)
- Presiona el pulsador **SL** (izquierdo)
- Esto confirma que los pulsadores funcionan

**B. Test de Actuadores**

1. **LEDs**: Se encienden los 3 LEDs (Verde, Naranja, Rojo)
   - Presiona **SR** si los ves encendidos
   - Presiona **SL** si alguno no funciona

2. **LED RGB**: Muestra 3 colores secuencialmente (Rojo, Verde, Azul)
   - Presiona **SR** si viste los 3 colores
   - Presiona **SL** si alguno faltó

3. **Zumbador**: Emite pitidos intermitentes
   - Ajusta el volumen con el potenciómetro si es necesario
   - Presiona **SR** si lo escuchas
   - Presiona **SL** si no funciona

**C. Test de Sensores**

Los sensores se prueban automáticamente. El programa mostrará el valor leído en tiempo real:

1. **Joystick**
   - Mueve a la izquierda (hasta que pase automáticamente)
   - Mueve a la derecha (hasta que pase automáticamente)
   - Mueve arriba (hasta que pase automáticamente)
   - Mueve abajo (hasta que pase automáticamente)
   - Si un movimiento no funciona, presiona **SL** para saltar

2. **Acelerómetro**
   - Inclina la placa a la izquierda
   - Inclina la placa a la derecha
   - Inclina la placa arriba
   - Inclina la placa abajo
   - El test pasa automáticamente cuando detecta la inclinación correcta
   - Presiona **SL** para saltar si no funciona

3. **LDR (Sensor de luz)**
   - Cubre completamente el sensor LDR con tu mano o un objeto opaco
   - El test pasa automáticamente cuando detecta oscuridad
   - Presiona **SL** para saltar si no funciona

4. **Temperatura**
   - El programa muestra la lectura del sensor
   - Verifica si el valor es razonable (temperatura ambiente ~20-25°C)
   - Presiona **SR** si es correcto, **SL** si no lo es

5. **Micrófono**
   - Haz ruido cerca del micrófono (habla, aplaude, silba)
   - El test pasa automáticamente cuando detecta sonido
   - Presiona **SL** para saltar si no funciona

#### 2. Test Modo MkMk

1. El programa te pedirá que cambies el **interruptor a Modo MkMk**
2. Presiona **SR** cuando esté listo
3. Para cada pin (A0, A1, A2, A3, A6, A7, D2, D3):
   - Toca simultáneamente el pin MkMk correspondiente y el pin "Hombre MkMk"
   - El test pasa automáticamente cuando detecta la conexión
   - Si un pin no pasa después de 30 segundos, el programa continúa automáticamente

#### 3. Reporte Final

Al finalizar todos los tests, el programa muestra:
- Número de actuadores OK/FAIL en Modo Normal
- Número de sensores OK/FAIL en Modo Normal
- Número de pines OK/FAIL en Modo MkMk
- Porcentaje total de éxito
- Indicador visual del resultado global

Para reiniciar el test, presiona el botón **RESET** en el Arduino.

## Mapeo de Pines

### Modo Normal

| Pin | Componente |
|-----|------------|
| D10 | Zumbador |
| D11 | LED Verde |
| D12 | LED Naranja |
| D13 | LED Rojo |
| D9  | LED RGB - Rojo |
| D6  | LED RGB - Verde |
| D5  | LED RGB - Azul |
| D2  | Pulsador SR (derecho) |
| D3  | Pulsador SL (izquierdo) |
| A0  | Joystick X |
| A1  | Joystick Y |
| A4  | I2C SDA - Acelerómetro |
| A5  | I2C SCL - Acelerómetro |
| A3  | LDR (Sensor de luz) |
| A6  | Sensor de Temperatura |
| A7  | Micrófono |

### Modo MkMk

Pines testeables: A0, A1, A2, A3, A6, A7, D2, D3

## Configuración y Umbrales

Los siguientes umbrales pueden ajustarse al inicio del archivo `.ino`:

```cpp
// Joystick
#define JOY_THRESHOLD_LOW 5        // Umbral para izquierda/abajo
#define JOY_THRESHOLD_HIGH 1018    // Umbral para derecha/arriba

// Acelerómetro (valores en m/s²)
#define ACCEL_THRESHOLD_LOW -7.8   // Umbral negativo (~0.8g)
#define ACCEL_THRESHOLD_HIGH 7.8   // Umbral positivo (~0.8g)

// LDR
#define LDR_THRESHOLD_DARK 30      // Valor para considerar oscuro

// Micrófono
#define MIC_THRESHOLD_NOISE 50     // Amplitud mínima para detectar ruido

// MkMk
#define MKMK_THRESHOLD 100         // Valor mínimo analógico para conexión

// Tiempo de actualización (ms)
#define SENSOR_READ_DELAY 500      // Frecuencia de lectura de sensores
```

## Solución de Problemas

### El programa no compila
- **Error: Adafruit_LIS3DH.h not found** o **Adafruit_Sensor.h not found**
  - Solución: Instala las librerías Adafruit LIS3DH y Adafruit Unified Sensor desde el gestor de librerías
  - Asegúrate de instalar las dependencias cuando te lo pregunte Arduino IDE

- **Error al cargar**
  - Solución: Prueba con el procesador "ATmega328P (Old Bootloader)" en lugar de "ATmega328P"

### El Monitor Serial no muestra nada
- Verifica que la velocidad esté en **9600 baudios**
- Presiona el botón RESET en el Arduino

### El acelerómetro no funciona
- Mensaje: "ERROR: No se pudo inicializar el acelerometro"
- Verifica las conexiones I2C (A4/SDA, A5/SCL)
- Comprueba que la dirección I2C sea 0x18

### Un sensor siempre falla
- Ajusta los umbrales en el código fuente
- Verifica las conexiones del sensor
- Comprueba que el sensor no esté dañado

### Los tests se saltan automáticamente
- Cada test tiene un timeout de 30 segundos
- Si no se alcanza el umbral, se considera fallo automático
- Puedes saltar manualmente con el pulsador SL

### El LED RGB no muestra los colores correctos
- Verifica el mapeo de pines RGB (D9=R, D6=G, D5=B)
- Si los colores están invertidos, ajusta los pines en el código

## Estructura del Código

```
test-fab-echidna.ino
├── Definiciones de pines
├── Umbrales configurables
├── Variables globales
├── setup()
│   ├── Inicialización serial
│   ├── Configuración de pines
│   └── Inicialización acelerómetro
├── loop()
│   ├── testModoNormal()
│   ├── testModoMkMk()
│   └── reporteFinal()
├── Funciones de utilidad
│   ├── apagarActuadores()
│   ├── esperarBotonSR()
│   ├── esperarBotonSL()
│   └── preguntarSiNo()
├── Tests de actuadores
│   ├── testLEDs()
│   ├── testLEDRGB()
│   └── testZumbador()
├── Tests de sensores
│   ├── testJoystick()
│   ├── testAcelerometro()
│   ├── testLDR()
│   ├── testTemperatura()
│   └── testMicrofono()
└── testModoMkMk()
```

## Notas Técnicas

- **Timeout por test**: 30 segundos para evitar bloqueos
- **Debounce de pulsadores**: 200ms
- **Dirección I2C del acelerómetro**: 0x18
- **Acelerómetro**: Usa librería Adafruit LIS3DH con valores en m/s² (1g ≈ 9.8 m/s²)
- **Frecuencia de actualización**: 500ms (configurable)
- **Pull-up en pulsadores**: Activado internamente
- **Conversión de temperatura**: Asume sensor tipo TMP36

## Autor

Programa desarrollado para placas educativas FAB Echidna basadas en Arduino Nano.

## Licencia

Este código es de uso libre para propósitos educativos.
