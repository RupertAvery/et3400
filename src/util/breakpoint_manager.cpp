#include "breakpoint_manager.h";

BreakpointManager::BreakpointManager()
{
	breakpoints = new std::vector<Breakpoint>;
}

BreakpointManager::~BreakpointManager()
{
	delete breakpoints;
}

std::vector<Breakpoint>* BreakpointManager::getBreakpoints() {
	return breakpoints;
}

bool BreakpointManager::hasBreakpoint(offs_t address)
{
	_lock.lock();
	std::vector<Breakpoint>::const_iterator it = breakpoints->begin();
	while (it != breakpoints->end())
	{
		if ((*it).address == address)
		{
			_lock.unlock();
			return true;
		}
		it++;
	}
	_lock.unlock();
	return false;
}

void BreakpointManager::addBreakpoints(std::vector<Breakpoint>* newBreakpoints)
{
	_lock.lock();
	std::vector<Breakpoint>::iterator current = newBreakpoints->begin();
	while (current != newBreakpoints->end())
	{
		bool bfound = false;
		std::vector<Breakpoint>::iterator it = breakpoints->begin();
		while (it != breakpoints->end())
		{
			if ((*it).address == (*current).address)
			{
				bfound = true;
				break;
			}
			it++;
		}

		if (!bfound) {
			breakpoints->push_back(*current);
		}
		current++;
	}
	_lock.unlock();
}

void BreakpointManager::addBreakpoint(offs_t address)
{
	_lock.lock();
	std::vector<Breakpoint>::iterator it = breakpoints->begin();
	while (it != breakpoints->end())
	{
		if ((*it).address == address)
		{
			return;
		}
		it++;
	}

	breakpoints->push_back(Breakpoint{ address, true });
	_lock.unlock();
}


void BreakpointManager::clearRamBreakpoints()
{
	_lock.lock();
	std::vector<Breakpoint>::iterator it = breakpoints->begin();
	while (it != breakpoints->end())
	{
		if ((*it).address < 0x0400)
		{
			it = breakpoints->erase(it);
		}
		else
		{
			it++;
		}
	}
	_lock.unlock();
}

void BreakpointManager::removeBreakpoint(offs_t address)
{
	_lock.lock();
	std::vector<Breakpoint>::iterator it = breakpoints->begin();
	while (it != breakpoints->end())
	{
		if ((*it).address == address)
		{
			it = breakpoints->erase(it);
		}
		else
		{
			it++;
		}
	}
	_lock.unlock();
}

void BreakpointManager::addOrRemoveBreakpoint(offs_t address)
{
	_lock.lock();
	std::vector<Breakpoint>::iterator it = breakpoints->begin();
	while (it != breakpoints->end())
	{
		if ((*it).address == address)
		{
			it = breakpoints->erase(it);
			_lock.unlock();
			return;
		}
		else
		{
			it++;
		}
	}

	breakpoints->push_back(Breakpoint{ address, true });
	_lock.unlock();
}

void BreakpointManager::loadBreakpoints(QString path, bool& success)
{
	std::vector<Breakpoint>* newBreakpoints = BreakpointReader::Read(path, success);
	if (success)
	{
		addBreakpoints(newBreakpoints);
	}
	delete newBreakpoints;
}

void BreakpointManager::saveBreakpoints(QString path, bool& success)
{
	//std::vector<Label> filteredLabels;
	//std::vector<Label>::iterator current = labels->begin();

	//while (current != labels->end())
	//{
	//    if ((*current).start >= start && (*current).end <= end)
	//    {
	//        filteredLabels.push_back((*current));
	//        break;
	//    }
	//    current++;
	//}

	BreakpointReader::Write(path, breakpoints, success);
}
