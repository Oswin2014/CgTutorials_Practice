
#ifndef _Own_Timer_H
#define _Own_Timer_H

#include "OwnMath.h"

class OwnTimer
{
public:
	OwnTimer(void);
	~OwnTimer(void);

	void timeStart();

	void timeEnd();

public:
	u64 mCountStart;
	u64 mCountEnd;

	//the frequency of the performance counter
	static f32 mCountFrequency;

	f32 mSec;

	//millisecond
	f32 mMsec;

	//microsecond 
	f32 mUsec;
};

#endif
