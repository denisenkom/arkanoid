#ifndef REALTIME_H_INCLUDED
#define REALTIME_H_INCLUDED

class RealTime {
private:
	static float _freq, _time_offset;
	static bool _initialized;

	static void _init();
public:
	static float getTime();
	static void setTime(float time);
	static void wait(float delay_time);
};

#endif	// REALTIME_H_INCLUDED