#define MAX_ANALOG_VALUE 4095
#define SWITCH_RESISTANCE 1273.0
#define NUM_REF_RESISTORS 8
#define VCC 3.3

float rRef[NUM_REF_RESISTORS] = {47, 100, 1000, 10000, 100000, 1000000, 5000000, 10000000}; // resistors used

const byte rSelPins[3] = {3, 4, 5}; // selection pins
const byte enableMux = 2; //disable all pins
const int measurePin = A7;

void setup() {
  // put your setup code here, to run once:
  analogReadResolution(12);
  pinMode(enableMux, OUTPUT);
  digitalWrite(enableMux, HIGH);

  for (int i = 0; i < 3; i++) { // set each selection pins as OUTPUT
    pinMode(rSelPins[i], OUTPUT);
    digitalWrite(rSelPins[i], HIGH); // select the highest Rref
  }

  Serial.begin(9600);
  Serial.println("Starting ohmmeter...");
}

void loop() {
  int cOut;
  float delta, deltaBest1 = MAX_ANALOG_VALUE, deltaBest2 = MAX_ANALOG_VALUE;
  float rBest1 = -1, rBest2 = -1, rR, rX;
  char unit = 0, fStr[16];
  for (byte count = 0; count < NUM_REF_RESISTORS; count++)
  {
    // Set the Mux select pins to switch in one Rref at a time.
    // count=0: Rref0 (50 ohms), count=1: Rref1 (100 ohms), etc...
    digitalWrite(rSelPins[0], count & 1); // C: least significant bit
    digitalWrite(rSelPins[1], count & 2); // B:
    digitalWrite(rSelPins[2], count & 4); // A: most significant bit
    
    digitalWrite(enableMux, LOW);       // enable the selected reference resistor
    delay(count + 1); 
    //delay(100);                  // delay 1ms for Rref0, 2ms for Ref1, etc...
    cOut = analogRead(measurePin);              // convert analog voltage Vx to a digital value
    digitalWrite(enableMux, HIGH);      // disable the selected reference resistor
    delay(NUM_REF_RESISTORS - count);   // delay 8ms for Rref0, 7ms for Ref1, etc...
    //delay(100);
    // Work only with valid digitized values
    if (cOut < MAX_ANALOG_VALUE)
    {
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
      delta = (MAX_ANALOG_VALUE / 3.0 - cOut);
      if (fabs(delta) < fabs(deltaBest1))
      {
        deltaBest2 = deltaBest1;
        rBest2 = rBest1;
        deltaBest1 = delta;
        rBest1 = rX;
      }
      else if (fabs(deltaBest2) > fabs(delta))
      {
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
  if (rBest1 >= 0 && rBest2 >= 0)
  {
    // Check to see if need to interpolate between the two data points.
    // Refer to the documentation for details regarding this.
    if (deltaBest1 * deltaBest2 < 0)
    {
      rX = rBest1 + deltaBest1 * (rBest2 - rBest1) / (deltaBest1 - deltaBest2); // Yes
      //Serial.println("yes");
    }
    else
    {
      rX = rBest1;  // No. Just use the best value
      //Serial.println("no");
    }
    // Convert the scaled float result to string and extract the units
    //unit = ScaleToMetricUnits(&rX, fStr);
  }
  Serial.println(rX);
  //Serial.println("");
  delay(10);
}

char scaleUnits(float *prVal, char fStr[]) { // calculate the unit, ohm, k, m
  char unit;

  if (*prVal < 1000) {
    unit = ' ';
  }
  else if (*prVal >= 1000 && *prVal < 1000000) {
    *prVal /= 1000;
    unit = 'K';
  }
  else if (*prVal >= 1000000 && *prVal < 1000000000) {
    *prVal /= 1000000;
    unit = 'M';
  }
  else {
    *prVal /= 1000000000;
    unit = 'G';
  }

  for (int k = 2, s = 10; k >= 0; k--, s *= 10) {
    if ((int)(*prVal) / s == 0) {
      //dtostrf(*prVal, 4, k, fStr);
      // Serial.println(snprintf(fStr, 16, "%f", *prVal));
      break;
    }
  }

  return unit;
}
