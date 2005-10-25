#include "StdAfx.h"
#pragma hdrstop

#include "RealTime.h"

float RealTime::_freq = 0;
float RealTime::_time_offset = 0;
bool RealTime::_initialized = false;

void RealTime::_init() {
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	_freq = (float)freq.QuadPart;
	_initialized = true;
}

float RealTime::getTime() {
	if (!_initialized) _init();
	LARGE_INTEGER _time;
	QueryPerformanceCounter(&_time);
	return _time.QuadPart/_freq - _time_offset;
}

void RealTime::setTime(float time) {
	_time_offset = getTime() - time;
}

void RealTime::wait(float delay_time) {
	if (!_initialized) _init();
	float end_time = getTime() + delay_time;
	while (getTime() < end_time);
}
