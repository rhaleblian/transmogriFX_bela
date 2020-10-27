/***** osc.cpp *****/

// Attaches TouchOSC controls.
// Use the 'transmogriFX' layout file.
// Change remoteip etc for your site -- the TouchOSC settings will give hints.
// ATM each effect uses a different structural idiom while Ray searches for the best one.
// There is some C++ in here but effort is made to not put in too much.

#include <Bela.h>
#include <stdio.h>
#include <iostream>
#include <libraries/OscSender/OscSender.h>
#include <libraries/OscReceiver/OscReceiver.h>
#include "osc.h"

namespace osc {

// BEGIN SITE CONFIGURATION
const int localport = 65001;
const int remoteport = 65002;
const char* remoteip = "192.168.1.65";  // iPad 2
// const char* remoteip = "192.168.1.73";  // Pixel 3a
// END SITE CONFIGURATION

// Local pointers to devices.
Sustainer* sustainer;
// overdrive* od;
eq_filters *eq1, *eq2;
tflanger* flanger;
// phaser_coeffs* phaser;
tflanger* delay;
// trem_coeffs* tremolo;
// iwah_coeffs* wah;
Reverb *reverb;

OscReceiver receiver;
OscSender sender;

float todomain(float f, parameter_map &p) {
	// Transform an effect parameter value to an OSC fader value. 
	return map(f, p[2], p[3], p[0], p[1]);
}

float torange(float f, parameter_map &p) {
	// Transform an OSC fader value to an effect parameter value.
	return map(f, p[0], p[1], p[2], p[3]);
}

void print_knob(knob_t &knob) {
	printf("%d %s %f %s ", knob.id, knob.name, *(knob.value), knob.path);
	printf("%f\n", *(knob.map + 3));
}

void send_knob(knob_t &knob) {
	float value = todomain(*(knob.value), knob.map);
	printf("%s <- %f\n", knob.path, value);
	sender.newMessage(knob.path).add(value).send();
}

void send_knobs(knob_t knobs[], int count) {
	for (unsigned int i=0; i<count; i++) {
		send_knob(knobs[i]);
	}
}

#include "osc_compressor.hpp"
#include "osc_klingon.hpp"
#include "osc_overdrive.hpp"
#include "osc_chorus.hpp"
#include "osc_reverb.hpp"

void on_receive(oscpkt::Message* msg, void* arg) {
	printf("%s\n", msg->addressPattern().c_str());
	receive_klingontone(msg);
	receive_chorus(msg);
	receive_reverb(msg);
}

void setup(
	feedback_compressor* cmp,
	Sustainer* sus,
	klingon *kot,
	::overdrive *ovd,
	tflanger *cho,
	Reverb *rev)
{
	receiver.setup(localport, on_receive);
	sender.setup(remoteport, remoteip);
	setup_klingontone(kot);
	setup_chorus(cho);
	setup_reverb(rev);
}

void teardown() {}

}
