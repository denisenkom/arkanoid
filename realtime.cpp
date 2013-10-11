#include "stdafx.h"
#pragma hdrstop

#include "realtime.h"

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
