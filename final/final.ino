// Servo Motor imports and variables 
#include "servoMotor.h"
// State Variables 
#include "gamestate.h"
// Pulse Sensor imports and variables
#include "pulseSensor.h"

// HIGH = ON, LOW = OFF
int magnetState = HIGH;
const bool printSerialForP5 = true;
int winningDiff = 100;
int winningPlayer = -1;
int maxVariance = 20;
int current_time = 0;

void setup() {
  Serial.begin(250000);
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
  
     int player1_fake = analogRead(A5);      // read the pot value
     int player2_fake = analogRead(A4); 
     int player1_bpm = map(player1_fake, 0, 1023, 0, 200);
     int player2_bpm = map(player2_fake, 0, 1023, 0, 200);
     digitalWrite(electroPin, magnetState); 
     
    // Pulse Sensor Library indicates to call this at least every 2 seconds for acturate pulse readings if 
    // USE_ARDUINO_INTERRUPTS sets to false. USE_ARDUINO_INTERRUPTS interupting and causing issue with servo
    // motor so we must take these alternate actions.
    if (pulseSensor.sawNewSample()) {
      if (pulseSensor.sawStartOfBeat()) {
        int reading0 = pulseSensor.getBeatsPerMinute(0);
        int reading1 = pulseSensor.getBeatsPerMinute(1);
        Serial.println("reading 0: " + String(reading0) + " reading 1: " + String(reading1));

//        q0.push(&reading0);
//        q1.push(&reading1); 

//        avgBPM0 = avgBPM(&q0, "q0 ");
//        avgBPM1 = avgBPM(&q1, "q0 ");
      }
    }
    q0.push(&player1_bpm);
    q1.push(&player2_bpm);
    avgBPM0 = avgBPM(&q0, "q0 ");
    avgBPM1 = avgBPM(&q1, "q1 ");
    
    if(millis() % 20 < 2) {
      servoMotor.write(servoAngle);
    }

  if (GAME_SEQUENCE[STATE_IDX] == START && abs(avgBPM0 - avgBPM1) < 5 && avgBPM0 > 20 && avgBPM0 > 20) {
    //Serial.println("CALIBRATING...");
    current_time = millis(); 
    STATE_IDX++;
  }

  if (GAME_SEQUENCE[STATE_IDX] == CALIBRATE && isCalibrated(&q0) && isCalibrated(&q1)) {
      //Serial.println("STARTING ACTIVE GAME...");

      if(millis() - current_time > 10000) {
        BASE_RATE[0] = avgBPM0;
        BASE_RATE[1] = avgBPM1;
        STATE_IDX++;
      }
  }

  if(GAME_SEQUENCE[STATE_IDX] == GAME_ACTIVE) {
    servoAngle = servoAngleStart + ((avgBPM0 - avgBPM1)/2);
    magnetState = LOW; //release ball!
  }

  if (GAME_SEQUENCE[STATE_IDX] == GAME_ACTIVE && abs(avgBPM0 - avgBPM1) > winningDiff) {
    //Serial.println("GAME OVER...");
    STATE_IDX++;
    servoAngle = servoAngleStart;
    if (avgBPM0  < avgBPM1) {
      winningPlayer = 0;
    }
    else {
      winningPlayer = 1;
    }
  }

  if (GAME_SEQUENCE[STATE_IDX] == GAME_OVER ) {
    servoAngle = servoAngleStart;
    STATE_IDX = 0; 
  }


  if (printSerialForP5) {
    //Serial.print("State: ");
    Serial.print(GAME_SEQUENCE[STATE_IDX] + ",");
    if (GAME_SEQUENCE[STATE_IDX] == GAME_OVER) {
      Serial.println(winningPlayer);
    }
    else {
      //Serial.print("BPM0: ");
//      Serial.print(String(avgBPM0) + ",");
      //Serial.print("BPM1: ");
//      Serial.println(String(avgBPM1));
//      Serial.print("Player0 Delta BPM, Player1 Delta BPM");
        Serial.print(String(player1_bpm) + "," + String(player2_bpm) + ","); 
        Serial.println(String(avgBPM0) + "," + String(avgBPM1)); 
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
