#ifndef BREAKPOINT_MANAGER_H
#define BREAKPOINT_MANAGER_H

#include "../common/common_defs.h"
#include "breakpoint.h"
#include <thread>
#include <mutex>
#include <vector>
#include <QString>

class BreakpointManager
{
public:
    BreakpointManager();
    ~BreakpointManager();
    void addBreakpoint(offs_t address);
    void addBreakpoints(std::vector<Breakpoint> *newBreakpoints);
    void clearRamBreakpoints();
    void removeBreakpoint(offs_t address);
    void addOrRemoveBreakpoint(offs_t address);
    bool hasBreakpoint(offs_t address);
    void loadBreakpoints(QString path, bool &success);
    void saveBreakpoints(QString path, bool &success);
    std::vector<Breakpoint> *getBreakpoints();

private:
    std::vector<Breakpoint> *breakpoints;
    std::mutex _lock;
};

#endif //BREAKPOINT_MANAGER_H
