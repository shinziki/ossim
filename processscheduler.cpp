#include "processscheduler.h"
#include <algorithm>
#include <numeric>

// Utility
static Process* findByPid(QList<Process> &list, int pid) {
    for (auto &p : list) if (p.pid == pid) return &p;
    return nullptr;
}

// Full schedule run
void ProcessScheduler::reset() {
    m_gantt.clear();
    m_totalTicks = 0;
    m_currentTick = 0;
    m_runningPid = -1;
    m_quantumLeft = 0;
    m_readyQueue.clear();
}