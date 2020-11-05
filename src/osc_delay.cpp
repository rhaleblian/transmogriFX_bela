/***** osc_delay.cpp *****/
#include <Bela.h>
#include "osc_delay.h"

using namespace osc;

namespace delay {

Faceplate::Faceplate() {
	m_effect = nullptr;
	m_enable = false;
}

void Faceplate::disable() {
	tflanger_setTrails(m_effect, 0);
	tflanger_setFinalGain(m_effect, 0.0);
	m_enable = false;
}

void Faceplate::enable() {
	tflanger_setTrails(m_effect, 1);
	tflanger_setFinalGain(m_effect, 1.0);
	m_enable = true;
}

bool Faceplate::receive(oscpkt::Message* msg) {
	float f = 0.5;

	if(msg->match("/delay/enable")) {
		msg->match("/delay/enable").popFloat(f).isOkNoMoreArgs();
		f > 0.0 ? enable() : disable();
		return true;
	}

	auto knob = &m_depth;
	if (msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		tflanger_setLfoDepth(m_effect, knob->value);
		return true;
	}

	knob = &m_width;
	if (msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		tflanger_setLfoWidth(m_effect, knob->value);
		return true;
	}

	knob = &m_regen;
	if (msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		tflanger_setFeedBack(m_effect, knob->value);
		return true;
	}

	knob = &m_damp;
	if (msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		tflanger_setDamping(m_effect, knob->value);
		return true;
	}

	knob = &m_mix;
	if (msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		tflanger_setWetDry(m_effect, knob->value);
		return true;
	}

	return false;
}

void Faceplate::send() {
	// set TouchOSC knobs to current parameter values.
	send_address_value("/delay/enable", 0.0);
	send_knob(m_depth);
	send_knob(m_width);
	send_knob(m_regen);
	send_knob(m_damp);
	send_knob(m_mix);
}

void Faceplate::setup(tflanger *effect) {
	m_effect = effect;
	
	disable();

	auto knob = &m_depth;
	int id = 0;
	knob->address = "/delay/depth";
	knob->id = id++;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 0.01;
	knob->map[3] = 0.99;
	knob->value = 0.05;
	tflanger_setLfoDepth(m_effect, knob->value);

	knob = &m_width;
	knob->address = "/delay/width";
	knob->id = id++;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 0.001;
	knob->map[3] = 0.0025;
	knob->value = 0.0015;
	tflanger_setLfoWidth(m_effect, knob->value);

	knob = &m_regen;
	knob->address = "/delay/regen";
	knob->id = id++;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 0.0;
	knob->map[3] = 1.0;
	knob->value = 0.001;
	tflanger_setFeedBack(m_effect, knob->value);

	knob = &m_damp;
	knob->address = "/delay/damp";
	knob->id = id++;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 40.0;
	knob->map[3] = 7400.0;
	knob->value = 100.0;
	tflanger_setDamping(m_effect, knob->value);

	knob = &m_mix;
	knob->address = "/delay/mix";
	knob->id = id++;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 0.0;
	knob->map[3] = 1.0;
	knob->value = 0.5;
	tflanger_setWetDry(m_effect, knob->value);

	send();
}

}
