/***** osc.cpp *****/
#include <stdio.h>
#include <libraries/OscSender/OscSender.h>
#include <libraries/OscReceiver/OscReceiver.h>
#include "osc.h"

OscReceiver oscReceiver;
OscSender oscSender;
int localPort = 59588;
int remotePort = 9000;
const char* remoteIp = "192.168.1.65";

struct {
	klingon *kot;
	tflanger *chorus;
} osc_s;

void on_receive(oscpkt::Message* msg, void* arg) {
	float f;
	if(msg->match("/3/rotary1")) {
		msg->match("/3/rotary1").popFloat(f).isOkNoMoreArgs();
		printf("rotary1: %f\n", f);
	} else if (msg->match("/3/fader1")) {
		msg->match("/3/fader1").popFloat(f).isOkNoMoreArgs();
		printf("fader1: %f\n", f);
		// f = map(f, 0, 1.0, 0.005, 0.035);
		tflanger_setLfoRate(osc_s.chorus, f);
	} else if (msg->match("/3/fader2")) {
		msg->match("/3/fader2").popFloat(f).isOkNoMoreArgs();
		printf("fader2: %f\n", f);
		tflanger_setLfoDepth(osc_s.chorus, f);
	} else
		printf("%s\n", msg->addressPattern().c_str());
}

void osc_setup()
{
	oscReceiver.setup(localPort, on_receive);
	oscSender.setup(remotePort, remoteIp);
}

void osc_set_knobs(klingon *kot, tflanger *chorus) {
	osc_s.kot = kot;
	osc_s.chorus = chorus;

	kot_set_drive(osc_s.kot, 30.0);
	kot_set_tone(osc_s.kot, -20.0);
	kot_set_level(osc_s.kot, -20.0);
	kot_set_boost(osc_s.kot, 0.5);

	tflanger_setLfoRate(osc_s.chorus, 0.5);
	tflanger_setLfoDepth(osc_s.chorus, 4.0);
}
