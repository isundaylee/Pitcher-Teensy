#pragma once

#include "Arduino.h"

const int PIN_AUDIO_IN = A2;
const int PIN_AUDIO_OUT = A21;
const int PIN_BUSY = A4;

const int SAMPLE_RATE = 44100 / 4;
const float US_PER_SAMPLE = 1000000.0 / SAMPLE_RATE;

const int BUFFER_SIZE = 8192;
const int BUFFER_MASK = BUFFER_SIZE - 1;
const int OUTPUT_DELAY = 4096;

const float SAMPLE_ZERO_ALPHA = 0.0001;
const int ANALOG_AVERAGING = 1;
const int ANALOG_RESOLUTION = 10;
const int ANALOG_SCALE = 1 << (ANALOG_RESOLUTION - 1);

const int FFT_SIZE = 1024;
const int FFT_MASK = FFT_SIZE - 1;

const int OVERLAP_FACTOR = 4;
const int WINDOW_SIZE = FFT_SIZE;
const int WINDOW_MASK = WINDOW_SIZE - 1;
const int WINDOW_STEP = WINDOW_SIZE / OVERLAP_FACTOR;
const int WINDOW_STEP_MASK = WINDOW_STEP - 1;
