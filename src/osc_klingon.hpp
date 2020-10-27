/***** osc_klingon.hpp *****/
// osc.cpp non-header include
// note this when scoping names etc.

#define KLINGON_ENABLE "/klingon/enable"

parameter_map kot_map[4] = {
	{ 0.0, 1.0,  12.0, 45.0 },   // DRIVE
	{ 0.0, 1.0, -40.0,  0.0 },   // TONE
	{ 0.0, 1.0, -40.0,  0.0 },   // LEVEL
	{ 0.0, 1.0,  0.01, 0.99 }    // BOOST
};
klingon *klingontone;

void send_klingontone() {
	// set TouchOSC knobs to current parameter values.
	printf("klingon is initially %f %f %f %f\n",
		klingontone->gain,
		klingontone->tone,
		klingontone->level,
		klingontone->hard);
	sender.newMessage("/klingontone/drive")
	.add(todomain(klingontone->gain, kot_map[0])).send();
	sender.newMessage("/klingontone/tone")
	.add(todomain(klingontone->tone, kot_map[1])).send();
	sender.newMessage("/klingontone/level")
	.add(todomain(klingontone->level, kot_map[2])).send();
	sender.newMessage("/klingontone/boost")
	.add(todomain(klingontone->hard, kot_map[3])).send();
}

void enable_klingon(bool enable=true) {
	kot_set_bypass(klingontone, !enable);
}

void setup_klingontone(klingon *device) {
	klingontone = device;
	send_klingontone();
}

bool receive_klingontone(oscpkt::Message *msg) {
	float f = 0.5;
	float val = 0.5;

	if(msg->match(KLINGON_ENABLE)) {
		msg->match(KLINGON_ENABLE).popFloat(f).isOkNoMoreArgs();
		if (f > 0.0) enable_klingon();
		else enable_klingon(false);
		return true;
	}
	if(msg->match("/klingon/drive")) {
		msg->match("/klingon/drive").popFloat(f).isOkNoMoreArgs();
		val = torange(f, kot_map[0]);
		// printf("kot drive: %f -> %f\n", f, val);
		kot_set_drive(klingontone, val);
		return true;
	}
	if(msg->match("/klingon/tone")) {
		msg->match("/klingon/tone").popFloat(f).isOkNoMoreArgs();
		val = torange(f, kot_map[1]);
		// printf("kot tone: %f -> %f\n", f, val);
		kot_set_tone(klingontone, val);
		return true;
	}
	if(msg->match("/klingon/level")) {
		msg->match("/klingon/level").popFloat(f).isOkNoMoreArgs();
		val = torange(f, kot_map[2]);
		// printf("kot level: %f -> %f\n", f, val);
		kot_set_level(klingontone, val);
		return true;
	}
	if(msg->match("/klingon/boost")) {
		msg->match("/klingontone/boost").popFloat(f).isOkNoMoreArgs();
		val = torange(f, kot_map[3]);
		// printf("kot boost: %f -> %f\n", f, val);
		kot_set_boost(klingontone, val);
		return true;
	}
	return false;
}
