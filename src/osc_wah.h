/***** osc_wah.h *****/

#pragma once

#include "inductorwah.h"
#include "osc.h"

namespace wah {
	
class Faceplate {
	public:
	iwah_t *m_effect;
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
	void setup(iwah_t *effect);
};

}
