#include "Servo.h"

#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>

#include "cppQueue.h"
#define  IMPLEMENTATION  FIFO

const int pulseReadingWindow = 20;
int deltaSum = 0;
cppQueue  q(sizeof(int), pulseReadingWindow, IMPLEMENTATION, true); // Instantiate queue

const int OUTPUT_TYPE = SERIAL_PLOTTER;

const int PULSE_SENSOR_COUNT = 2;
byte myBPM[2];
int deltaBPM;

const int PULSE_INPUT0 = A0;
const int PULSE_INPUT1 = A1;
const int PULSE_BLINK1 = 3;
const int PULSE_FADE1 = 2;

int pulseDifference[pulseReadingWindow];
bool calibrated = true;

const int THRESHOLD = 550;   // Adjust this number to avoid noise when idle

Servo servoMotor;
const int servoPin = 3;
int servoAngle = 90;

PulseSensorPlayground pulseSensor(PULSE_SENSOR_COUNT);

void setup() {

  Serial.begin(250000);

  servoMotor.attach(servoPin);
  servoMotor.write(servoAngle);

  pulseSensor.analogInput(PULSE_INPUT0, 0);
  pulseSensor.analogInput(PULSE_INPUT1, 1);

  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);

  // Now that everything is ready, start reading the PulseSensor signal.
  if (!pulseSensor.begin()) {
    Serial.println("pulse sensor not set up correctly");
  }
  else {
    Serial.println("pulse sensor library started up correctly");
  }
}

void loop() {

  delay(20);

  // write the latest sample to Serial. (raw data)
  // pulseSensor.outputSample();

  for (int i = 0; i < PULSE_SENSOR_COUNT; ++i) {
    myBPM [i] = pulseSensor.getBeatsPerMinute(i);

    deltaBPM = myBPM[0] - myBPM[1];

    // printing raw data do the serial plotter
    // pulseSensor.outputBeat(i);

        //A0 - BPM
        Serial.print(myBPM[0]);
        Serial.print(",");
        //A1 - BPM
        Serial.print(myBPM[1]);
        Serial.print(",");
        // Difference B/W A0 and A1 BPM
        Serial.println(deltaBPM);
  }
  if (calibrated) {
    if (q.isFull()) {
      int ref;
      q.peekIdx(&ref, pulseReadingWindow - 1);
      //      Serial.println(String(ref));
      deltaSum = deltaSum - ref;
    }
    q.push(&deltaBPM);
    deltaSum = deltaSum + deltaBPM;
    //    Serial.println(deltaSum);
    if (q.isFull()) {
      //Serial.println("q count: " + String(q.getCount()));
      int avg_delta = deltaSum / q.getCount();
      //Serial.println(avg_delta);
//      moveServo(avg_delta);
    }
  }
}


void moveServo(int value) {
  if (value < -25) {
    value = -25;
  }
  if (value > 25) {
    value = 25;
  }
  servoAngle = map(value, -25, 25, 88, 92);
  Serial.println("servoAngle " + String(servoAngle));
//  servoMotor.write(servoAngle);
}
