#ifndef REALTIME_H_INCLUDED
#define REALTIME_H_INCLUDED

class RealTime {
private:
	static double _freq, _time_offset;
	static bool _initialized;

	static void _init();
public:
	static double getTime();
	static void setTime(double time);
	static void wait(double delay_time);
};

#endif	// REALTIME_H_INCLUDED