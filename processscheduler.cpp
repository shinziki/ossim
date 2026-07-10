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

    int tick = 0;
    while (!remaining.isEmpty()) {
        // Find arrived and not finished
        QList<Process*> available;
        for (auto &p : remaining)
            if (p.arrivalTime <= tick) available.append(&p);

        if (available.isEmpty()) { tick++; continue; }

        // Sort by burst time
        std::sort(available.begin(), avaiable.end(), [](const Process *a, const Process *b){ return a->burstTime < b->burstTime; });

        Process *chosen = available.first();
        chosen->startTick = tick;
        chosen->waitingTime = tick - chosen->arrivalTime;

        GanttEntry g;
        g.pid       = chosen->pid;
        g.name      = chosen->name;
        g.color     = chosen->color;
        g.startTick = tick;
        g.endTick   = tick + chosen->burstTime;
        m_gantt.append(g);

        tick                  += chosen->burstTime;
        chosen->finishTick     = tick;
        chosen->turnaroundTime = tick - chosen->arrivalTime;
        chosen->state          = ProcState::TERMINATED;

        // write back and remove
        for (auto &orig : m_processes)
            if (orig.pid == chosen->pid) { orig = *chosen; break; }

        remaining.removeIf([&](const Process &p){ return p.pid == chosen->pid; });
    }
    m_totalTicks = tick;
}

// Round Robin
void ProcessScheduler::runRR() {
    QList<Process> procs = m_processes;
    QList<int> queue;
    int tick = 0;

    // sort by arrival
    std::sort(procs.begin(), procs.end(), [](const Process &a, const Process &b){ return a.arrivalTime < b.arrivalTime; });

    // seed queue with first arriavls at tick 0
    for (auto &p : procs)
        if (p.arrivalTime == 0) queue.append(p.pid);

    auto anyRemaining = [&]() {
        return std::any_of(procs.begin(), procs.end(), [](const Process &p){ return p.remainingTime > 0; });
    };

    while (anyRemaining()) {
        if (queue.isEmpty()) {
            // advance to next arrival
            int nextArr = INT_MAX;
            for (auto &p : procs)
                if (p.remainingTime > 0) nextArr = qMin(nextArr, p.arrivalTime);
            tick = nextArr;
            for (auto &p : procs)
                if (p.arrivalTime <= tick && p.remainingTime > 0 && !queue.contains(p.pid))
                    queue.append(p.pid);
            continue;
        }

        int pid      = queue.takeFirst();
        Process *cur = findByPid(procs, pid);
        if (!cur || cur->remainingTime <= 0) continue;

        if (cur->startTick < 0) cur->startTick = tick;

        int slice = qMin(m_quantum, cur->remainingTime);
        GanttEntry g;
        g.pid       = cur->pid;
        g.name      = cur->name;
        g.color     = cur->color;
        g.startTick = tick;
        g.endTick   = tick + slice;
        m_gantt.append(g);

        cur->remainingTime -= slice;
        int oldTick = tick;
        tick += slice;

        // Enqueue processes that arrived during this slice
        for (auto &p : procs)
            if (p.arrivalTime > oldTick && p.arrivalTime <= tick
                && p.remainingTime > 0 && !queue.contains(p.pid))
                queue.append(p.pid);

        if (cur->remainingTime > 0) {
            queue.append(cur->pid);
        } else {
            cur->finishTick     = tick;
            cur->turnaroundTime = tick - cur->arrivalTime;
            cur->waitingTime    = cur->turnaroundTime - cur->burstTime;
            cur->state          = ProcState::TERMINATED;
            for (auto &orig : m_processes)
                if (orig.pid == cur->pid) { orig = *cur; break; }
        }
    }
    m_totalTicks = tick;
}

// Priority
void ProcessScheduler::runPriority() {
    QList<Process> remaining = m_processes;
    int tick = 0;

    auto anyRemaining = [&]() {
        return std::any_of(remaining.begin(), remaining.end(), [](const Process &p){ return p.remainingTime > 0; });;

    };

    while (anyRemaining()) {
        QList<Process*> available;
        for (auto &p : remaining)
            if (p.arrivalTime <= tick && p.remainingTime > 0) available.append(&p);

        if (available.isEmpty()) { tick++; continue; }

        std::sort(available.begin(), available.end(),
                  [](const Process *a, const Process *b){ return a->priority < b->priority; });

        Process *chosen = available.first();
        if (chosen->startTick < 0) chosen->startTick = tick;
        chosen->waitingTime = tick - chosen->arrivalTime;

        GanttEntry g;
        g.pid       = chosen->pid;
        g.name      = chosen->name;
        g.color     = chosen->color;
        g.startTick = tick;
        g.endTick   = tick + chosen->burstTime;
        m_gantt.append(g);

        tick                  += chosen->burstTime;
        chosen->finishTick     = tick;
        chosen->turnaroundTime = tick - chosen->arrivalTime;
        chosen->remainingTime  = 0;
        chosen->state          = ProcState::TERMINATED;

        for (auto &orig : m_processes)
            if (orig.pid == chosen->pid) { orig = *chosen; break; }

        remaining.removeIf([&](const Process &p){ return p.pid == chosen->pid; });
    }
    m_totalTicks = tick;
}

// Stats
double ProcessScheduler::avgWaitingTime() const {
    if (m_processes.isEmpty()) return 0;
    double sum = 0;
    for (const auto &p : m_processes) sum += p.waitingTime;
    return sum / m_processes.size();
}

double ProcessScheduler::avgTurnAroundTime() const {
    if (m_processes.isEmpty()) return 0;
    double sum = 0;
    for (const auto &p : m_processes) sum += p.turnaroundTime;
    return sum / m_processes.size();
}

double ProcessScheduler::cpuUtilization() const {
    if (m_totalTicks == 0) return 0;
    int busy = 0;
    for (const auto &p : m_gantt) busy += (g.endTick - g.startTick);
    return (double)busy / m_totalTicks * 100.0;
}

// Step-based simulation
void ProcessScheduler::resetSim() {
    reset();
    m_currentTick = 0;
}