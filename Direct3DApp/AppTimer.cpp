#include "AppTimer.h"


/**
 * @brief Set the time at the start of all rendering for the frame.
 */
void AppTimer::setPreRenderTime()
{
	QueryPerformanceCounter(&counter);
	preRenderTime = counter.QuadPart * timePerCount;
}


/**
 * @brief Set the time at the end of all rendering for the frame.
 */
void AppTimer::setPostRenderTime()
{
	QueryPerformanceCounter(&counter);
	postRenderTime = counter.QuadPart * timePerCount;
}


/**
 * @brief Get the amount of time until a swap should occur for the desired frame rate.
 * @param frameRate The desired frame rate used to calculate the sleep time.
 * @return The number of milliseconds to sleep for. 
 */
double AppTimer::getPostRenderSleepTime(int frameRate)
{
	double totalRenderTime = postRenderTime - preRenderTime;
	totalRenderTime = totalRenderTime < 0.0 ? 0.f : totalRenderTime;
	double sleepTimeInSecs = (1.0 / frameRate) - totalRenderTime;
	return 1000.0 * sleepTimeInSecs;
}

/**
 * @note LARGE_INTEGER union provides 64-bit int support regardless of if the compiler supports it or not.
 * The QuadPart of the union represents the entire 64-bit value.
 */
AppTimer::AppTimer()
{
	LARGE_INTEGER counterFrequency;
	// Find the frequency of the performance counter
	QueryPerformanceFrequency(&counterFrequency);
	timePerCount = 1.0 / (double)(counterFrequency.QuadPart);
	preRenderTime = 0.0;
	postRenderTime = 0.0;
}
