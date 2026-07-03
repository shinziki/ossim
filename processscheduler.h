#pragma once
#include <QString>
#include <QColor>
#include <QVector>
#include <QList>

// Process state machine
enum class ProcState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

inline QString stateString(ProcState s) {
    switch (s) {
    case ProcState::NEW:        return "NEW";
    case ProcState::READY:      return "READY";
    case ProcState::RUNNING:    return "RUNNING";
    case ProcState::WAITING:    return "WAITING";
    case ProcState::TERMINATED: return "TERMINATED";
    }
    return "UNKNOWN";
}

inline QColor stateColor(ProcState s) {
    switch (s) {
    case ProcState::NEW:        return QColor("#4a6080");
    case ProcState::READY:      return QColor("#ff9f0a");
    case ProcState::RUNNING:    return QColor("#39ff14");
    case ProcState::WAITING:    return QColor("#00e5ff");
    case ProcState::TERMINATED: return QColor("#ff2d55");
    }
    return Qt::gray;
}

// Process descriptor
struct Process {
    int       pid;
    QString   name;
    int       arrivalTime;     // when it enters the queue (ticks)
    int       burstTime;       // total CPU ticks needed
    int       remainingTime;   // ticks left
    int       priority;        // lower = higher priority
    int       waitingTime;     // accumulated waiting ticks
    int       turnaroundTime;  // total time from arrival to finish
    int       startTick;
    int       finishTick;
    ProcState state;
    QColor    color;

    bool operator==(const Process &o) const {
        return pid == o.pid;
    }
};

// Gantt bar Entry
struct GanttEntry {
    int pid;
    int startTick;
    int endTick;
    QColor color;
    QString name;
};

// Scheduling algorithms
enum class Algorithm {
    FCFS,
    SJF,
    ROUND_ROBIN,
    PRIORITY
};

// Scheduler engine
class ProcessScheduler
{
public:
    ProcessScheduler() = default;

    void setAlgorithm(Algorithm alg) { m_algorithm = alg; }
    void setQuantum(int q) { m_quantum = q; }
    void addProcess(const Process &p) { m_processes.append(p); }
    void clearProcesses()  { m_processes.clear(); m_gantt.clear(); }

    const QList<Process>    &processes()     const { return m_processes; }
    const QList<GanttEntry> &ganttChart()    const { return m_gantt; }
    int                      totalTicks()    const { return m_totalTicks; }

    void reset();
    void run();          // compute full schedule, fill gantt

    // Step-based simulation (one tick at a time for animation)
    void  resetSim();
    bool  stepOneTick(); // returns false when simulation is done
    int   currentTick()  const { return m_currentTick; }
    int   runningPid()   const { return m_runningPid; }

    // Stats (populated after run())
    double avgWaitingTime()     const;
    double avgTurnaroundTime() const;
    double cpuUtilization() const;

private:
    QList<Process> m_processes;
    QList<GanttEntry> m_gantt;
    Algorithm m_algorithm = Algorithm::ROUND_ROBIN;
    int m_quantum = 2;
    int m_totalTicks = 0;

    // step-sim state
    int m_currentTick = 0;
    int m_runningPid = -1;
    int m_quantumLeft = 0;
};
