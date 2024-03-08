#include <CapacitiveSensor.h>

CapacitiveSensor   cs = CapacitiveSensor(4,3);
float progress = 0;
int lowerThreshold = 10;
int higherThreshold = 500;
float cycleTime = higherThreshold;

int LED = 0;

void setup() {
  // put your setup code here, to run once:
  cs.set_CS_AutocaL_Millis(0xFFFFFFFF); 

}

void loop() {
  // put your main code here, to run repeatedly:
  long reading =  cs.capacitiveSensor(30);
  if (reading > 150) {
    if (cycleTime > lowerThreshold + 1) cycleTime -= (cycleTime - lowerThreshold) / (higherThreshold - lowerThreshold);
  }
  else {
    if (cycleTime < higherThreshold - 1) cycleTime += (cycleTime - lowerThreshold) / (higherThreshold - lowerThreshold);
  }
  float degree = PI / cycleTime;
  progress += degree;
  int brightness = (sin(progress) + 1) * 255 / 2;
  // if (cycleTime <= threshold) analogWrite(LED, 255);
  analogWrite(LED, brightness);
}
