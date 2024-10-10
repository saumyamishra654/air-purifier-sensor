/**
 * Example for using GP2Y1010AU0F Dust Sensor library
 */

#include <GP2Y1010AU0F.h>

int measurePin = 26;   // Connect dust sensor analog measure pin to ESP32 pin 26
int ledPin     = 27;    // Connect dust sensor LED pin to ESP32 pin 27

GP2Y1010AU0F dustSensor(ledPin, measurePin); // Construct dust sensor global object
float dustDensity = 0;

void setup() {
  Serial.begin(115200); // Initialize serial communication at 115200 baud
  Serial.println(F("GP2Y1010AU0F Dust Sensor Library Example"));

  dustSensor.begin(); // Initialize the dust sensor
}

void loop() {
  dustDensity = dustSensor.read(); 
  
  Serial.print("Dust Density = ");
  Serial.print(dustDensity);
  Serial.println(" ug/m3");

  delay(5000); 
}
