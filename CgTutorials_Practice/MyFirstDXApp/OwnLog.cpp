#include "OwnLog.h"

#include <ctime>
#include <iomanip>

//-----------------------------------------------------------------------
OwnLog::OwnLog(const std::string& name)
	: mLogName(name)
{
	mLog.open(name.c_str());
}
//-----------------------------------------------------------------------
OwnLog::~OwnLog(void)
{
	mLog.close();
}
//-----------------------------------------------------------------------
void OwnLog::logMessage( const std::string& message )
{
	struct tm *pTime;
	time_t ctTime;
	time(&ctTime);
	pTime = localtime(&ctTime);

	mLog << std::setw(2) << std::setfill('0') << pTime->tm_hour
		<< ":" << std::setw(2) << std::setfill('0') << pTime->tm_min
		<< ":" << std::setw(2) << std::setfill('0') << pTime->tm_sec
		<< ": ";

	mLog << message << std::endl;
	mLog.flush();
}
//-----------------------------------------------------------------------
