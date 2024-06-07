#ifndef USER_EVENTS_MANAGER_H
#define USER_EVENTS_MANAGER_H

#include <Windows.h>
#include <string>

/**
* Manager of user events.
*
* It is a singleton.
*/

class UserEventsManager
{
public:
	~UserEventsManager(void) {
		OutputDebugString("Destroying UserEventsManager...\n");
	};

	static UserEventsManager * getUserEventsManager();
	void execute(const std::string &scriptName);

private:
	static UserEventsManager * userEventsManager;
	UserEventsManager() {};

};

#endif //USER_EVENTS_MANAGER_H
