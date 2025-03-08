#include "CPULoadMonitor.h"
#include <chrono>
#include <thread>

CPULoadMonitor::CPULoadMonitor() : previousTotalTicks(0), previousIdleTicks(0) {}

float CPULoadMonitor::GetCPULoad() {
    FILETIME idleTime, kernelTime, userTime;

    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        throw std::runtime_error("Failed to get system times.");
    }

    const auto idleTicks = FileTimeToUInt64(idleTime);
    const auto totalTicks = FileTimeToUInt64(kernelTime) + FileTimeToUInt64(userTime);

    return CalculateCPULoad(idleTicks, totalTicks);
}

unsigned long long CPULoadMonitor::FileTimeToUInt64(const FILETIME& ft) {
    return (static_cast<unsigned long long>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

float CPULoadMonitor::CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks) {
    const auto totalTicksSinceLastTime = totalTicks - previousTotalTicks;
    const auto idleTicksSinceLastTime = idleTicks - previousIdleTicks;

    const float cpuLoad = 1.0f - ((totalTicksSinceLastTime > 0)
        ? static_cast<float>(idleTicksSinceLastTime) / totalTicksSinceLastTime
        : 0.0f);

    previousTotalTicks = totalTicks;
    previousIdleTicks = idleTicks;

    return cpuLoad;
}