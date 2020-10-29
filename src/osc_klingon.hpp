/***** osc_klingon.hpp *****/
// osc.cpp inline include -- note this when scoping names etc.

struct {
	knob_t drive;
	knob_t tone;
	knob_t level;
	knob_t boost;
	klingon *effect;
} klingon_faceplate;

void send_klingon() {
	// set TouchOSC knobs to current parameter values.
	send_address_value("/klingon/enable", 0.0);
	send_knob(klingon_faceplate.drive);
	send_knob(klingon_faceplate.tone);
	send_knob(klingon_faceplate.level);
	send_knob(klingon_faceplate.boost);
}

void enable_klingon(bool enable=true) {
	kot_set_bypass(klingon_faceplate.effect, !enable);
}

bool receive_klingon(oscpkt::Message *msg) {
	float f = 0.5;
	auto effect = klingon_faceplate.effect;
	
	if(msg->match("/klingon/enable")) {
		msg->match("/klingon/enable").popFloat(f).isOkNoMoreArgs();
		enable_klingon(f > 0.0);
		return true;
	}
	auto knob = &klingon_faceplate.drive;
	if(msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		kot_set_drive(effect, knob->value);
		return true;
	}
	knob = &klingon_faceplate.tone;
	if(msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		kot_set_tone(effect, knob->value);
		return true;
	}
	knob = &klingon_faceplate.level;
	if(msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		kot_set_level(effect, knob->value);
		return true;
	}
	knob = &klingon_faceplate.boost;
	if(msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		kot_set_boost(effect, knob->value);
		return true;
	}
	return false;
}

void setup_klingon(klingon *effect) {
	klingon_faceplate.effect = effect;

	knob_t *knob = &klingon_faceplate.drive;
	knob->address = "/klingon/drive";
	knob->id = 0;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 0.0;
	knob->map[3] = 1.0;
	knob->value = 0.5;
	kot_set_drive(effect, knob->value);

	knob = &klingon_faceplate.tone;
	knob->address = "/klingon/tone";
	knob->id = 1;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = -40.0;
	knob->map[3] = 0.0;
	knob->value = -5.0;
	kot_set_tone(effect, knob->value);

	knob = &klingon_faceplate.level;
	knob->address = "/klingon/level";
	knob->id = 2;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = -40.0;
	knob->map[3] = 0.0;
	knob->value = 0.0;
	kot_set_level(effect, knob->value);

	knob = &klingon_faceplate.boost;
	knob->address = "/klingon/boost";
	knob->id = 3;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 0.01;
	knob->map[3] = 0.99;
	knob->value = 0.5;
	kot_set_boost(effect, knob->value);

	send_klingon();
}
