#include "ServerStdThread.h"

ServerStdThread::~ServerStdThread()
{
	if (thread.joinable())
		thread.join();
}
