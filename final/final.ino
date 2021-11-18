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
  Serial.println("setup!");
//  pinMode(electroPin, OUTPUT);
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

    // Pulse Sensor Library indicates to call this at least every 2 seconds for acturate pulse readings if 
    // USE_ARDUINO_INTERRUPTS sets to false. USE_ARDUINO_INTERRUPTS interupting and causing issue with servo
    // motor so we must take these alternate actions.
    pulseSensor.sawNewSample();
//    pulseSensor.sawNewSample();

    int reading0 = pulseSensor.getBeatsPerMinute(0);
    int reading1 = pulseSensor.getBeatsPerMinute(1);
    Serial.println("reading 0: " + String(reading0) + " reading 1: " + String(reading1));

    q0.push(&reading0);
    q1.push(&reading1); 

    avgBPM0 = avgBPM(&q0, "q0 ");
    avgBPM1 = avgBPM(&q1, "q0 ");
    
    if(millis() % 20 < 2) {
      servoMotor.write(servoAngle);
    }

  if (GAME_SEQUENCE[STATE_IDX] == START && avgBPM0 > 20 && avgBPM1 > 20 ) {
    //Serial.println("CALIBRATING...");
    STATE_IDX++;
  }

  if (GAME_SEQUENCE[STATE_IDX] == CALIBRATE && isCalibrated(&bpmWindow[0]) && isCalibrated(&bpmWindow[1])) {
      //Serial.println("STARTING ACTIVE GAME...");
      BASE_RATE[0] = avgBPM0;
      BASE_RATE[1] = avgBPM1;
      STATE_IDX++;
  }

  if (GAME_SEQUENCE[STATE_IDX] == GAME_ACTIVE && abs(avgBPM0 - avgBPM1) > winningDiff) {
    //Serial.println("GAME OVER...");
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
    //Serial.print("State: ");
    Serial.print(GAME_SEQUENCE[STATE_IDX] + ",");
    if (GAME_SEQUENCE[STATE_IDX] == GAME_OVER) {
      Serial.println(winningPlayer);
    }
    else {
      //Serial.print("BPM0: ");
      Serial.print(String(avgBPM0) + ",");
      //Serial.print("BPM1: ");
      Serial.println(String(avgBPM1));
    }
  }
}

int avgBPM(cppQueue * q, String type) {
  int sum = 0;
  int ref;
  for (int i = 0; i < q->getCount(); i++) {
    q->peekIdx(&ref, i);
    sum += ref;
  }
  return round(sum / q->getCount());
}

int avgRelBPM(cppQueue * q, int base) {
  int sum = 0;
  for (int i = 0; i < q->getCount(); i++) {
    int ref;
    q->peekIdx(&ref, i);
    int relBPMPercent = ref/base;
    sum += relBPMPercent;
  }
  return round(sum / q->getCount());
}


bool isCalibrated(cppQueue * q) {
  if (!q->isFull()) {
    return false;
  }
  int min = 1000;
  int max = 0;
  for (int i = 0; i < q->getCount(); i++) {
    int ref;
    q->peekIdx(&ref, i);
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
