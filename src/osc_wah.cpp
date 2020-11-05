/***** osc_wah.cpp *****/
/***** osc_delay.cpp *****/
#include <Bela.h>
#include "osc_wah.h"

using namespace osc;

namespace wah {

Faceplate::Faceplate() {
	m_effect = nullptr;
	m_enable = false;
}

void Faceplate::disable() {
	iwah_bypass(m_effect, true);
	m_enable = false;
}

void Faceplate::enable() {
	iwah_bypass(m_effect, false);
	m_enable = true;
}

bool Faceplate::receive(oscpkt::Message* msg) {
	float f = 0.5;

	if(msg->match("/wah/enable")) {
		msg->match("/wah/enable").popFloat(f).isOkNoMoreArgs();
		f > 0.0 ? enable() : disable();
		return true;
	}

	return false;
}

void Faceplate::send() {
	// set TouchOSC knobs to current parameter values.
	send_address_value("/wah/enable", 0.0);
}

void Faceplate::setup(iwah_t *effect) {
	m_effect = effect;
	
	disable();

	send();
}

}
