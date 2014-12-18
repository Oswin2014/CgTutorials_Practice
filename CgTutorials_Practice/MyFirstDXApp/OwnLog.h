
#ifndef _Own_Log_H
#define _Own_Log_H

#include <string>
#include <fstream>

class OwnLog
{
public:
	OwnLog(const std::string& name);
	~OwnLog(void);

	void logMessage(const std::string& message);

protected:
	std::ofstream mLog;

	std::string mLogName;
};

#endif