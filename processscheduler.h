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
