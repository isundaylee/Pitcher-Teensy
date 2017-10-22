#pragma once

#include "arm_math.h"

static arm_cfft_radix2_instance_f32 _insFFT;
static arm_cfft_radix2_instance_f32 _insIFFT;

const int TRIG_TABLE_COUNT = 1024;
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

static float altAtan2(float y, float x) {}

static void ifft(float* data, float* bias) {
  arm_cfft_radix2_f32(&_insIFFT, data);
  // static float buffer[FFT_SIZE];
  //
  // for (int i = 0; i < FFT_SIZE / 2; i++) {
  //   bias[i] += (float)i;
  // }
  //
  // for (int i = 0; i < FFT_SIZE; i++) {
  //   buffer[i] = 0.0;
  //   for (int j = 0; j < FFT_SIZE / 2; j++) {
  //     float angle = 2 * PI * float(i) * float(j) / (float)FFT_SIZE;
  //     buffer[i] +=
  //         data[2 * j] * altCos(angle) - data[2 * j + 1] * altSin(angle);
  //   }
  // }
  //
  // for (int i = 0; i < FFT_SIZE; i++) {
  //   data[2 * i] = buffer[i] / FFT_SIZE;
  //   data[2 * i + 1] = 0.0;
  // }
}
