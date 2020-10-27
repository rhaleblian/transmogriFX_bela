/***** osc_compressor.hpp *****/

enum compressor_knob_id {
	COMPRESSOR_THRESHOLD,
	COMPRESSOR_RATIO,
	COMPRESSOR_LEVEL,
	COMPRESSOR_MIX,
	COMPRESSOR_ATTACK,
	COMPRESSOR_RELEASE
};
parameter_map compressor_map[] = {
	{ 0.0, 1.0, -60.0, -12.0 },  // THRESHOLD
	{ 0.0, 1.0, 1.0,    24.0 },  // RATIO
	{ 0.0, 1.0, -24.0,   6.0 },  // LEVEL
	{ 0.0, 1.0, 0.0,     1.0 },  // MIX
	{ 0.0, 1.0, 1.0,  1000.0 },  // ATTACK
	{ 0.0, 1.0, 20.0, 1000.0 }   // RELEASE
};
knob_t compressor_knobs[6];
feedback_compressor* compressor;

void setup_compressor(feedback_compressor *device) {
	// Most of this could be done at compile-time.
	compressor = device;
	
	auto knob = compressor_knobs;
	knob->id = COMPRESSOR_THRESHOLD;
	knob->name = "threshold";
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = -60.0;
	knob->map[3] = -12.0;
	knob->path = "/tmfx/compressor-threshold";
	knob->value = &(compressor->threshold_db);

	knob++;
	knob->id = COMPRESSOR_RATIO;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 1.0;
	knob->map[3] = 24.0;
	knob->name = "ratio";
	knob->path = "/tmfx/compressor-ratio";
	knob->value = &(compressor->ratio);

	knob++;
	knob->id = COMPRESSOR_LEVEL;
	knob->name = "level";
	knob->path = "/tmfx/compressor-level";
	knob->value = &(compressor->ratio);

	knob++;
	knob->id = COMPRESSOR_MIX;
	knob->name = "mix";
	knob->path = "/tmfx/compressor-mix";
	knob->value = &(compressor->ratio);

	knob++;
	knob->id = COMPRESSOR_ATTACK;
	knob->name = "attack";
	knob->path = "/tmfx/compressor-attack";
	knob->value = &(compressor->atk);

	knob++;
	knob->id = COMPRESSOR_RELEASE;
	knob->name = "release";
	knob->path = "/tmfx/compressor-release";
	knob->value = &(compressor->rls);

	send_knobs(compressor_knobs, 1);
}

void write_compressor(unsigned int id, float val) {
	switch (id) {
		case COMPRESSOR_THRESHOLD:
		feedback_compressor_set_threshold(compressor, val);
		break;
		case COMPRESSOR_RATIO:
		feedback_compressor_set_ratio(compressor, val);
		break;
	}
}

bool receive_compressor(oscpkt::Message *msg) {
	float f = 0.5;
	float v = 0.5;

	if (msg->match("/tmfx/compressor-enable")) {
		msg->match("/tmfx/compressor-enable").popFloat(f).isOkNoMoreArgs();
		compressor->bypass = f > 0.0 ? false: true;
		return true;
	} else {
		for (knob_t *knob = compressor_knobs;
			knob != compressor_knobs + 2;
			knob++) {
			if(msg->match(knob->name)) {
				msg->match(knob->name).popFloat(f).isOkNoMoreArgs();
				v = torange(f, knob->map);
				write_compressor(knob->id, v);
				return true;
			}
		}
	}
	return false;
}
