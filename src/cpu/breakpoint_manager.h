#ifndef BREAKPOINT_MANAGER_H
#define BREAKPOINT_MANAGER_H

#include "defs.h"
#include <thread>
#include <mutex>
#include <vector>

struct Breakpoint
{
    offs_t address;
    bool is_enabled;
};

class BreakpointManager
{
public:
    BreakpointManager();
    ~BreakpointManager();
    void addBreakpoint(offs_t address);
    void removeBreakpoint(offs_t address);
    void addOrRemoveBreakpoint(offs_t address);
    bool hasBreakpoint(offs_t address);
    std::vector<Breakpoint> getBreakpoints();

private:
    std::vector<Breakpoint> *breakpoints;
    std::mutex lock;
};

#endif //BREAKPOINT_MANAGER_H
