# Test FAB Echidna

Automated test program for FAB Echidna educational robotics boards based on Arduino Nano. It allows verification of all board actuators and sensors in two modes: Normal and MkMk.

## Description

This program performs sequential tests of all FAB Echidna board components:
- Actuators: LEDs (Green, Orange, Red), RGB LED, Buzzer
- Sensors: Joystick, Accelerometer, LDR, Temperature, Microphone, IO Pins
- MkMk Mode: Pin connectivity verification for external connections

The program guides the user through each test by displaying instructions via the Serial Monitor and provides a final report with success and failure statistics.

## Requirements

### Hardware
- FAB Echidna board with Arduino Nano
- USB cable for programming and serial monitor
- Computer with Arduino IDE

### Software
- Arduino IDE 1.8.x or higher (or Arduino IDE 2.x)
- **Adafruit LIS3DH** library for the accelerometer
- **Adafruit Unified Sensor** library (automatic dependency)

## Installation

### 1. Install Arduino IDE
If you don't have it installed, download Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software)

### 2. Install the required libraries

**Option A: From the Library Manager (recommended)**
1. Open Arduino IDE
2. Go to `Tools > Manage Libraries...`
3. Search for "Adafruit LIS3DH" in the search box
4. Install the **Adafruit LIS3DH** library from Adafruit
5. When prompted to install dependencies, click "Install all"
   - This will automatically install **Adafruit Unified Sensor** and other necessary dependencies

**Option B: Manual**
1. Download the libraries from GitHub:
   - [Adafruit LIS3DH](https://github.com/adafruit/Adafruit_LIS3DH)
   - [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor)
2. In Arduino IDE go to `Sketch > Include Library > Add .ZIP Library`
3. Install Adafruit Unified Sensor first, then Adafruit LIS3DH

### 3. Upload the program

1. Open the `test-fab-echidna.ino` file in Arduino IDE
2. Connect the FAB Echidna board to the computer via USB
3. Select the board: `Tools > Board > Arduino Nano`
4. Select the processor: `Tools > Processor > ATmega328P` (or ATmega328P Old Bootloader if it fails)
5. Select the correct port: `Tools > Port > [Your Arduino Port]`
6. Click the "Upload" button (right arrow) or press `Ctrl+U`

## Usage

### Preparation
1. Once the program is uploaded, open the Serial Monitor: `Tools > Serial Monitor`
2. Set the speed to **9600 baud** in the lower right corner
3. Have jumper wires or a wire ready for testing the IO pins
4. Make sure you have the board in a space where you can:
   - See the LEDs clearly
   - Hear the buzzer (adjust the volume potentiometer if necessary)
   - Move the joystick
   - Tilt the board
   - Cover the LDR sensor
   - Make noise near the microphone
   - Access pins A2, D4, D7, D8 and GND for the IO pins test

### Test Sequence

#### 1. Normal Mode Test

The program will start by showing instructions. You must:

**A. Button Verification**
- Press the **SR** button (right)
- Press the **SL** button (left)
- This confirms that the buttons work

**B. Actuator Tests**

1. **LEDs**: The 3 LEDs turn on (Green, Orange, Red)
   - Press **SR** if you see them lit
   - Press **SL** if any don't work

2. **RGB LED**: Shows 3 colors sequentially (Red, Green, Blue)
   - Press **SR** if you saw all 3 colors
   - Press **SL** if any were missing

3. **Buzzer**: Emits intermittent beeps
   - Adjust the volume with the potentiometer if necessary
   - Press **SR** if you hear it
   - Press **SL** if it doesn't work

**C. Sensor Tests**

Sensors are tested automatically. The program will show the value read in real time:

1. **Joystick**
   - Move left (until it passes automatically)
   - Move right (until it passes automatically)
   - Move up (until it passes automatically)
   - Move down (until it passes automatically)
   - If a movement doesn't work, press **SL** to skip

2. **Accelerometer**
   - Tilt the board to the left
   - Tilt the board to the right
   - Tilt the board up
   - Tilt the board down
   - Place the board face up (components facing up)
   - Place the board face down (components facing down)
   - The test passes automatically when it detects the correct tilt
   - Press **SL** to skip if it doesn't work

3. **LDR (Light sensor)**
   - Completely cover the LDR sensor with your hand or an opaque object
   - The test passes automatically when it detects darkness
   - Press **SL** to skip if it doesn't work

4. **Temperature**
   - The program displays the sensor reading
   - Verify if the value is reasonable (room temperature ~20-25°C)
   - Press **SR** if correct, **SL** if not

5. **Microphone**
   - Make noise near the microphone (talk, clap, whistle)
   - The test passes automatically when it detects sound
   - Press **SL** to skip if it doesn't work

6. **IO Pins (A2, D4, D7, D8)**
   - For each pin, the program will ask you to short the pin to GND
   - Connect a wire or jumper between the specified pin and any GND pin on the board
   - The test passes automatically when it detects the short circuit (pin reads LOW)
   - Remove the short circuit before moving to the next pin
   - Press **SL** to skip if a pin doesn't work

#### 2. MkMk Mode Test

1. The program will ask you to switch the **switch to MkMk Mode**
2. Press **SR** when ready
3. For each pin (A0, A1, A2, A3, A6, A7, D2, D3):
   - Simultaneously touch the corresponding MkMk pin and the "MkMk Man" pin
   - The test passes automatically when it detects the connection
   - Each pin has **10 attempts** (configurable with `MKMK_MAX_ATTEMPTS`)
   - If a pin doesn't pass after the attempts, the program continues automatically

#### 3. Final Report

After completing all tests, the program displays:
- Number of actuators OK/FAIL in Normal Mode
  - **Detailed list of actuators that failed** (if there are failures)
- Number of sensors OK/FAIL in Normal Mode
  - **Detailed list of sensors that failed** (if there are failures)
- Number of pins OK/FAIL in MkMk Mode
  - **Detailed list of pins that failed** (if there are failures)
- Total success percentage
- Visual indicator of the overall result

To restart the test, press the **RESET** button on the Arduino.

## Pin Mapping

### Normal Mode

| Pin | Component |
|-----|-----------|
| D10 | Buzzer |
| D11 | Green LED |
| D12 | Orange LED |
| D13 | Red LED |
| D9  | RGB LED - Red |
| D6  | RGB LED - Green |
| D5  | RGB LED - Blue |
| D2  | SR Button (right) |
| D3  | SL Button (left) |
| A0  | Joystick X |
| A1  | Joystick Y |
| A4  | I2C SDA - Accelerometer |
| A5  | I2C SCL - Accelerometer |
| A3  | LDR (Light sensor) |
| A6  | Temperature Sensor |
| A7  | Microphone |
| A2  | IO Pin (general purpose input/output) |
| D4  | IO Pin (general purpose input/output) |
| D7  | IO Pin (general purpose input/output) |
| D8  | IO Pin (general purpose input/output) |

### MkMk Mode

Testable pins: A0, A1, A2, A3, A6, A7, D2, D3

## Configuration and Thresholds

The following thresholds and parameters can be adjusted at the beginning of the `.ino` file:

```cpp
// Joystick
#define JOY_THRESHOLD_LOW 5        // Threshold for left/down
#define JOY_THRESHOLD_HIGH 1018    // Threshold for right/up

// Accelerometer (values in m/s²)
#define ACCEL_THRESHOLD_LOW -7.8   // Negative threshold (~0.8g)
#define ACCEL_THRESHOLD_HIGH 7.8   // Positive threshold (~0.8g)

// LDR
#define LDR_THRESHOLD_DARK 30      // Value to consider dark

// Microphone
#define MIC_THRESHOLD_NOISE 50     // Minimum amplitude to detect noise

// MkMk
#define MKMK_THRESHOLD 100         // Minimum analog value for connection
#define MKMK_MAX_ATTEMPTS 10       // Number of attempts per pin in MkMk mode

// Update time (ms)
#define SENSOR_READ_DELAY 500      // Sensor reading frequency
```

### Notes on constants

- **JOY_THRESHOLD_LOW/HIGH**: Joystick ADC values (0-1023) to detect extreme movements
- **ACCEL_THRESHOLD_LOW/HIGH**: Thresholds in m/s² to detect tilt. Values of ±7.8 correspond to ~0.8g
- **LDR_THRESHOLD_DARK**: ADC value below which it's considered dark
- **MIC_THRESHOLD_NOISE**: Minimum amplitude (deviation from midpoint 512) to detect sound
- **MKMK_THRESHOLD**: Minimum ADC value to consider there's a connection in MkMk mode
- **MKMK_MAX_ATTEMPTS**: Controls the number of attempts for each pin in MkMk mode. Centralizes this value to maintain consistency in all messages
- **SENSOR_READ_DELAY**: Time in ms between successive sensor readings

## Troubleshooting

### The program doesn't compile
- **Error: Adafruit_LIS3DH.h not found** or **Adafruit_Sensor.h not found**
  - Solution: Install the Adafruit LIS3DH and Adafruit Unified Sensor libraries from the library manager
  - Make sure to install the dependencies when Arduino IDE asks you

- **Upload error**
  - Solution: Try the "ATmega328P (Old Bootloader)" processor instead of "ATmega328P"

### The Serial Monitor shows nothing
- Verify that the speed is set to **9600 baud**
- Press the RESET button on the Arduino

### The accelerometer doesn't work
- Message: "ERROR: Could not initialize accelerometer"
- Verify the I2C connections (A4/SDA, A5/SCL)
- Check that the I2C address is 0x18

### A sensor always fails
- Adjust the thresholds in the source code
- Verify the sensor connections
- Check that the sensor is not damaged

### Tests skip automatically
- Each test has a 30-second timeout
- If the threshold is not reached, it's considered an automatic failure
- You can skip manually with the SL button

### The RGB LED doesn't show the correct colors
- Verify the RGB pin mapping (D9=R, D6=G, D5=B)
- If the colors are inverted, adjust the pins in the code

## Code Structure

```
test-fab-echidna.ino
├── Pin definitions
├── Configurable thresholds
├── Global variables
├── setup()
│   ├── Serial initialization
│   ├── Pin configuration
│   └── Accelerometer initialization
├── loop()
│   ├── testNormalMode()
│   ├── testMkMkMode()
│   └── finalReport()
├── Utility functions
│   ├── turnOffActuators()
│   ├── waitForButtonSR()
│   ├── waitForButtonSL()
│   └── askYesNo()
├── Actuator tests
│   ├── testLEDs()
│   ├── testLEDRGB()
│   └── testBuzzer()
├── Sensor tests
│   ├── testJoystick()
│   ├── testAccelerometer()
│   ├── testLDR()
│   ├── testTemperature()
│   ├── testMicrophone()
│   └── testIOPins()
└── testMkMkMode()
```

## Technical Notes

- **Timeout per test**: 30 seconds to avoid blocking
- **Attempts in MkMk mode**: 10 attempts per pin (configurable with `MKMK_MAX_ATTEMPTS`)
- **Button debounce**: 200ms
- **Accelerometer I2C address**: 0x18
- **Accelerometer**: Uses Adafruit LIS3DH library with values in m/s² (1g ≈ 9.8 m/s²)
  - **Accelerometer tests**: 6 tests (left, right, up, down, face up, face down)
  - Face up/down tests verify the Z axis by measuring gravity (±9.8 m/s²)
- **Update frequency**: 500ms (configurable with `SENSOR_READ_DELAY`)
- **Pull-up on buttons**: Internally activated
- **IO Pins test**: Tests 4 general purpose pins (A2, D4, D7, D8) as digital inputs
  - Pins configured with internal pull-up resistors (normally read HIGH)
  - Test detects short circuit to GND (pin reads LOW)
  - Each pin tested sequentially with 30-second timeout
  - Allows manual failure indication with SL button
- **Temperature conversion**: Formula `(ADCvalue * 0.4658) - 50.0` to get degrees Celsius
- **Failure report**: The final report includes a detailed list of which specific tests failed

## Author

Program developed for FAB Echidna educational boards based on Arduino Nano.

## License

This code is free to use for educational purposes.
