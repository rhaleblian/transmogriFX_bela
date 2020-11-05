/***** osc.h
Open Sound Control I/O for turning knobs
when you have no physical knobs.

This is best suited for TouchOSC using a
specific layout included with the source code.
*****/
#pragma once

#include <libraries/OscSender/OscSender.h>
#include <libraries/OscReceiver/OscReceiver.h>
#include "fb_compressor.h"
#include "Sustainer.h"
#include "eq.h"
// #include "lfo.h"
// #include "inductorwah.h"
// #include "phaser.h"
// #include "trem.h"
// #include "envelope_filter.h"
#include "overdrive.h"
#include "klingon.h"
#include "flange.h"
#include "reverb.h"

namespace osc {

typedef float parameter_map[4];

typedef struct {
	unsigned int id;
	const char* address;
	parameter_map map;
	float value;
} knob_t;

float todomain(float f, parameter_map &p);
float torange(float f, parameter_map &p);
void send_address_value(const char *address, float value);

void setup(
	feedback_compressor* compressor,
	Sustainer* sustainer,
	klingon* klingontone,
	overdrive* overdrive,
	tflanger* chorus,
	tflanger* delay,
	Reverb* reverb);

void send_knob(knob_t &knob);

void teardown();

}
