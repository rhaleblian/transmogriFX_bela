/***** osc_sustainer.hpp *****/
/***** osc_sustainer.hpp *****/

struct {
	Sustainer* effect;
	knob_t gain;
	knob_t sustain;
	bool enable;
} sustainer_faceplate;

bool receive_sustainer(oscpkt::Message *msg) {
	float f = 0.5;
	auto effect = sustainer_faceplate.effect;
	
	if (msg->match("/sustainer/enable")) {
		msg->match("/sustainer/enable").popFloat(f).isOkNoMoreArgs();
		effect->setBypass();  // This is actually a toggle.
		return true;
	}
	knob_t *knob = &sustainer_faceplate.gain;
	if(msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		effect->setGain(knob->value);
		return true;
	}
	knob = &sustainer_faceplate.sustain;
	if(msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		effect->setSustain(knob->value);
		return true;
	}
	return false;
}

void setup_sustainer(Sustainer *effect) {
	sustainer_faceplate.effect = effect;
	
	sustainer_faceplate.enable = false;
	send_address_value("/sustainer/enable", 0.0);
	
	auto knob = &sustainer_faceplate.gain;
	knob->address = "/sustainer/gain";
	knob->id = 0;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 0.0;
	knob->map[3] = 1.0;
	knob->value = 0.5;
	effect->setGain(knob->value);

	knob = &sustainer_faceplate.sustain;
	knob->address = "/sustainer/sustain";
	knob->id = 1;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 0.0;
	knob->map[3] = 1.0;
	knob->value = 0.5;
	effect->setSustain(knob->value);

	send_knob(sustainer_faceplate.gain);
	send_knob(sustainer_faceplate.sustain);
}
