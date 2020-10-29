/***** osc_chorus.hpp *****/
// inlined by osc.cpp -- consider scoping etc accordingly.

struct {
	tflanger *effect;
	bool enable;
	knob_t rate;
	knob_t depth;
	knob_t width;
	knob_t phase;
	knob_t feedback;
} chorus_faceplate;

void enable_chorus(bool enable) {
	if (enable) {
		tflanger_setTrails(chorus_faceplate.effect, 1);
		tflanger_setFinalGain(chorus_faceplate.effect, 0.5);
	}
	else {
		tflanger_setTrails(chorus_faceplate.effect, 0);
		tflanger_setFinalGain(chorus_faceplate.effect, 0.0);
	}
	chorus_faceplate.enable = enable;
}

bool receive_chorus(oscpkt::Message* msg) {
	float f = 0.5;
	auto effect = chorus_faceplate.effect;
	
	if(msg->match("/chorus/enable")) {
		msg->match("/chorus/enable").popFloat(f).isOkNoMoreArgs();
		enable_chorus(f > 0.0);
		return true;
	}

	auto knob = &chorus_faceplate.rate;
	if (msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		tflanger_setLfoRate(effect, knob->value);
		return true;
	}

	knob = &chorus_faceplate.depth;
	if (msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		tflanger_setLfoDepth(effect, knob->value);
		return true;
	}

	knob = &chorus_faceplate.width;
	if (msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		tflanger_setLfoWidth(effect, knob->value);
		return true;
	}

	knob = &chorus_faceplate.phase;
	if (msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		tflanger_setLfoPhase(effect, knob->value);
		return true;
	}

	knob = &chorus_faceplate.feedback;
	if (msg->match(knob->address)) {
		msg->match(knob->address).popFloat(f).isOkNoMoreArgs();
		knob->value = torange(f, knob->map);
		tflanger_setFeedBack(effect, knob->value);
		return true;
	}

	return false;
}

void send_chorus() {
	// set TouchOSC knobs to current parameter values.
	send_address_value("/chorus/enable", 0.0);
	send_knob(chorus_faceplate.rate);
	send_knob(chorus_faceplate.depth);
	send_knob(chorus_faceplate.width);
	send_knob(chorus_faceplate.phase);
	send_knob(chorus_faceplate.feedback);
}

void setup_chorus(tflanger *effect) {
	chorus_faceplate.effect = effect;
	
	enable_chorus(false);

	auto knob = &chorus_faceplate.rate;
	knob->address = "/chorus/rate";
	knob->id = 0;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 0.1;
	knob->map[3] = 10.0;
	knob->value = 1.0;
	tflanger_setLfoRate(effect, knob->value);

	knob = &chorus_faceplate.depth;
	knob->address = "/chorus/depth";
	knob->id = 1;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 0.01;
	knob->map[3] = 0.99;
	knob->value = 0.05;
	tflanger_setLfoDepth(effect, knob->value);

	knob = &chorus_faceplate.width;
	knob->address = "/chorus/width";
	knob->id = 2;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 0.001;
	knob->map[3] = 0.0025;
	knob->value = 0.0015;
	tflanger_setLfoWidth(effect, knob->value);

	knob = &chorus_faceplate.phase;
	knob->address = "/chorus/phase";
	knob->id = 3;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = -1.0;
	knob->map[3] = 1.0;
	knob->value = 0.0;
	tflanger_setLfoPhase(effect, knob->value);

	knob = &chorus_faceplate.feedback;
	knob->address = "/chorus/feedback";
	knob->id = 4;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 0.0;
	knob->map[3] = 1.0;
	knob->value = 0.1;
	tflanger_setFeedBack(effect, knob->value);

	send_chorus();
}
