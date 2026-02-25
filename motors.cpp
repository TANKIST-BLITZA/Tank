#include <Arduino.h>

#define JOYSTICK A0
#define LMLD 6
#define LMRD 5


void setup() {
  Serial.begin(9600);
  delay(1000);
  pinMode(JOYSTICK, INPUT);
  // pinMode(LMLD, OUTPUT);
  // pinMode(LMRD, OUTPUT);
}

void loop() {
  static int16_t power;

  power = analogRead(JOYSTICK);
  // Serial.println(power);
  static int dir;

  if (power >= 480 && power <= 522) {
    dir = 0;
    analogWrite(LMLD, 0);
    analogWrite(LMRD, 0);
  }
  if (power < 480) {
    dir = map(power, 490, 0, 0, 255);
    analogWrite(LMLD, dir);
    digitalWrite(LMRD, 0);
  }
  if (power > 522) {
    dir = map(power, 522, 1023, 0, 255);
    analogWrite(LMRD, dir);
    digitalWrite(LMLD, 0);
  }
  Serial.println("Dir: " + (String)dir + " LMRD: " + (String)analogRead(LMRD) + " LMLD: " + (String)analogRead(LMLD));
}