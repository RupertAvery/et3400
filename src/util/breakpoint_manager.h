#ifndef BREAKPOINT_MANAGER_H
#define BREAKPOINT_MANAGER_H

#include "../common/common_defs.h"
#include "breakpoint.h"
#include <mutex>
#include <unordered_map>
#include <vector>
#include <QString>

class BreakpointManager
{
public:
    BreakpointManager() = default;
    ~BreakpointManager() = default;
    void addBreakpoint(offs_t address);
    void addBreakpoints(std::vector<Breakpoint>* newBreakpoints);
    void clearRamBreakpoints();
    void removeBreakpoint(offs_t address);
    void addOrRemoveBreakpoint(offs_t address);
    bool hasBreakpoint(offs_t address);
    void loadBreakpoints(QString path, bool& success);
    void saveBreakpoints(QString path, bool& success);
    void check_read(offs_t address, uint8_t data);
    void check_write(offs_t address, uint8_t data);

private:
    std::unordered_map<offs_t, Breakpoint> breakpoints;
    std::mutex _lock;
};

#endif //BREAKPOINT_MANAGER_H
