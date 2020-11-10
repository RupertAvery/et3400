#include "breakpoint_manager.h";

BreakpointManager::BreakpointManager()
{
    breakpoints = new std::vector<Breakpoint>;
}

BreakpointManager::~BreakpointManager()
{
    delete breakpoints;
}

bool BreakpointManager::hasBreakpoint(offs_t address)
{
    lock.lock();
    std::vector<Breakpoint>::const_iterator it = breakpoints->begin();
    while (it != breakpoints->end())
    {
        if ((*it).address == address)
        {
            lock.unlock();
            return true;
        }
        it++;
    }
    lock.unlock();
    return false;
}

void BreakpointManager::addBreakpoint(offs_t address)
{
    lock.lock();
    std::vector<Breakpoint>::iterator it = breakpoints->begin();
    while (it != breakpoints->end())
    {
        if ((*it).address == address)
        {
            return;
        }
        it++;
    }

    breakpoints->push_back(Breakpoint{address, true});
    lock.unlock();
}

void BreakpointManager::removeBreakpoint(offs_t address)
{
    lock.lock();
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
    lock.unlock();
}