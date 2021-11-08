
#include "Servo.h"

#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>

const int OUTPUT_TYPE = SERIAL_PLOTTER;

const int PULSE_SENSOR_COUNT = 2;
byte myBPM[2]; //create an array to store BPM
int deltaBPM;

const int PULSE_INPUT0 = A0;
const int PULSE_BLINK0 = 13;    // Pin 13 is the on-board LED
const int PULSE_FADE0 = 5;

const int PULSE_INPUT1 = A1;
const int PULSE_BLINK1 = 12;
const int PULSE_FADE1 = 11;

const int THRESHOLD = 550;   // Adjust this number to avoid noise when idle

PulseSensorPlayground pulseSensor(PULSE_SENSOR_COUNT);

Servo servoMotor;   
const int servoPin = 3;
int servoAngle = 90;

void setup() {

  Serial.begin(250000);
  
  servoMotor.attach(servoPin);
  servoMotor.write(servoAngle);

  pulseSensor.analogInput(PULSE_INPUT0, 0);
  pulseSensor.blinkOnPulse(PULSE_BLINK0, 0);
  pulseSensor.fadeOnPulse(PULSE_FADE0, 0);

  pulseSensor.analogInput(PULSE_INPUT1, 1);
  pulseSensor.blinkOnPulse(PULSE_BLINK1, 1);
  pulseSensor.fadeOnPulse(PULSE_FADE1, 1);

  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);
  
  if (!pulseSensor.begin()) {

    for (;;) {
      // Flash the led to show things didn't work.
      digitalWrite(PULSE_BLINK0, LOW);
      delay(50);
      digitalWrite(PULSE_BLINK0, HIGH);
      delay(50);
    }
  }
}

void loop() {

  delay(20);

  for (int i = 0; i < PULSE_SENSOR_COUNT; ++i) {
//    if (pulseSensor.sawStartOfBeat(i)) {
     myBPM [i] = pulseSensor.getBeatsPerMinute(i);
     deltaBPM = myBPM[0] - myBPM[1];
     
     Serial.print(myBPM[0]);
     Serial.print(",");
     Serial.print(myBPM[1]);
     Serial.print(",");
     Serial.print(deltaBPM);
     Serial.print(",");
     Serial.println(servoAngle);
    }
  //}
    moveServo(deltaBPM);
}

  void moveServo(int value){
  servoAngle = map(value,20,150,120,90);
  servoMotor.write(servoAngle);
}
