#pragma once

float bufIn[BUFFER_SIZE];
float bufOut[BUFFER_SIZE];
float bufFFT[FFT_SIZE * 2];
float bufFFT2[FFT_SIZE * 2];

#define INPUT_BUFFER(ind) bufIn[(ind) & BUFFER_MASK]
#define OUTPUT_BUFFER(ind) bufOut[(ind) & BUFFER_MASK]
#define OUTPUT_BUFFER_FOR(ind) bufOut[((ind) - OUTPUT_DELAY) & BUFFER_MASK]

#define FFT_BUFFER_RE(ind) bufFFT[((ind) & FFT_MASK) * 2]
#define FFT_BUFFER_IM(ind) bufFFT[((ind) & FFT_MASK) * 2 + 1]

static int busyScale = 0;

static void markBusy() {
  if (busyScale == 0) {
    digitalWrite(PIN_BUSY, 1);
  }

  busyScale++;
}

static void markIdle() {
  busyScale--;

  if (busyScale == 0) {
    digitalWrite(PIN_BUSY, 0);
  }
}
