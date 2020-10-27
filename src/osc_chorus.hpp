/***** osc_chorus.hpp *****/
// included by osc.cpp

#define CHORUS_ENABLE   "/tmfx/chorus-enable"
#define CHORUS_MIX      "/tmfx/chorus-mix"
#define CHORUS_RATE     "/tmfx/chorus-rate"
#define CHORUS_DEPTH    "/tmfx/chorus-depth"
#define CHORUS_WIDTH    "/tmfx/chorus-width"
#define CHORUS_PHASE    "/tmfx/chorus-phase"
#define CHORUS_FEEDBACK "/tmfx/chorus-feedback"
parameter_map chorus_map[6] = {
	{ 0.0, 1.0,   0.0,    1.0 }, // MIX
	{ 0.0, 1.0,   0.1,   10.0 }, // RATE
	{ 0.0, 1.0, 0.001,   0.99 }, // DEPTH
	{ 0.0, 1.0, 0.001, 0.0025 }, // WIDTH
	{ 0.0, 1.0,  -1.0,    1.0 }, // PHASE
	{ 0.0, 1.0,   0.0,    1.0 }  // FEEDBACK
};
tflanger* chorus;

void send_chorus() {
	// set TouchOSC knobs to current parameter values.
	printf("chorus is initially %f %f %f %f %f %f\n",
		chorus->wet,
		chorus->lfoRate,
		chorus->lfoDepth,  
		chorus->lfoWidth,
		chorus->lfoPhase,
		chorus->feedBack);
	sender.newMessage(CHORUS_MIX).add(todomain(chorus->wet, chorus_map[0])).send();
	sender.newMessage(CHORUS_RATE).add(todomain(chorus->lfoRate, chorus_map[1])).send();
	sender.newMessage(CHORUS_DEPTH).add(todomain(chorus->lfoDepth, chorus_map[2])).send();
	sender.newMessage(CHORUS_WIDTH).add(todomain(chorus->lfoWidth, chorus_map[3])).send();
	sender.newMessage(CHORUS_PHASE).add(todomain(chorus->lfoPhase, chorus_map[4])).send();
	sender.newMessage(CHORUS_FEEDBACK).add(todomain(chorus->feedBack, chorus_map[5])).send();
}

void enable_chorus(bool enable) {
	if (enable) {
		tflanger_setTrails(chorus, 1);
		tflanger_setFinalGain(chorus, 0.5);
	}
	else {
		tflanger_setTrails(chorus, 0);
		tflanger_setFinalGain(chorus, 0.0);
	}
}

bool receive_chorus(oscpkt::Message* msg) {
	float f = 0.5;
	float val = 0.5;
	
	if(msg->match(CHORUS_ENABLE)) {
		msg->match(CHORUS_ENABLE).popFloat(f).isOkNoMoreArgs();
		if (f < 1.0) enable_chorus(false);
		else enable_chorus(true);
		return true;
	}
	if(msg->match(CHORUS_MIX)) {
		msg->match(CHORUS_MIX).popFloat(f).isOkNoMoreArgs();
		val = torange(f, chorus_map[0]);
		printf("dampness: %f -> %f\n", f, val);
		tflanger_setWetDry(chorus, val);
		return true;
	}
	if (msg->match(CHORUS_RATE)) {
		msg->match(CHORUS_RATE).popFloat(f).isOkNoMoreArgs();
		val = torange(f, chorus_map[1]);
		printf("fader1: %f rate: %f\n", f, val);
		tflanger_setLfoRate(chorus, val);
		return true;
	}
	if (msg->match(CHORUS_DEPTH)) {
		msg->match(CHORUS_DEPTH).popFloat(f).isOkNoMoreArgs();
		val = torange(f, chorus_map[2]);
		printf("depth: %f -> %f\n", f, val);
		// can sometimes blow everything up
		tflanger_setLfoDepth(chorus, val);
		return true;
	}
	if (msg->match(CHORUS_WIDTH)) {
		msg->match(CHORUS_WIDTH).popFloat(f).isOkNoMoreArgs();
		val = torange(f, chorus_map[3]);
		printf("width: %f -> %f\n", f, val);
		tflanger_setLfoWidth(chorus, val);
	}
	if (msg->match(CHORUS_PHASE)) {
		msg->match(CHORUS_PHASE).popFloat(f).isOkNoMoreArgs();
		val = torange(f, chorus_map[4]);
		printf("damping: %f -> %f\n", f, val);
		tflanger_setLfoPhase(chorus, val);
		return true;
	}
	if (msg->match(CHORUS_FEEDBACK)) {
		msg->match(CHORUS_FEEDBACK).popFloat(f).isOkNoMoreArgs();
		val = torange(f, chorus_map[5]);
		printf("feedback: %f -> %f\n", f, val);
		tflanger_setFeedBack(chorus, val);
		return true;
	}
	return false;
}

void setup_chorus(tflanger *device) {
	chorus = device;
	enable_chorus(false);
	send_chorus();
}
