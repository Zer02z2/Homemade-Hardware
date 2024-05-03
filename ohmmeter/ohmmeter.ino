#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(SCL, SDA, U8X8_PIN_NONE);

#define MAX_ANALOG_VALUE 1023
#define SWITCH_RESISTANCE 8
#define NUM_REF_RESISTORS 8
#define VCC 3.3

float rRef[NUM_REF_RESISTORS] = { 47, 100, 1000, 10000, 100000, 1000000, 5000000, 10000000 };  // resistors used

const byte rSelPins[3] = { 8, 9, 10 };  // selection pins
const byte enableMux = 7;               //disable all pins
const int measurePin = A0;

void setup() {
  // put your setup code here, to run once:
  pinMode(enableMux, OUTPUT);
  digitalWrite(enableMux, HIGH);

  u8x8.begin();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_u);

  for (int i = 0; i < 3; i++) {  // set each selection pins as OUTPUT
    pinMode(rSelPins[i], OUTPUT);
    digitalWrite(rSelPins[i], HIGH);  // select the highest Rref
  }

  // Serial.begin(9600);
  // Serial.println("Starting ohmmeter...");
}

void loop() {
  int cOut;
  float delta, deltaBest1 = MAX_ANALOG_VALUE, deltaBest2 = MAX_ANALOG_VALUE;
  float rBest1 = -1, rBest2 = -1, rR, rX;
  char unit = ' ';
  for (byte count = 0; count < NUM_REF_RESISTORS; count++) {
    // Set the Mux select pins to switch in one Rref at a time.
    // count=0: Rref0 (50 ohms), count=1: Rref1 (100 ohms), etc...
    digitalWrite(rSelPins[0], count & 1);  // C: least significant bit
    digitalWrite(rSelPins[1], count & 2);  // B:
    digitalWrite(rSelPins[2], count & 4);  // A: most significant bit

    digitalWrite(enableMux, LOW);      // enable the selected reference resistor
    delay(count + 1);                  // delay 1ms for Rref0, 2ms for Ref1, etc...
    cOut = analogRead(measurePin);     // convert analog voltage Vx to a digital value
    digitalWrite(enableMux, HIGH);     // disable the selected reference resistor
    delay(NUM_REF_RESISTORS - count);  // delay 8ms for Rref0, 7ms for Ref1, etc...
    // Work only with valid digitized values
    if (cOut < MAX_ANALOG_VALUE) {
      // Identify the Rref value being used and compute Rx based on formula #2.
      // Note how Mux's internal switch resistance is added to Rref.
      rR = rRef[count] + SWITCH_RESISTANCE;
      float Vx = cOut * (VCC / MAX_ANALOG_VALUE);
      // float buffer = (VCC - Vx) / rR;
      // rX = Vx / buffer;
      rX = (rR * cOut) / (MAX_ANALOG_VALUE - cOut);
      // Serial.print("Vx: ");
      // Serial.print(Vx);
      // Serial.print(" rX: ");
      // Serial.print(rX);
      // Compute the delta and track the top two best delta and Rx values
      delta = (MAX_ANALOG_VALUE / 2.0 - cOut);
      if (fabs(delta) < fabs(deltaBest1)) {
        deltaBest2 = deltaBest1;
        rBest2 = rBest1;
        deltaBest1 = delta;
        rBest1 = rX;
      } else if (fabs(deltaBest2) > fabs(delta)) {
        deltaBest2 = delta;
        rBest2 = rX;
      }
      // Serial.print(" rBest1: ");
      // Serial.print(rBest1);
      // Serial.print(" rBest2: ");
      // Serial.print(rBest2);

      // Serial.print(" deltaBest1: ");
      // Serial.print(deltaBest1);
      // Serial.print(" deltaBest2: ");
      // Serial.println(deltaBest2);
    }
  }
  // Make sure there are at least two good samples to work with
  if (rBest1 >= 0 && rBest2 >= 0) {
    // Check to see if need to interpolate between the two data points.
    // Refer to the documentation for details regarding this.
    if (deltaBest1 * deltaBest2 < 0) {
      rX = rBest1 - deltaBest1 * (rBest2 - rBest1) / (deltaBest2 - deltaBest1);  // Yes
      //Serial.println("yes");
    } else {
      rX = rBest1;  // No. Just use the best value
      //Serial.println("no");
    }
    // Convert the scaled float result to string and extract the units
    //unit = ScaleToMetricUnits(&rX, fStr);
  }

  if (rX < 1000) {
    unit = ' ';
  } else if (rX >= 1000 && rX < 1000000) {
    rX /= 1000;
    unit = 'K';
  } else if (rX >= 1000000 && rX < 1000000000) {
    rX /= 1000000;
    unit = 'M';
  } else {
    rX /= 1000000000;
    unit = 'G';
  }

  //rX = ScaleToMetricUnits(&rX, fStr);

  //Serial.println(fStr);

  u8x8.setCursor(1, 1);
  u8x8.print(rX);
  u8x8.setCursor(1, 4);
  u8x8.print(unit);
  u8x8.setCursor(3, 4);
  u8x8.print("OHM");

  //Serial.println(rX);
  //Serial.println("");
  delay(100);
}

// float ScaleToMetricUnits(float rX, char fStr[]) {  // calculate the unit, ohm, k, m
//   char unit[1];

//   if (rX < 1000) {
//     unit[0] = ' ';
//   } else if (rX >= 1000 && rX < 1000000) {
//     rX /= 1000;
//     unit[0] = 'K';
//   } else if (rX >= 1000000 && rX < 1000000000) {
//     rX /= 1000000;
//     unit[0] = 'M';
//   } else {
//     rX /= 1000000000;
//     unit[0] = 'G';
//   }

//   fStr[0] = unit[0];

//   return rX;

//   //sprintf(sz, "%d.%d", val_int, val_fra);
//   //dtostrf(rX, 4, k, fStr);
//   // Serial.println(snprintf(fStr, 16, "%f", rX));
//   // String stringVal = "";

//   // stringVal += String(int(rX)) + "." + String(getDecimal(rX));

//   // stringVal.toCharArray(fStr, stringVal.length() + 1);

//   // strcat(fStr, unit);
//   // char ohm[4] = "Ohm";
//   // strcat(fStr, ohm);
// }

// long getDecimal(float val) {
//   int intPart = int(val);
//   long decPart = 1000 * (val - intPart);          //I am multiplying by 1000 assuming that the foat values will have a maximum of 3 decimal places.
//                                                   //Change to match the number of decimal places you need
//   if (decPart > 0) return (decPart);              //return the decimal part of float number if it is available
//   else if (decPart < 0) return ((-1) * decPart);  //if negative, multiply by -1
//   else if (decPart = 0) return (00);              //return 0 if decimal part of float number is not available
// }
