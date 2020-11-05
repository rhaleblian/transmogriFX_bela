/***** osc_delay.h *****/

#pragma once

#include "flange.h"
#include "osc.h"

namespace delay {
	
class Faceplate {
	public:
	tflanger *m_effect;
	bool m_enable;
	osc::knob_t m_depth;
	osc::knob_t m_width;
	osc::knob_t m_phase;
	osc::knob_t m_regen;
	osc::knob_t m_damp;
	osc::knob_t m_mix;
	Faceplate();
	void enable();
	void disable();
	bool receive(oscpkt::Message *msg);
	void send();
	void setup(tflanger *effect);
};

}
