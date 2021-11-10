#include "Servo.h"
#include "gamestate.h"
#include "pulseSensor.h"
#include "servoMotor.h"

int magnetState = HIGH;
const bool printSerialForP5 = false;
int winningDiff = 20;
int winningPlayer = -1;
int maxVariance = 20;

void setup() {
  Serial.begin(250000);
  //Serial.begin(9600);
  Serial.println("setup!");
  pinMode(electroPin, OUTPUT);
  servoMotor.attach(servoPin);

  pulseSensor.analogInput(PULSE_INPUT0, 0);
  pulseSensor.analogInput(PULSE_INPUT1, 1);
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
  digitalWrite(electroPin, magnetState);

  int bpm = pulseSensor.getBeatsPerMinute(0);
  bpmWindow[0].push(&bpm);

  bpm = pulseSensor.getBeatsPerMinute(1);
  bpmWindow[1].push(&bpm);

  int avgBPM0 = avgBPM(&bpmWindow[0]);
  int avgBPM1 = avgBPM(&bpmWindow[0]);

  if (GAME_SEQUENCE[STATE_IDX] == GAME_ACTIVE) {
    servoMotor.write(avgBPM0 - avgBPM1);
  }

  if (GAME_SEQUENCE[STATE_IDX] == START && avgBPM0 > 20 && avgBPM1 > 20 ) {
    STATE_IDX++;
  }
  if (GAME_SEQUENCE[STATE_IDX] == CALIBRATE && isCalibrated(&bpmWindow[0]) && isCalibrated(&bpmWindow[1])) {
      BASE_RATE[0] = avgBPM0;
      BASE_RATE[1] = avgBPM1;
      STATE_IDX++;
  }
  if (GAME_SEQUENCE[STATE_IDX] == GAME_ACTIVE && abs(avgBPM0 - avgBPM1) > winningDiff) {
    STATE_IDX++;
    int magnetState = LOW; //release ball!
    if (avgBPM0  < avgBPM1) {
      winningPlayer = 0;
    }
    else {
      winningPlayer = 1;
    }
  }
  if (printSerialForP5) {
    Serial.print(GAME_SEQUENCE[STATE_IDX] + ",");
    if (GAME_SEQUENCE[STATE_IDX] == GAME_OVER) {
      Serial.println(winningPlayer);
    }
    else {
      Serial.print(avgBPM0 + ",");
      Serial.println(avgBPM1);
    }
  }
}

int avgBPM(cppQueue &q) {
  int sum = 0;
  for (int i = 0; i < q.getCount(); i++) {
    int ref;
    q.peekIdx(&ref, i);
    sum += ref;
  }
  return sum / q.getCount();
}


bool isCalibrated(cppQueue &q) {
  if (!q.isFull()) {
    return false;
  }
  int min = 1000;
  int max = 0;
  for (int i = 0; i < q.getCount(); i++) {
    int ref;
    q.peekIdx(&ref, i);
    if (ref < min) {
      min = ref;
    }
    if (ref > max) {
      max = ref;
    }
  }
  if (max - min < maxVariance) {
    return true;
  }
  return false;
}
