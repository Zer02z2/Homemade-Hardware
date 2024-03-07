int cycleTime = 1000;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  float degree = PI / cycleTime;
  int brightness = (sin(degree * millis()) + 1) * 255 / 2;
  analogWrite(0, brightness);
}
