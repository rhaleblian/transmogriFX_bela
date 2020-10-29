/***** osc_compressor.hpp *****/

struct {
	knob_t knobs[6];
	feedback_compressor* effect;
} compressor_faceplate;

bool receive_compressor(oscpkt::Message *msg) {
	float f = 0.5;
	auto effect = compressor_faceplate.effect;

	if (msg->match("/compressor/enable")) {
		msg->match("/compressor/enable").popFloat(f).isOkNoMoreArgs();
		feedback_compressor_set_bypass(effect, f < 1.0);
		return true;
	}
	knob_t *knob = compressor_faceplate.knobs;
	if(msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		feedback_compressor_set_threshold(compressor_faceplate.effect, knob->value);
		return true;
	}
	knob++;
	if(msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		feedback_compressor_set_ratio(compressor_faceplate.effect, knob->value);
		return true;
	}
	knob++;
	if(msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		feedback_compressor_set_out_gain(compressor_faceplate.effect, knob->value);
		return true;
	}
	knob++;
	if(msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		feedback_compressor_set_mix(compressor_faceplate.effect, knob->value);
		return true;
	}
	knob++;
	if(msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		feedback_compressor_set_attack(compressor_faceplate.effect, knob->value);
		return true;
	}
	knob++;
	if(msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		feedback_compressor_set_release(compressor_faceplate.effect, knob->value);
		return true;
	}
	
	return false;
}

void setup_compressor(feedback_compressor *effect) {
	// Most of this could be done at compile-time.
	compressor_faceplate.effect = effect;
	
	auto knob = compressor_faceplate.knobs;
	knob->address = "/compressor/threshold";
	knob->id = 0;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = -60.0;
	knob->map[3] = -12.0;
	knob->value = 0.5;
	feedback_compressor_set_threshold(effect, knob->value);

	knob++;
	knob->address = "/compressor/ratio";
	knob->id = 1;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 1.0;
	knob->map[3] = 24.0;
	knob->value = 0.5;
	feedback_compressor_set_threshold(effect, knob->value);

	knob++;
	knob->id = 2;
	knob->address = "/compressor/level";
	knob->value = 0.5;
	feedback_compressor_set_threshold(effect, knob->value);

	knob++;
	knob->id = 3;
	knob->address = "/compressor/mix";
	knob->value = 0.5;
	feedback_compressor_set_threshold(effect, knob->value);

	knob++;
	knob->id = 4;
	knob->address = "/compressor/attack";
	knob->value = 0.5;
	feedback_compressor_set_threshold(effect, knob->value);

	knob++;
	knob->id = 5;
	knob->address = "/compressor/release";
	knob->value = 0.5;
	feedback_compressor_set_threshold(effect, knob->value);

	for (int i=0; i<6; i++)
		send_knob(compressor_faceplate.knobs[i]);
}
