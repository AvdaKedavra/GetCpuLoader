#ifndef CPULOADMONITOR_H
#define CPULOADMONITOR_H

#include <windows.h>
#include <stdexcept>

class CPULoadMonitor {
public:

    CPULoadMonitor();
    float GetCPULoad();
private:

    unsigned long long previousTotalTicks;
    unsigned long long previousIdleTicks;

    static unsigned long long FileTimeToUInt64(const FILETIME& ft);
    float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);
};

#endif // CPULOADMONITOR_H