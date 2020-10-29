/***** osc_overdrive.hpp *****/

struct {
	overdrive *effect;
	knob_t drive;
	knob_t tone;
	knob_t level;
} overdrive_faceplate;

void enable_overdrive(bool enable) {
	od_set_bypass(overdrive_faceplate.effect, !enable);
}

bool receive_overdrive(oscpkt::Message* msg) {
	float f = 0.5;
	auto effect = overdrive_faceplate.effect;
    if(msg->match("/overdrive/enable")) {
        msg->match("/overdrive/enable").popFloat(f).isOkNoMoreArgs();
		enable_overdrive(f > 0.0);
		return true;
    }
    auto knob = &overdrive_faceplate.drive;
    if(msg->match(knob->address)) {
        msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
        knob->value = torange(f, knob->map);
		od_set_drive(effect, knob->value);
		return true;
    }
    knob = &overdrive_faceplate.tone;
    if(msg->match(knob->address)) {
        msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
        knob->value = torange(f, knob->map);
		od_set_tone(effect, knob->value);
    	return true;
    }
    knob = &overdrive_faceplate.level;
    if(msg->match(knob->address)) {
        msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
        knob->value = torange(f, knob->map);
		od_set_level(effect, knob->value);
		return true;
    }
	return false;
}

void send_overdrive() {
	send_address_value("/overdrive/enable", 0.0);
	send_knob(overdrive_faceplate.drive);
	send_knob(overdrive_faceplate.tone);
	send_knob(overdrive_faceplate.level);
}

void setup_overdrive(overdrive *effect) {
	overdrive_faceplate.effect = effect;

	auto knob = &overdrive_faceplate.drive;
	knob->address = "/overdrive/drive";
	knob->id = 0;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 12.0;
	knob->map[3] = 45.0;
	knob->value = 36.0;
	od_set_drive(effect, knob->value);

	knob = &overdrive_faceplate.tone;
	knob->address = "/overdrive/tone";
	knob->id = 1;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = -12.0;
	knob->map[3] = 12.0;
	knob->value = 4.0;
	od_set_tone(effect, knob->value);

	knob = &overdrive_faceplate.level;
	knob->address = "/overdrive/level";
	knob->id = 2;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = -40.0;
	knob->map[3] = 6.0;
	knob->value = -20.0;
	od_set_level(effect, knob->value);

	send_overdrive();
}
