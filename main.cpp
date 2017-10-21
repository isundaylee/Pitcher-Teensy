#include "config.h"
#include "util.h"

IntervalTimer timer;

int lastProcessed = 0;
int lastRead = 0;

float sampleZero = 0.67 * ANALOG_SCALE;

float in[BUFFER_SIZE];
float out[BUFFER_SIZE];
float fft_in[FFT_SIZE];
float fft_out[FFT_SIZE];

void sample() {
  markBusy();

  lastRead++;

  float reading = analogRead(PIN_AUDIO_IN);
  sampleZero =
      (1.0 - SAMPLE_ZERO_ALPHA) * sampleZero + SAMPLE_ZERO_ALPHA * reading;
  in[lastRead & BUFFER_MASK] = 1.0 * (reading - sampleZero) / ANALOG_SCALE;

  if (lastRead - OUTPUT_DELAY <= lastProcessed) {
    // If the output sample due this moment is ready already;
    float sample = out[(lastRead - OUTPUT_DELAY) & BUFFER_MASK];
    analogWrite(PIN_AUDIO_OUT, int(sample / 3.0 * ANALOG_SCALE) + sampleZero);
  }

  markIdle();
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_BUSY, OUTPUT);
  pinMode(PIN_AUDIO_IN, INPUT);
  pinMode(PIN_AUDIO_OUT, OUTPUT);

  analogReadResolution(ANALOG_RESOLUTION);
  analogReadAveraging(ANALOG_AVERAGING);
  analogWriteResolution(ANALOG_RESOLUTION);

  timer.begin(sample, US_PER_SAMPLE);
}

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

  if ((lastProcessed & FFT_MASK) == 0) {
  }

  markIdle();
}
