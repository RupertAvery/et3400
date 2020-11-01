#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <thread>
#include <Windows.h>

#define LOG(x) std::cout << x << std::endl;


#ifdef _WINDOWS_
static void
sleep(long long useconds) //const struct timespec *requested_delay)
{
	time_t seconds = useconds / 1000000;                // second part
	long int nanoSeconds = (useconds % 1000000) * 1000; // nano seconds part
	if (seconds > 0)
		Sleep(seconds * 1000 + nanoSeconds / 1000000); //If more than one second
	else
	{
		static double frequency; // ticks per second
		if (frequency == 0)
		{
			LARGE_INTEGER freq;
			if (!QueryPerformanceFrequency(&freq))
			{
				/* Cannot use QueryPerformanceCounter. */
				Sleep(nanoSeconds / 1000000);
				return;
			}
			frequency = (double)freq.QuadPart / 1000000000.0; // ticks per nanosecond
		}
		long long counter_difference = nanoSeconds * frequency;
		int sleep_part = (int)nanoSeconds / 1000000 - 10;
		LARGE_INTEGER start;
		QueryPerformanceCounter(&start);
		long long expected_counter = start.QuadPart + counter_difference;
		if (sleep_part > 0) // for milliseconds part
			Sleep(sleep_part);
		for (;;) // for nanoseconds part
		{
			LARGE_INTEGER stop;
			QueryPerformanceCounter(&stop);
			if (stop.QuadPart >= expected_counter)
				break;
		}
	}
}
#else
static void sleep(int millisec)
{
	struct timespec req = { 0 };
	req.tv_sec = 0;
	req.tv_nsec = millisec * 1000000L;
	nanosleep(&req, (struct timespec*)NULL);
}
#endif // _WINDOWS_


#endif COMMON_H