#pragma once

#include <stdio.h>
#include "wav.h"


void write_key(WAV* wav, char* note, int* pin, int* end_pin, double t);
void write_kick(WAV* wav, int* pin, int* end_pin, double t);
void write_hihat(WAV* wav, int* pin, int* end_pin, double t);
void write_snare(WAV* wav, int* pin, int* end_pin, double t);
// void generateSnare(WAV* wav, int* pin, int* end_pin, double t);
float bandpass_filter(float sample);
void write_crash(WAV* wav, int* pin, int* end_pin, double t);