#pragma once
#include "Common.h"


class AppTimer {
	LARGE_INTEGER counter;
	double timePerCount;
	double preRenderTime;
	double postRenderTime;
public:
	void setPreRenderTime();
	void setPostRenderTime();
	double getPostRenderSleepTime(int frameRate);
	AppTimer();
};
