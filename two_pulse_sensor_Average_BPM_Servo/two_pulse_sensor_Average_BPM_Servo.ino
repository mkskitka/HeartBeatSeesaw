#include "Servo.h"

Servo servoMotor;   
int servoPin = 3;
int servoAngle_0;
int servoAngle_1;

int Signal0;
int Signal1;                
int Threshold = 550;           

const byte RATE_SIZE = 4; 

byte rates_0[RATE_SIZE]; 
byte rateSpot_0 = 0;
long lastBeat_0 = 0; 

float beatsPerMinute_0;
int beatAvg_0;

byte rates_1[RATE_SIZE]; 
byte rateSpot_1 = 0;
long lastBeat_1 = 0; 

float beatsPerMinute_1;
int beatAvg_1;


void setup() {
  
   Serial.begin(9600);
   
   servoMotor.attach(servoPin);         
}

void loop() {
  
  delay(20);
  
  int Signal0_old = Signal0;
  int Signal0 = analogRead(0); 
  int Signal1_old = Signal1;
  int Signal1 = analogRead(1);  

    if (Signal0 >= Threshold && Signal0_old < Threshold) {

    long delta = millis() - lastBeat_0;
    lastBeat_0 = millis();

    beatsPerMinute_0 = 60 / (delta / 1000.0);

    if (beatsPerMinute_0 < 255 && beatsPerMinute_0 > 20)
    {
      rates_0[rateSpot_0++] = (byte)beatsPerMinute_0;
      rateSpot_0 %= RATE_SIZE; 

      beatAvg_0 = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg_0 += rates_0[x];
      beatAvg_0 /= RATE_SIZE;
    }
  }


    if (Signal1 >= Threshold && Signal1_old < Threshold) {

    long delta = millis() - lastBeat_1;
    lastBeat_1 = millis();

    beatsPerMinute_1 = 60 / (delta / 1000.0);

    if (beatsPerMinute_1 < 255 && beatsPerMinute_1 > 20)
    {
      rates_1[rateSpot_1++] = (byte)beatsPerMinute_1;
      rateSpot_1 %= RATE_SIZE; 

      beatAvg_1 = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg_1 += rates_1[x];
      beatAvg_1 /= RATE_SIZE;
    }
  }

  if (beatAvg_0 < 200 && beatAvg_0 > 30)
  {
  servoAngle_0 = map(beatAvg_0, 80, 210, 120, 90);
  servoMotor.write(servoAngle_0);
  }

  if (beatAvg_1 < 200 && beatAvg_1 > 30)
  {
  servoAngle_1 = map(beatAvg_1, 80, 210, 60, 90);
  servoMotor.write(servoAngle_1);
  }

//  Serial.print(beatAvg_0);
//  Serial.print(",");
//  Serial.println(beatAvg_1);

  Serial.print(Signal0);
  Serial.print(",");
  Serial.println(Signal1);

}
