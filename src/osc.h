/***** osc.h
Open Sound Control I/O for turning knobs
when you have no physical knobs.

This is best suited for TouchOSC using a
specific layout included with the source code.
*****/
#pragma once

#include "fb_compressor.h"
#include "Sustainer.h"
#include "eq.h"
#include "overdrive.h"
#include "klingon.h"
#include "flange.h"
#include "reverb.h"
// #include "lfo.h"
// #include "inductorwah.h"
// #include "phaser.h"
// #include "trem.h"
// #include "envelope_filter.h"

namespace osc {

typedef float parameter_map[4];

typedef struct {
	unsigned int id;
	const char* address;
	parameter_map map;
	float value;
	// (void)setter(void*, float);
} knob_t;

void setup(
	feedback_compressor* compressor,
	Sustainer* sustainer,
	klingon* klingontone,
	overdrive* overdrive,
	tflanger* chorus,
	Reverb* reverb);

void send_knob(knob_t &knob);

void teardown();

}
