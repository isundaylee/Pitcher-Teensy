#include "Arduino.h"

const int PIN_AUDIO_IN = A5;
const int PIN_AUDIO_OUT = A21;
const int PIN_BUSY = A4;

const int SAMPLE_RATE_HZ = 11025;
const int SAMPLE_ZERO = 2037;
const float US_PER_SAMPLE = 1000000.0 / SAMPLE_RATE_HZ;

const int BUFFER_SIZE = 8192;
const int BUFFER_MASK = BUFFER_SIZE - 1;
const int OUTPUT_DELAY = 1024;

IntervalTimer timer;

int lastProcessed = 0;
int lastRead = 0;

const int SCALE = 2048.0;
float in[BUFFER_SIZE];
float out[BUFFER_SIZE];

const int FFT_SIZE = 512;
const int FFT_MASK = FFT_SIZE - 1;
float fft_in[FFT_SIZE];
float fft_out[FFT_SIZE];

static int busyScale = 0;

void markBusy() {
  if (busyScale == 0) {
    digitalWrite(PIN_BUSY, 1);
  }

  busyScale++;
}

void markIdle() {
  busyScale--;

  if (busyScale == 0) {
    digitalWrite(PIN_BUSY, 0);
  }
}

void sample() {
  markBusy();

  lastRead++;

  in[lastRead & BUFFER_MASK] =
      1.0 * (analogRead(PIN_AUDIO_IN) - SAMPLE_ZERO) / SCALE;
  if (lastRead - OUTPUT_DELAY <= lastProcessed) {
    // If the output sample due this moment is ready already;
    float sample = out[(lastRead - OUTPUT_DELAY) & BUFFER_MASK];
    analogWrite(PIN_AUDIO_OUT, int(sample / 3.0 * SCALE) + SAMPLE_ZERO);
  }

  markIdle();
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_BUSY, OUTPUT);
  pinMode(PIN_AUDIO_IN, INPUT);
  pinMode(PIN_AUDIO_OUT, OUTPUT);

  analogReadResolution(12);
  analogReadAveraging(4);
  analogWriteResolution(12);

  timer.begin(sample, US_PER_SAMPLE);
}

int makeWork = 12;

void loop() {
  markBusy();

  noInterrupts();
  for (int i = lastProcessed + 1; i <= lastRead; i++) {
    lastProcessed = i;
    out[i & BUFFER_MASK] = in[i & BUFFER_MASK];
    fft_in[i & FFT_MASK] = in[i & BUFFER_MASK];

    if ((i & FFT_MASK) == 0) {
      break;
    }
  }
  interrupts();

  markIdle();
}
