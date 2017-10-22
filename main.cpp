#include "config.h"
#include "fft.h"
#include "util.h"

IntervalTimer timer;

int lastProcessed = 0;
int lastRead = 0;

float sampleZero = 0.67 * ANALOG_SCALE;

int attempts = 0;
int successes = 0;
int shift = 1;

void sample() {
  markBusy();

  lastRead++;

  float reading = analogRead(PIN_AUDIO_IN);
  sampleZero =
      (1.0 - SAMPLE_ZERO_ALPHA) * sampleZero + SAMPLE_ZERO_ALPHA * reading;
  INPUT_BUFFER(lastRead) = 1.0 * (reading - sampleZero) / ANALOG_SCALE;

  attempts++;

  if (lastRead - OUTPUT_DELAY <= lastProcessed) {
    successes++;
    // If the output sample due this moment is ready already;
    float sample = OUTPUT_BUFFER_FOR(lastRead);
    analogWrite(PIN_AUDIO_OUT, int(sample / 4.0 * ANALOG_SCALE) + sampleZero);
  }

  markIdle();
}

void setup() {
  // Serial.begin(9600);

  pinMode(PIN_BUSY, OUTPUT);
  pinMode(PIN_AUDIO_IN, INPUT);
  pinMode(PIN_AUDIO_OUT, OUTPUT);

  analogReadResolution(ANALOG_RESOLUTION);
  analogReadAveraging(ANALOG_AVERAGING);
  analogWriteResolution(ANALOG_RESOLUTION);

  fftInit();

  timer.begin(sample, US_PER_SAMPLE);
}

void doWindow(int end) {
  static float lastPhases[FFT_SIZE / 2];
  static float bias[FFT_SIZE / 2];

  int start = end - WINDOW_SIZE + 1;

  for (int i = start; i <= end; i++) {
    FFT_BUFFER_RE(i) = INPUT_BUFFER(i);
    FFT_BUFFER_IM(i) = 0.0;
  }

  fft(bufFFT);
  ifft(bufFFT, bias);

  for (int i = end - WINDOW_STEP + 1; i <= end; i++) {
    OUTPUT_BUFFER(i) = 0;
  }

  for (int i = start; i <= end; i++) {
    OUTPUT_BUFFER(i) +=
        windowEnvelop[(i - start) & WINDOW_MASK] * FFT_BUFFER_RE(i);
  }

  noInterrupts();
  lastProcessed = start;
  interrupts();
}

void loop() {
  markBusy();

  noInterrupts();
  int start = lastProcessed + 1;
  int end = lastRead;
  interrupts();

  if (shift) {
    for (int i = start; i <= end; i++) {
      if ((i & WINDOW_STEP_MASK) == WINDOW_STEP_MASK) {
        doWindow(i);
      }
    }
  } else {
    for (int i = start; i <= end; i++) {
      OUTPUT_BUFFER(i) = INPUT_BUFFER(i);
    }

    noInterrupts();
    lastProcessed = end;
    interrupts();
  }

  markIdle();
}
