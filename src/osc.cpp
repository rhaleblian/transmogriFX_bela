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
	// Transform an internal parameter value to an OSC fader value. 
	return map(f, p[2], p[3], p[0], p[1]);
}

float torange(float f, parameter_map &p) {
	// Transform an OSC fader value to an internal parameter value.
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
	printf("eh?\n");
	for (unsigned int i=0; i<count; i++) {
		send_knob(knobs[i]);
	}
}

// ------------------------------------------

enum compressor_knob_id {
	COMPRESSOR_THRESHOLD,
	COMPRESSOR_RATIO,
	COMPRESSOR_LEVEL,
	COMPRESSOR_MIX,
	COMPRESSOR_ATTACK,
	COMPRESSOR_RELEASE
};
parameter_map compressor_map[] = {
	{ 0.0, 1.0, -60.0, -12.0 },  // THRESHOLD
	{ 0.0, 1.0, 1.0,    24.0 },  // RATIO
	{ 0.0, 1.0, -24.0,   6.0 },  // LEVEL
	{ 0.0, 1.0, 0.0,     1.0 },  // MIX
	{ 0.0, 1.0, 1.0,  1000.0 },  // ATTACK
	{ 0.0, 1.0, 20.0, 1000.0 }   // RELEASE
};
knob_t compressor_knobs[6];
feedback_compressor* compressor;

void setup_compressor(feedback_compressor *device) {
	// Most of this could be done at compile-time.
	compressor = device;
	
	auto knob = compressor_knobs;
	knob->id = COMPRESSOR_THRESHOLD;
	knob->name = "threshold";
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = -60.0;
	knob->map[3] = -12.0;
	knob->path = "/tmfx/compressor-threshold";
	knob->value = &(compressor->threshold_db);

	knob++;
	knob->id = COMPRESSOR_RATIO;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 1.0;
	knob->map[3] = 24.0;
	knob->name = "ratio";
	knob->path = "/tmfx/compressor-ratio";
	knob->value = &(compressor->ratio);

	knob++;
	knob->id = COMPRESSOR_LEVEL;
	knob->name = "level";
	knob->path = "/tmfx/compressor-level";
	knob->value = &(compressor->ratio);

	knob++;
	knob->id = COMPRESSOR_MIX;
	knob->name = "mix";
	knob->path = "/tmfx/compressor-mix";
	knob->value = &(compressor->ratio);

	knob++;
	knob->id = COMPRESSOR_ATTACK;
	knob->name = "attack";
	knob->path = "/tmfx/compressor-attack";
	knob->value = &(compressor->atk);

	knob++;
	knob->id = COMPRESSOR_RELEASE;
	knob->name = "release";
	knob->path = "/tmfx/compressor-release";
	knob->value = &(compressor->rls);

	send_knobs(compressor_knobs, 1);
}

void write_compressor(unsigned int id, float val) {
	switch (id) {
		case COMPRESSOR_THRESHOLD:
		feedback_compressor_set_threshold(compressor, val);
		break;
		case COMPRESSOR_RATIO:
		feedback_compressor_set_ratio(compressor, val);
		break;
	}
}

bool receive_compressor(oscpkt::Message *msg) {
	float f = 0.5;
	float v = 0.5;

	if (msg->match("/tmfx/compressor-enable")) {
		msg->match("/tmfx/compressor-enable").popFloat(f).isOkNoMoreArgs();
		compressor->bypass = f > 0.0 ? false: true;
		return true;
	} else {
		for (knob_t *knob = compressor_knobs;
			knob != compressor_knobs + 2;
			knob++) {
			if(msg->match(knob->name)) {
				msg->match(knob->name).popFloat(f).isOkNoMoreArgs();
				v = torange(f, knob->map);
				write_compressor(knob->id, v);
				return true;
			}
		}
	}
	return false;
}

// ------------------------------------------

enum klingontone_knob_id {
	KLINGONTONE_DRIVE,
	KLINGONTONE_TONE,
	KLINGONTONE_LEVEL,
	KLINGONTONE_BOOST,
	KLINGONTONE_KNOBCOUNT
};
parameter_map kot_map[4] = {
	{ 0.0, 1.0,  12.0, 45.0 },   // DRIVE
	{ 0.0, 1.0, -40.0,  0.0 },   // TONE
	{ 0.0, 1.0, -40.0,  0.0 },   // LEVEL
	{ 0.0, 1.0,  0.01, 0.99 }    // BOOST
};
klingon *klingontone;

void send_kot() {
	// set TouchOSC knobs to current parameter values.
	printf("klingontone is initially %f %f %f %f\n",
		klingontone->gain,
		klingontone->tone,
		klingontone->level,
		klingontone->hard);
	sender.newMessage("/tmfx/klingontone-drive")
	.add(todomain(klingontone->gain, kot_map[0])).send();
	sender.newMessage("/tmfx/klingontone-tone")
	.add(todomain(klingontone->tone, kot_map[1])).send();
	sender.newMessage("/tmfx/klingontone-level")
	.add(todomain(klingontone->level, kot_map[2])).send();
	sender.newMessage("/tmfx/klingontone-boost")
	.add(todomain(klingontone->hard, kot_map[3])).send();
}

void setup_klingontone(klingon *device) {
	klingontone = device;
	send_kot();
}

bool receive_kot(oscpkt::Message *msg) {
	float f = 0.5;
	float val = 0.5;
	
	if(msg->match("/tmfx/klingontone-drive")) {
		msg->match("/tmfx/klingontone-drive").popFloat(f).isOkNoMoreArgs();
		val = torange(f, kot_map[0]);
		// printf("kot drive: %f -> %f\n", f, val);
		kot_set_drive(klingontone, val);
		return true;
	}
	if(msg->match("/tmfx/klingontone-tone")) {
		msg->match("/tmfx/klingontone-tone").popFloat(f).isOkNoMoreArgs();
		val = torange(f, kot_map[1]);
		// printf("kot tone: %f -> %f\n", f, val);
		kot_set_tone(klingontone, val);
		return true;
	}
	if(msg->match("/tmfx/klingontone-level")) {
		msg->match("/tmfx/klingontone-level").popFloat(f).isOkNoMoreArgs();
		val = torange(f, kot_map[2]);
		// printf("kot level: %f -> %f\n", f, val);
		kot_set_level(klingontone, val);
		return true;
	}
	if(msg->match("/tmfx/klingontone-boost")) {
		msg->match("/tmfx/klingontone-boost").popFloat(f).isOkNoMoreArgs();
		val = torange(f, kot_map[3]);
		// printf("kot boost: %f -> %f\n", f, val);
		kot_set_boost(klingontone, val);
		return true;
	}
	return false;
}

// ------------------------------------------

struct {
	overdrive *device;
	knob_t knobs[];
} overdrive_faceplate;

void setup_overdrive(overdrive *device) {
	overdrive_faceplate.device = device;
	auto knob = overdrive_faceplate.knobs;
	
	knob->id = 0;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 12.0;
	knob->map[3] = 45.0;
	knob->name = "gain";
	knob->path = "/tmfx/overdrive-gain";
	knob->value = &(device->gain);
}

void send_overdrive() {
	send_knobs(overdrive_faceplate.knobs, 1);
}

// ------------------------------------------

#define CHORUS_ENABLE   "/tmfx/chorus-enable"
#define CHORUS_RATE     "/tmfx/chorus-rate"
#define CHORUS_DEPTH    "/tmfx/chorus-depth"
#define CHORUS_WIDTH    "/tmfx/chorus-width"
#define CHORUS_PHASE    "/tmfx/chorus-phase"
#define CHORUS_FEEDBACK "/tmfx/chorus-feedback"
#define CHORUS_MIX      "/tmfx/chorus-mix"
parameter_map chorus_map[6] = {
	{ 0.0, 1.0,   0.1,   10.0 }, // RATE
	{ 0.0, 1.0, 0.001,   0.99 }, // DEPTH
	{ 0.0, 1.0, 0.001, 0.0025 }, // WIDTH
	{ 0.0, 1.0,  -1.0,    1.0 }, // PHASE
	{ 0.0, 1.0,   0.0,    1.0 }, // FEEDBACK
	{ 0.0, 1.0,   0.0,    1.0 }  // MIX
};
tflanger* chorus;

void enable_chorus(bool enable) {
	if (enable) {
		tflanger_setTrails(chorus, 1);
		tflanger_setFinalGain(chorus, 0.5);
	}
	else {
		tflanger_setTrails(chorus, 0);
		tflanger_setFinalGain(chorus, 0.0);
	}
}

bool receive_chorus(oscpkt::Message* msg) {
	float f = 0.5;
	float val = 0.5;
	
	if(msg->match(CHORUS_ENABLE)) {
		msg->match(CHORUS_ENABLE).popFloat(f).isOkNoMoreArgs();
		if (f < 1.0) enable_chorus(false);
		else enable_chorus(true);
		return true;
	}
	if(msg->match(CHORUS_MIX)) {
		msg->match(CHORUS_MIX).popFloat(f).isOkNoMoreArgs();
		val = torange(f, chorus_map[0]);
		printf("dampness: %f -> %f\n", f, val);
		tflanger_setWetDry(chorus, val);
		return true;
	}
	if (msg->match(CHORUS_RATE)) {
		msg->match(CHORUS_RATE).popFloat(f).isOkNoMoreArgs();
		val = torange(f, chorus_map[1]);
		printf("fader1: %f rate: %f\n", f, val);
		tflanger_setLfoRate(chorus, val);
		return true;
	}
	if (msg->match(CHORUS_DEPTH)) {
		msg->match(CHORUS_DEPTH).popFloat(f).isOkNoMoreArgs();
		val = torange(f, chorus_map[2]);
		printf("depth: %f -> %f\n", f, val);
		// can sometimes blow everything up
		tflanger_setLfoDepth(chorus, val);
		return true;
	}
	if (msg->match(CHORUS_WIDTH)) {
		msg->match(CHORUS_WIDTH).popFloat(f).isOkNoMoreArgs();
		val = torange(f, chorus_map[3]);
		printf("width: %f -> %f\n", f, val);
		tflanger_setLfoWidth(chorus, val);
	}
	if (msg->match(CHORUS_PHASE)) {
		msg->match(CHORUS_PHASE).popFloat(f).isOkNoMoreArgs();
		val = torange(f, chorus_map[4]);
		printf("damping: %f -> %f\n", f, val);
		tflanger_setLfoPhase(chorus, val);
		return true;
	}
	if (msg->match(CHORUS_FEEDBACK)) {
		msg->match(CHORUS_FEEDBACK).popFloat(f).isOkNoMoreArgs();
		val = torange(f, chorus_map[5]);
		printf("feedback: %f -> %f\n", f, val);
		tflanger_setFeedBack(chorus, val);
		return true;
	}
	return false;
}

void send_chorus() {
	// set TouchOSC knobs to current parameter values.
	printf("chorus is initially %f %f %f %f %f %f\n",
		chorus->wet,
		chorus->lfoRate,
		chorus->lfoDepth,  
		chorus->lfoWidth,
		chorus->lfoPhase,
		chorus->feedBack);
	sender.newMessage(CHORUS_RATE).add(todomain(chorus->lfoRate, chorus_map[0])).send();
	sender.newMessage(CHORUS_DEPTH).add(todomain(chorus->lfoDepth, chorus_map[1])).send();
	sender.newMessage(CHORUS_WIDTH).add(todomain(chorus->lfoWidth, chorus_map[2])).send();
	sender.newMessage(CHORUS_PHASE).add(todomain(chorus->lfoPhase, chorus_map[3])).send();
	sender.newMessage(CHORUS_FEEDBACK).add(todomain(chorus->feedBack, chorus_map[4])).send();
	sender.newMessage(CHORUS_MIX).add(todomain(chorus->wet, chorus_map[5])).send();
}

void setup_chorus(tflanger *device) {
	chorus = device;
	enable_chorus(false);
	send_chorus();
}

// -----------------------------------------

void setup_reverb(Reverb* device) {
	reverb = device;
	// Make silent?
	reverb->set_opmix(0.0);
}

// -----------------------------------------

void on_receive(oscpkt::Message* msg, void* arg) {
	printf("%s\n", msg->addressPattern().c_str());
	receive_chorus(msg);
}

void setup(
	feedback_compressor* cmp,
	Sustainer* sus,
	klingon *kot,
	::overdrive *ovd,
	tflanger *cho,
	Reverb *rev)
{
	// Only chorusing ATM!
	receiver.setup(localport, on_receive);
	sender.setup(remoteport, remoteip);
	setup_chorus(cho);
}

void teardown() {}

}
