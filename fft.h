#pragma once

#include "arm_math.h"

static arm_cfft_radix2_instance_f32 _insFFT;
static arm_cfft_radix2_instance_f32 _insIFFT;

const int TRIG_TABLE_COUNT = 4096;
const int TRIG_TABLE_MASK = TRIG_TABLE_COUNT - 1;
const float TRIG_TABLE_RESOLUTION = 2 * PI / TRIG_TABLE_COUNT;

// float angCons = 6.2831852 / (float)FFT_SIZE;
float sinTable[TRIG_TABLE_COUNT];
float cosTable[TRIG_TABLE_COUNT];

float windowEnvelop[WINDOW_SIZE];

static void fftInit() {
  arm_cfft_radix2_init_f32(&_insFFT, FFT_SIZE, 0, 1);
  arm_cfft_radix2_init_f32(&_insIFFT, FFT_SIZE, 1, 1);

  for (int i = 0; i < TRIG_TABLE_COUNT; i++) {
    sinTable[i] = sin(TRIG_TABLE_RESOLUTION * i);
    cosTable[i] = cos(TRIG_TABLE_RESOLUTION * i);
  }

  for (int i = 0; i < WINDOW_SIZE; i++) {
    // Linear envelop
    // windowEnvelop[i] = 1.0 * i / (WINDOW_SIZE / 2);
    // windowEnvelop[WINDOW_SIZE - 1 - i] = 1.0 * i / (WINDOW_SIZE / 2);

    // Cosine envelop
    windowEnvelop[i] = 0.5 * (1 - cos(i * (2 * PI / WINDOW_SIZE)));

    // Unity envelop
    // windowEnvelop[i] = 1.0;
  }
}

static void fft(float* data) { arm_cfft_radix2_f32(&_insFFT, data); }

static float altSin(float x) {
  return sinTable[(int)(x / TRIG_TABLE_RESOLUTION) & TRIG_TABLE_MASK];
}

static float altCos(float x) {
  return cosTable[(int)(x / TRIG_TABLE_RESOLUTION) & TRIG_TABLE_MASK];
}

static float altAtan(float x) {
  if (x >= -1 && x <= 1) {
    return (0.97179803008 * x) - (0.19065470515 * x * x * x);
  } else {
    x = 1 / x;
    float ans = (0.97179803008 * x) - (0.19065470515 * x * x * x);

    if (x > 0) {
      return 0.5 * PI - ans;
    } else {
      return -0.5 * PI - ans;
    }
  }
}

static float altAtan2(float y, float x) {
  if (x == 0) {
    return 0.0;
  }

  if (x > 0) {
    return altAtan(y / x);
  } else {
    if (y > 0) {
      return altAtan(y / x) + PI;
    } else {
      return altAtan(y / x) - PI;
    }
  }
}

static void ifft(float* data) { arm_cfft_radix2_f32(&_insIFFT, data); }
