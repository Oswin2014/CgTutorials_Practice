#include "OwnTimer.h"

f32 OwnTimer::mCountFrequency = 0;
//-----------------------------------------------------------------------
OwnTimer::OwnTimer(void)
{
	if(0 == mCountFrequency)
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		mCountFrequency = (f32)li.QuadPart;
	}
	mCountEnd = 0;
}
//-----------------------------------------------------------------------
OwnTimer::~OwnTimer(void)
{
}
//-----------------------------------------------------------------------
void OwnTimer::timeStart()
{
	QueryPerformanceCounter((LARGE_INTEGER *)&mCountStart);
}
//-----------------------------------------------------------------------
void OwnTimer::timeEnd()
{
	QueryPerformanceCounter((LARGE_INTEGER *)&mCountEnd);

	f32 countPass = (f32)(mCountEnd - mCountStart);
	mSec = countPass / mCountFrequency;
	mMsec = mSec * 1000.0f;
	mUsec = mSec * 1000000.0f;
}
//-----------------------------------------------------------------------