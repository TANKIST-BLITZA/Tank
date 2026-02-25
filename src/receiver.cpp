#include <Arduino.h>
#include <Servo.h>
#include <RF24.h>

#define SERVO 3
#define BTN_LEFT 4
#define BTN_RIGHT 5

byte address[][6] = {"1Node", "2Node"};
byte channel = 127;

Servo servo;
RF24 rf(2, 6);

byte pipeNo;

long expRunningAverage(long newVal);

void setup() {
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);

  Serial.begin(9600);
  delay(1000);
  servo.attach(SERVO);

  if (!rf.begin()) {
    Serial.println("Radio module is not responding!");
    while (!rf.begin()) {}
  }

  rf.setAutoAck(1);
  rf.setRetries(0, 15);
  rf.enableAckPayload();
  rf.setPayloadSize(6);
  
  rf.openReadingPipe(1, address[0]);
  rf.setChannel(channel);
  
  rf.setPALevel(RF24_PA_MAX);
  rf.setDataRate(RF24_1MBPS);

  rf.powerUp();
  rf.startListening();
}

int errorX, errorY, posX, posY;

unsigned long last = millis();

void loop() {
  if (millis() - last >= 3) {
    if (digitalRead(BTN_LEFT) && digitalRead(BTN_RIGHT)) {
      posX = expRunningAverage(errorX);
      posY = expRunningAverage(errorY);
      
      posX = constrain(posX, -190, 190);
      posY = constrain(posY, -190, 190);
      
      posX = map(posX, -190, 190, 0, 180);
      posY = map(posY, -190, 190, 0, 180);
      
      servo.write(posY);
    }
    else {
      servo.write(85);
    }
  }
  
  while (rf.available(&pipeNo)) {
    static int in_data[2];

    rf.read(&in_data, sizeof(in_data));

    errorX = in_data[0];
    errorY = in_data[1];
  }
}

long expRunningAverage(long newVal) {
  static long filVal = 0;
  filVal += (newVal - filVal) * 0.02;
  return filVal;
}