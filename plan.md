Necesitamos probar todos los actuadores y sensores de una placa de robótica educativa basada en arduino nano. Para ello queremos un programa de test que pruebe secuencialmente todos los sensores y actuadores. 
Vamos a tener dos subprogramas diferenciados que corresponden a 2 modos de funcionamiento de la placa: Modo normal y Modo MkMk

# Mapeo de pines:el mapeo de sensores a pines de arduino es el siguiente:

## Modo normal: 

Pin - Sensor

D10- Zumbador
D11- LED Verde
D12- LED Naranja
D13- LED Rojo
D9, D5, D5- LED RGB
D2 - SR pulsador derecho
D3 - SL pulsador izquierdo
A0 - joyx analogico
A1 - joyy analogico
A4, A5 - I2C acelerómetro
#define LIS3DHTR_ADDR 0x18 // I2C address of LIS3DHTR accelerometer
A3 - LDR analogico
A6 - temperatura analogico
A7 - microfono analogico

## Modo MkMk:

El nombre de los sensores es el mismo que el de los pines y son: A0, A1, A2, A3, A6, A7 D2, D3

# Funcionamiento

El funcionamiento del programa será como sigue:

## Testeo del modo normal:

Utilizamos los pulsadores SR para indicar que los resultados son correctos y pasar al siguiente componente, SL para indicar que el resultado no es correcto y hacer un informe final de errores y éxitos

1- Se avisa por pantalla que se va a proceder a realizar el test del modo normal. Cuando el usuario pulsa SR y después SL comienza el test: Esto sirve de comprobación de los pulsadores.
2- Probamos primero los actuadores.
A- Encender los LEDS
Preguntar si están los 3 encendidos. 
SR Si
SL No
Cuando se contesta se apagan los LEDs
B- Encender 3 colores del LED RGB consecutivamente
Preguntar si se han visto los 3 colores
SR Si
SL No
Cuando se contesta se apagan el LED RGB
C- Suena un pitido intermitente Zumbador
Preguntar si se escucha. Avisar del potenciómetro de ajuste de volumen. 
SR Si
SL No
Cuando se contesta se apaga el zumbador
3- Probamos los sensores

El programa pedirá una actuación por parte del usuario para cada sensor. Si se lee un valor correcto, el programa pasa automáticamente al siguiente sensor. Los valores correctos son mayores o menores a un umbral o igual a un valor que se especificará a continuación. El valor del sensor se mostrará por pantalla con una frecuencia de 0.5 segundos. Si no se alcanza el criterio positivo, el usuario hará clic en SL (No pasa test), y se pasará al siguiente sensor. 

Esta será la secuencia de sensores:

A- Joystick

Se solicita que se mueva el joystick a la izquierda. El test es correcto si la lectura es menor que 5.
Se solicita que se mueva el joystick a la derecha. El test es correcto si la lectura es mayor que 1018.
Se solicita que se mueva el joystick arriba. El test es correcto si la lectura es mayor que 1018.
Se solicita que se mueva el joystick abajo. El test es correcto si la lectura es mayor menor que 5.

B- Acelerómetro

Se solicita que se incline la placa hacia la izquierda. El test es correcto si la lectura es menor que -0.8.
Se solicita que se incline la placa hacia la derecha. El test es correcto si la lectura es mayor que 0.8.
Se solicita que se incline la placa hacia arriba. El test es correcto si la lectura es mayor que 0.8.
Se solicita que se incline la placa hacia abajo. El test es correcto si la lectura es menor que -0.8.

C- LDR

Se pide que se cubra la LDR completamente. El test es correcto si la lectura es menor que 30
D- Temperatura
En este test, como excepción, se pide al usuario si la lectura es correcta (SR) o no (SL)
E- Micrófono
Se pide que se haga ruido. Si se registran valores mayores de 50 se da por correcto.

Los valores umbrales deben de fijarse mediante constantes en el código fuente del programa.

## Testeo del modo MkMk: 

Se avisa por pantalla que se va a proceder a realizar el test del modo MkMk y que el usuario debe cambiar el interruptor de modo MkMk. 
Se avisa tocar MkMk A0 y Hombre MkMk. Si valor de MkMk mayor de 100 se pasa al siguiente entrada. Y así hasta probar todo.
Cuando se han probado todos los sensores. El test termina.

El programa debe dar un reporte de los resultados, mostrando una estadística de test que pasan y no pasan en cada modo.

