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
    for (auto &p : m_processes) {
        p.remainingTime = p.burstTime;
        p.waitingTime = 0;
        p.turnaroundTime = 0;
        p.startTick = -1;
        p.finishTick = -1;
        p.state = ProcState::NEW;
    }
}

void ProcessScheduler::run() {
    reset();
    switch (m_algorithm) {
    case Algorithm::FCFS: runFCFS(); break;
    case Algorithm::SJF: runSJF(); break;
    case Algorithm::ROUND_ROBIN: runRR(); break;
    case Algorithm::PRIORITY: runPriority(); break;
    }
}

// FCFS
void ProcessScheduler::runFCFS() {
    QList<Process> procs = m_processes;
    std::sort(procs.begin(), procs.end(), [](const Process &a, const Process &b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int tick = 0;
    for (auto &p : procs) {
        if (tick < p.arrivalTime) tick = p.arrivalTime; // idle gap
        p.startTick      = tick;
        p.waitingTime    = tick - p.arrivalTime;
        p.state          = ProcState::RUNNING;

        GanttEntry g;
        g.pid       = p.pid;
        g.name      = p.name;
        g.color     = p.color;
        g.startTick = tick;
        g.endTick   = tick + p.burstTime;
        m_gantt.append(g);

        tick             += p.burstTime;
        p.finishTick      = tick;
        p.turnaroundTime  = tick - p.arrivalTime;
        p.state           = ProcState::TERMINATED;

        // write back
        for (auto &orig : m_processes)
            if (orig.pid == p.pid) { orig = p; break; }
    }
    m_totalTicks = tick;
}

// SJF
void ProcessScheduler::runSJF() {
    QList<Process> remaining = m_processes;
    for (auto &p : remaining) p.state = ProcState::READY;
}