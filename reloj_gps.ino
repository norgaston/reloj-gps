// Reloj GPS

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <TM1637Display.h>

// Definir los pines y bibliotecas necesarios
#define GPS_RX 13  // Pin RX para la comunicación serial con el módulo GPS
#define GPS_TX 12  // Pin TX para la comunicación serial con el módulo GPS
#define CLK 3      // Pin de reloj para el módulo TM1637
#define DIO 2      // Pin de datos para el módulo TM1637

TinyGPSPlus gps;                           // Crear una instancia de la clase TinyGPSPlus para manejar datos GPS
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);  // Configurar comunicación serial con el módulo GPS
TM1637Display display(CLK, DIO);           // Inicializar el módulo TM1637 para el display

const int TIME_ZONE_OFFSET = -3;  // Ajuste para la zona horaria local
bool displayBlinkState = false;   // Variable para controlar el estado de parpadeo del display

void setup() {
  Serial.begin(9600);        // Inicializar comunicación serial con el monitor serial
  gpsSerial.begin(9600);     // Inicializar comunicación serial con el módulo GPS
  display.setBrightness(7);  // Ajustar el brillo del display (0 a 7)
}

void loop() {
  if (gpsSerial.available() > 0) {
    while (gpsSerial.available() > 0) {
      if (gps.encode(gpsSerial.read())) {
        if (gps.time.isValid()) {
          int utcHour = gps.time.hour();
          int utcMinute = gps.time.minute();
          int utcSecond = gps.time.second();

          // Calcular el tiempo local ajustado por la zona horaria
          int localHour = (utcHour + TIME_ZONE_OFFSET + 24) % 24;
          int timeValue = localHour * 100 + gps.time.minute();

          // Imprimir la hora local en el puerto serie (se envían los segundos)
          Serial.print(localHour < 10 ? "0" : "");
          Serial.print(localHour);
          Serial.print(':');
          Serial.print(gps.time.minute() < 10 ? "0" : "");
          Serial.print(gps.time.minute());
          Serial.print(':');
          Serial.print(gps.time.second() < 10 ? "0" : "");
          Serial.println(gps.time.second());

          // Mostrar la hora completa en el display y controlar el parpadeo
          display.showNumberDecEx(timeValue, displayBlinkState ? 0b11110000 : 0, true);
          displayBlinkState = !displayBlinkState;  // Cambiar el estado de parpadeo cada segundo
        } else {
          // Mostrar mensaje de error en el display si los datos del GPS no son válidos
          display.showNumberDecEx(0b01111001, 0b1000, true);   // E
          display.showNumberDecEx(0b01001111, 0b1000, false);  // r
          display.showNumberDecEx(0b01110011, 0b1000, false);  // r
        }
      }
    }
    // Limpiar el buffer de recepción para evitar procesar la misma trama varias veces
    gpsSerial.flush();
  }
}
