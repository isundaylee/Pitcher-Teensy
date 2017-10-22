#include "config.h"
#include "fft.h"
#include "util.h"

IntervalTimer timer;

int lastProcessed = 0;
int lastRead = 0;

float sampleZero = 392.82;

int attempts = 0;
int successes = 0;
int shift = -1;
float shiftRate = pow(10, log(2) / 12.0 * shift);

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
  pinMode(PIN_AUDIO_IN, INPUT);
  pinMode(PIN_AUDIO_OUT, OUTPUT);

  analogReadResolution(ANALOG_RESOLUTION);
  analogReadAveraging(ANALOG_AVERAGING);
  analogWriteResolution(ANALOG_RESOLUTION);

  fftInit();

  timer.begin(sample, US_PER_SAMPLE);
}

void pitchShift() {
  // All phases are in radian
  // All freqs are in multiples of bin frequency

  static float lastPhases[FFT_SIZE / 2 + 1];
  static float sumPhases[FFT_SIZE / 2 + 1];
  static float trueFreqs[FFT_SIZE / 2 + 1];
  static float trueMags[FFT_SIZE / 2 + 1];
  static float synFreqs[FFT_SIZE / 2 + 1];
  static float synMags[FFT_SIZE / 2 + 1];

  float expDiff = 2.0 * PI / (float)OVERLAP_FACTOR;

  for (int i = 0; i <= FFT_SIZE / 2; i++) {
    float phase = altAtan2(FFT_BUFFER_IM(i), FFT_BUFFER_RE(i));
    float phaseDiff = phase - lastPhases[i];
    lastPhases[i] = phase;

    // Shift phaseDiff to [-PI, PI)
    phaseDiff -= (float)i * expDiff;
    int wholePis = phaseDiff / PI;
    if (wholePis >= 0)
      wholePis += wholePis & 1;
    else
      wholePis -= wholePis & 1;
    phaseDiff -= PI * (double)wholePis;

    trueFreqs[i] = (float)i + (phaseDiff * OVERLAP_FACTOR / (2.0 * PI));
    trueMags[i] = 6.0 * sqrt(FFT_BUFFER_IM(i) * FFT_BUFFER_IM(i) +
                             FFT_BUFFER_RE(i) * FFT_BUFFER_RE(i));
  }

  memset(synFreqs, 0, sizeof(synFreqs));
  memset(synMags, 0, sizeof(synMags));

  for (int i = 0; i <= FFT_SIZE / 2; i++) {
    int newIndex = shiftRate * i;
    if (newIndex <= FFT_SIZE / 2) {
      synMags[newIndex] += trueMags[i];
      synFreqs[newIndex] = trueFreqs[i] * shiftRate;
    }
  }

  for (int i = 0; i <= FFT_SIZE / 2; i++) {
    float phaseDelta = 2.0 * PI * synFreqs[i] / OVERLAP_FACTOR;
    sumPhases[i] += phaseDelta;

    float phase = sumPhases[i];

    FFT_BUFFER_RE(i) = altCos(phase) * synMags[i];
    FFT_BUFFER_IM(i) = altSin(phase) * synMags[i];
  }

  for (int i = FFT_SIZE / 2 + 1; i < FFT_SIZE; i++) {
    FFT_BUFFER_RE(i) = 0.0;
    FFT_BUFFER_IM(i) = 0.0;
  }
}


void doWindow(int end) {
  int start = end - WINDOW_SIZE + 1;

  for (int i = 0; i < WINDOW_SIZE; i++) {
    FFT_BUFFER_RE(i) = windowEnvelop[i] * INPUT_BUFFER(start + i);
    FFT_BUFFER_IM(i) = 0.0;
  }

  fft(bufFFT);
  pitchShift();
  ifft(bufFFT);

  for (int i = end - WINDOW_STEP + 1; i <= end; i++) {
    OUTPUT_BUFFER(i) = 0;
  }

  for (int i = 0; i < WINDOW_SIZE; i++) {
    OUTPUT_BUFFER(start + i) += windowEnvelop[i] * FFT_BUFFER_RE(i);
  }

  noInterrupts();
  lastProcessed = start + WINDOW_STEP - 1;
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
