#include <Arduino.h>
#include <Servo.h>
#include <RF24.h>

#define SERVO 3
#define BTN_LEFT 4
#define BTN_RIGHT A0

#define LMLD 5
#define LMRD 6
#define RMLD 9
#define RMRD 10

const bool isMotorsOn = true;

byte address[][6] = {"1Node", "2Node"};
byte channel = 127;

Servo servo;
RF24 rf(2, A1);

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
        posX = expRunningAverage(errorX);
        posY = expRunningAverage(errorY);

        if (isMotorsOn){
            static int dirX;
            static int dirY;
            
            if (posX >= -20 && posX <= 20) {
                dirX = 0;
                analogWrite(LMLD, 0);
                analogWrite(LMRD, 0);
            }
            if (posX < -20) {
                dirX = map(posX, -20, -511, 0, 255);
                analogWrite(LMLD, dirX);
                digitalWrite(LMRD, 0);
            }
            if (posX > 20) {
                dirX = map(posX, 20, 511, 0, 255);
                analogWrite(LMRD, dirX);
                digitalWrite(LMLD, 0);
            }
            if (posY >= -20 && posY <= 20) {
                dirY = 0;
                analogWrite(RMLD, 0);
                analogWrite(RMRD, 0);
            }
            if (posY < -20) {
                dirY = map(posY, -20, -511, 0, 255);
                analogWrite(RMLD, dirY);
                digitalWrite(RMRD, 0);
            }
            if (posY > 20) {
                dirY = map(posY, 20, 511, 0, 255);
                analogWrite(RMRD, dirY);
                digitalWrite(RMLD, 0);
            }
        }
        else {
            posX = constrain(posX, -190, 190);
            posY = constrain(posY, -190, 190);
    
            posX = map(posX, -190, 190, 0, 180);
            posY = map(posY, -190, 190, 0, 180);

            if (digitalRead(BTN_LEFT) && digitalRead(BTN_RIGHT)) {  
            servo.write(posY);
        }
        else {
            servo.write(85);
        }
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