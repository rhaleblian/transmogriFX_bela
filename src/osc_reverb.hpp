/***** osc_reverb.hpp *****/
// inlined by osc.cpp -- take note re scoping etc!

struct {
	knob_t mix;
	Reverb *effect;	
} reverb_faceplate;

bool receive_reverb(oscpkt::Message* msg) {
	float f = 0.5;

	auto knob = &reverb_faceplate.mix;
	if(msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		reverb_faceplate.effect->set_opmix(knob->value);
		return true;
	}
	return false;
}

void send_reverb() {
	send_knob(reverb_faceplate.mix);
}

void setup_reverb(Reverb* effect) {
	reverb_faceplate.effect = effect;
	
	auto knob = &reverb_faceplate.mix;
	knob->address = "/reverb/mix";
	knob->id = 4;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 0.0;
	knob->map[3] = 1.0;
	knob->value = 0.0;
	// Make silent with mix. No toggleswitch yet...
	reverb_faceplate.effect->set_opmix(knob->value);
}
