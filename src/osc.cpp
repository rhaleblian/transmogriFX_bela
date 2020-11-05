/***** osc.cpp *****/

// Attaches TouchOSC controls.
// Use the 'transmogriFX' layout file.
// Change remoteip etc for your site -- the TouchOSC settings will give hints.
// ATM each effect uses a different structural idiom while Ray searches for the best one.
// There is some C++ in here but effort is made to not put in too much.

#include <Bela.h>
#include <stdio.h>
#include <iostream>
#include "osc.h"
#include "osc_delay.h"
#include "osc_wah.h"

namespace osc {

// BEGIN SITE CONFIGURATION
const int localport = 65001;
const int remoteport = 65002;
const char* remoteip = "192.168.1.65";  // iPad 2
// const char* remoteip = "192.168.1.73";  // Pixel 3a
// END SITE CONFIGURATION

// Local pointers to devices.
// Sustainer* sustainer;
// eq_filters *eq1, *eq2;
// tflanger* flanger;
// phaser_coeffs* phaser;
delay::Faceplate* delay;
// trem_coeffs* tremolo;
wah::Faceplate* wahwah;

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

void create_knob(knob_t& knob, const char* address, int id,
			  float map0, float map1, float map2, float map3,
			  float value)
{
	knob.address = address;
	knob.id = id;
	knob.map[0] = map0;
	knob.map[1] = map1;
	knob.map[2] = map2;
	knob.map[3] = map3;
	knob.value = value;
}

void print_knob(knob_t &knob) {
	printf("id %d value %f address %s ", knob.id, knob.value, knob.address);
	printf("map %f %f %f %f\n", knob.map[0], knob.map[1], knob.map[2], knob.map[3]);
}

void send_knob(knob_t &knob) {
	float value = todomain(knob.value, knob.map);
	printf("%s <- %f\n", knob.address, value);
	sender.newMessage(knob.address).add(value).send();
}

void send_address_value(const char *address, float value) {
	// Use this when you don't have a knob for the value.
	sender.newMessage(address).add(value).send();
}

#include "osc_compressor.hpp"
#include "osc_sustainer.hpp"
#include "osc_klingon.hpp"
#include "osc_overdrive.hpp"
#include "osc_chorus.hpp"
#include "osc_reverb.hpp"

void on_receive(oscpkt::Message* msg, void* arg) {
	// printf("received %s\n", msg->addressPattern().c_str());
	wahwah->receive(msg);
	receive_compressor(msg);
	receive_sustainer(msg);
	receive_klingon(msg);
	receive_overdrive(msg);
	receive_chorus(msg);
	delay->receive(msg);
	receive_reverb(msg);
}

void setup(
	iwah_t* wah,
	feedback_compressor* cmp,
	Sustainer* sus,
	klingon *kot,
	::overdrive *ovd,
	tflanger *cho,
	tflanger *dly,
	Reverb *rev)
{
	receiver.setup(localport, on_receive);
	sender.setup(remoteport, remoteip);
	
	wahwah = new wah::Faceplate();
	delay = new delay::Faceplate();

	wahwah->setup(wah);
	setup_compressor(cmp);
	setup_sustainer(sus);
	setup_klingon(kot);
	setup_overdrive(ovd);
	setup_chorus(cho);
	delay->setup(dly);
	setup_reverb(rev);
}

void teardown() {}

}
