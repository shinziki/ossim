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
};
