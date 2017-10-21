#pragma once

#include "Arduino.h"

const int PIN_AUDIO_IN = A5;
const int PIN_AUDIO_OUT = A21;
const int PIN_BUSY = A4;

const int SAMPLE_RATE = 44100 / 4;
const float US_PER_SAMPLE = 1000000.0 / SAMPLE_RATE;

const int BUFFER_SIZE = 8192;
const int BUFFER_MASK = BUFFER_SIZE - 1;
const int OUTPUT_DELAY = 1024;

const float SAMPLE_ZERO_ALPHA = 0.001;
const int ANALOG_AVERAGING = 4;
const int ANALOG_RESOLUTION = 12;
const int ANALOG_SCALE = 1 << (ANALOG_RESOLUTION - 1);

const int FFT_SIZE = 512;
const int FFT_MASK = FFT_SIZE - 1;
