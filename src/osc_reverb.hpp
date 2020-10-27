/***** osc_reverb.hpp *****/
// included by osc.cpp

void setup_reverb(Reverb* device) {
	reverb = device;
	// Make silent?
	reverb->set_opmix(0.0);
}
	
bool receive_reverb(oscpkt::Message* msg) {
	float f = 0.5;
	// float val = 0.5;
	
	if(msg->match("/reverb/mix")) {
		msg->match("/reverb/mix").popFloat(f).isOkNoMoreArgs();
		reverb->set_opmix(f);
	}
	return false;
}
