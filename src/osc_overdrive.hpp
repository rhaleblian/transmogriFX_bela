/***** osc_overdrive.hpp *****/
struct {
	overdrive *device;
	knob_t knobs[];
} overdrive_faceplate;

void setup_overdrive(overdrive *device) {
	overdrive_faceplate.device = device;
	auto knob = overdrive_faceplate.knobs;
	
	knob->id = 0;
	knob->map[0] = 0.0;
	knob->map[1] = 1.0;
	knob->map[2] = 12.0;
	knob->map[3] = 45.0;
	knob->name = "gain";
	knob->path = "/tmfx/overdrive-gain";
	knob->value = &(device->gain);
}

void send_overdrive() {
	send_knobs(overdrive_faceplate.knobs, 1);
}
