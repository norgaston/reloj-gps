// Rejoj GPS

#include <TinyGPS++.h>       // Incluir la biblioteca TinyGPS++
#include <SoftwareSerial.h>  // Incluir la biblioteca de comunicación serial por software
// Incluir la biblioteca para el módulo de display TM1637
#include <TM1637Display.h>

#define S_RX 13  // Definir el pin RX para la comunicación serial por software
#define S_TX 12  // Definir el pin TX para la comunicación serial por software
// Definir los pines de conexión para el módulo TM1637
#define CLK 3  // Pin de reloj
#define DIO 2  // Pin de datos

TinyGPSPlus gps;
// Configurar la comunicación serial por software
SoftwareSerial gpsSerial(S_RX, S_TX);
// Inicializar la biblioteca para el módulo de display TM1637
TM1637Display display(CLK, DIO);

const int TIME_ZONE_OFFSET = -3;  // Ajuste para la zona horaria local

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  display.setBrightness(7);  // Ajustar el brillo del display (0 a 7)
}

void loop() {
  if (gpsSerial.available() > 0) {
    while (gpsSerial.available() > 0) {
      // Leer y procesar los datos del GPS
      if (gps.encode(gpsSerial.read())) {
        // Verificar si los datos del GPS son válidos
        if (gps.time.isValid()) {
          // Obtener la hora UTC del módulo GPS
          int utcHour = gps.time.hour();
          int utcMinute = gps.time.minute();

          // Calcular la hora local
          int localHour = (utcHour + TIME_ZONE_OFFSET + 24) % 24;

          // Concatenar horas y minutos en una variable entera
          int timeValue = localHour * 100 + gps.time.minute();

          // Imprimir la hora local en el puerto serie con dos dígitos cada uno
          Serial.print(localHour < 10 ? "0" : "");
          Serial.print(localHour);
          Serial.print(':');
          Serial.print(gps.time.minute() < 10 ? "0" : "");
          Serial.println(gps.time.minute());

          // Mostrar la hora completa en el display
          display.showNumberDecEx(timeValue, 0b11110000, true);
        } else {
          // Mostrar "Err" en el display si los datos del GPS no son válidos
          display.showNumberDecEx(0b01111001, 0b1000, true);   // E
          display.showNumberDecEx(0b01001111, 0b1000, false);  // r
          display.showNumberDecEx(0b01110011, 0b1000, false);  // r
        }
      }
    }
  }
}
