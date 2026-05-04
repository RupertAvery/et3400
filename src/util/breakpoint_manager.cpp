#include "breakpoint_manager.h"

bool BreakpointManager::hasBreakpoint(offs_t address)
{
    std::lock_guard<std::mutex> guard(_lock);
    return breakpoints.count(address) > 0;
}

void BreakpointManager::addBreakpoint(offs_t address)
{
    std::lock_guard<std::mutex> guard(_lock);
    breakpoints.try_emplace(address, Breakpoint{ address, 0, {}, {}, true });
}

void BreakpointManager::addBreakpoints(std::vector<Breakpoint>* newBreakpoints)
{
    std::lock_guard<std::mutex> guard(_lock);
    for (const auto& bp : *newBreakpoints)
        breakpoints.try_emplace(bp.address, bp);
}

void BreakpointManager::removeBreakpoint(offs_t address)
{
    std::lock_guard<std::mutex> guard(_lock);
    breakpoints.erase(address);
}

void BreakpointManager::addOrRemoveBreakpoint(offs_t address)
{
    std::lock_guard<std::mutex> guard(_lock);
    auto it = breakpoints.find(address);
    if (it != breakpoints.end())
        breakpoints.erase(it);
    else
        breakpoints.emplace(address, Breakpoint{ address, 0, {}, {}, true });
}

void BreakpointManager::clearRamBreakpoints()
{
    std::lock_guard<std::mutex> guard(_lock);
    for (auto it = breakpoints.begin(); it != breakpoints.end(); )
    {
        if (it->first < 0x0400)
            it = breakpoints.erase(it);
        else
            ++it;
    }
}

void BreakpointManager::loadBreakpoints(QString path, bool& success)
{
    std::vector<Breakpoint>* loaded = BreakpointReader::Read(path, success);
    if (success)
        addBreakpoints(loaded);
    delete loaded;
}

void BreakpointManager::saveBreakpoints(QString path, bool& success)
{
    std::vector<Breakpoint> vec;
    {
        std::lock_guard<std::mutex> guard(_lock);
        vec.reserve(breakpoints.size());
        for (const auto& pair : breakpoints)
            vec.push_back(pair.second);
    }
    BreakpointReader::Write(path, &vec, success);
}

void BreakpointManager::check_read(offs_t address, uint8_t data)
{
    std::lock_guard<std::mutex> guard(_lock);
    auto it = breakpoints.find(address);
    if (it != breakpoints.end() && it->second.type == 1)
    {
        // hit read breakpoint
    }
}

void BreakpointManager::check_write(offs_t address, uint8_t data)
{
    std::lock_guard<std::mutex> guard(_lock);
    auto it = breakpoints.find(address);
    if (it != breakpoints.end() && it->second.type == 2)
    {
        // hit write breakpoint
    }
}
