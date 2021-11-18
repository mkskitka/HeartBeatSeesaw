
#include "cppQueue.h"
// interupts increase the accuracy of the pulse sensor measure
#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>
// First In First Out (FIFO) Queue used to average bpm over a window of time
#define  IMPLEMENTATION  FIFO
//pulse reading window is the window size to average bpm values over (creates smoother curve)
const int pulseReadingWindow = 20;
int deltaSum = 0;

// BPM window for A0
cppQueue  q0(sizeof(int), pulseReadingWindow, IMPLEMENTATION, true); // Instantiate queue
//BPM window for A1
cppQueue  q1(sizeof(int), pulseReadingWindow, IMPLEMENTATION, true); // Instantiate queue

// number of pressure sensors being used
const int PULSE_SENSOR_COUNT = 2;
PulseSensorPlayground pulseSensor(PULSE_SENSOR_COUNT);
int BASE_RATE[2];
cppQueue bpmWindow[] = {q0, q1};
const int THRESHOLD = 550;   // Adjust this number to avoid noise when idle
// Are the bpm's calibrated (difference between readings not greater than certain range)

const int PULSE_INPUT0 = A0;
const int PULSE_INPUT1 = A1;

int avgBPM0; 
int avgBPM1; 
