#include <CapacitiveSensor.h>

/*
 * CapitiveSense Library Demo Sketch
 * Paul Badger 2008
 * Uses a high value resistor e.g. 10M between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 */


CapacitiveSensor   cs_4_2 = CapacitiveSensor(1,2);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   cs_4_6 = CapacitiveSensor(3, 4);        // 10M resistor between pins 4 & 6, pin 6 is sensor pin, add a wire and or foil       // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil
int ledPin = 0;

long shortLastBlink = 0;
long longLastBlink = 0;

int shortInterval = 500;
int longInterval = 1000;

void setup()                    
{

  pinMode(ledPin, OUTPUT);
  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  //Serial.begin(9600);
}

void loop()                    
{
    // long start = millis();
    long total1 =  cs_4_2.capacitiveSensor(30);
    long total2 =  cs_4_6.capacitiveSensor(30);

    if (total1 > 150 && total2 > 150) {

      digitalWrite(ledPin, HIGH);
    }
    else if (total1 > 150 && total2 < 150) {

      if (millis() - shortLastBlink > shortInterval) {
        digitalWrite(ledPin, HIGH);
      }
      if (millis() - shortLastBlink > 2 * shortInterval) {
        digitalWrite(ledPin, LOW);
        shortLastBlink = millis();
      }
    }
    else if (total1 < 150 && total2 > 150) {

      if (millis() - longLastBlink > longInterval) {
        digitalWrite(ledPin, HIGH);
      }
      if (millis() - longLastBlink > 2 * longInterval) {
        digitalWrite(ledPin, LOW);
        longLastBlink = millis();
      }
    }
    else {
      digitalWrite(ledPin, LOW);
    }

    delay(10);                             // arbitrary delay to limit data to serial port 
}
