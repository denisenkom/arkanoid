#include "RealTime.h"
#include <windows.h>

double RealTime::_freq = 0;
double RealTime::_time_offset = 0;
bool RealTime::_initialized = false;

void RealTime::_init() {
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	_freq = (double)freq.QuadPart;
	_initialized = true;
}

double RealTime::getTime() {
	if (!_initialized) _init();
	LARGE_INTEGER _time;
	QueryPerformanceCounter(&_time);
	return _time.QuadPart/_freq - _time_offset;
}

void RealTime::setTime(double time) {
	_time_offset = getTime() - time;
}

void RealTime::wait(double delay_time) {
	if (!_initialized) _init();
	double end_time = getTime() + delay_time;
	while (getTime() < end_time);
}
