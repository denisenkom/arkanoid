#include "a_shared.h"
#include "Server.h"
#include "Client.h"
#include "Host.h"
#include "Controls.h"
#include "log.h"
#include "Realtime.h"

extern double Host::frametime = 0;
//extern int Host::exit = 0;

//static double time, oldtime = 0;

/*void Host::Frame() {
	time = RealTime::getTime();
	double dt = time - oldtime;

	if (dt > 0.1)
		dt = 0.1;
	else if (dt < 0.001)
		dt = 0.001;

	if (Controls::keys[KEY_ESC])
		exit = 1;
	else
		exit = 0;

	Host::frametime = dt;
	//Host::time += dt;

	//double time1 = RealTime::getTime();
	Server::Frame();
	//double time2 = RealTime::getTime();
	Client::Frame();
	//double time3 = RealTime::getTime();
	//Log_Print("server %.3f : client %.3f\n", time2-time1, time3-time2);
	oldtime = time;
}*/
