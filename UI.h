#pragma once

#include <mutex>
#include <vector>

#include "arcdps_structs.h"

class UI {
public:
	void Draw();
	void addEvent(EventType type, cbtevent* ev, ag* src, ag* dst, char* skillname);
	bool opened;
	
private:
	std::vector<Event> events;
	int nextIndex = 0;
	std::mutex eventsMutex;

	char playerBuf[2048]{};
};
