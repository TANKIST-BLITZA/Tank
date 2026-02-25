#include <Arduino.h>
#include <RF24.h>

RF24 rf(2, 4);

byte address[][6] = {"1Node", "2Node"};
byte channel = 127;

uint8_t stickError = 20;

#define stickX A0
#define stickY A1

int stickX0, stickY0;
int out_data[2];

void setup() {
    Serial.begin(9600);

    delay(50);
    stickX0 = analogRead(stickX);
    delay(50);
    stickY0 = analogRead(stickY);

    if (!rf.begin()) {
        Serial.println("Radio module is not responding!");
        while (!rf.begin()) {}
    }
    rf.setAutoAck(1);
    rf.setRetries(0, 15);
    rf.enableAckPayload();
    rf.setPayloadSize(6);

    rf.openWritingPipe(address[0]);
    rf.setChannel(channel);

    rf.setPALevel(RF24_PA_MAX);
    rf.setDataRate(RF24_1MBPS);

    rf.powerUp();
    rf.stopListening();
}

void loop() {
    static int moveX, moveY;
    if (abs(stickX0 - analogRead(stickX)) > stickError) moveX = stickX0 - analogRead(moveX); else moveX = 0;
    if (abs(stickY0 - analogRead(stickY)) > stickError) moveY = stickY0 - analogRead(moveY); else moveY = 0;

    out_data[0] = moveX;
    out_data[1] = moveY;
    rf.write(&out_data, sizeof(out_data));
    Serial.println("Pos X: " + (String)out_data[0] + ", Pos Y: " + (String)out_data[1]);
}