#pragma once
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QScrollArea>
#include "processscheduler.h"

// Gantt Chart custom painter widget
class GanttWidget : public QWidget {
    Q_OBJECT
public:
    explicit GanttWidget(QWidget *parent = nullptr);

    void setGanttData(const QList<GanttEntry> &entries, int totalTicks, int currentTick);
    void setCurrentTick();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QList<GanttEntry> m_entries;
    int m_totalTicks = 0;
    int m_currentTick = 0;
    int m_rowH = 36;

    // Layout constants
    static constexpr int LEFT_MARGIN  = 90;
    static constexpr int RIGHT_MARGIN = 20;
    static constexpr int TOP_MARGIN   = 30;
    static constexpr int TICK_LABEL_H = 22;
};

// ── Process table widget ──────────────────────────────────────────────────────
class ProcessTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit ProcessTableWidget(QWidget *parent = nullptr);
    void refreshProcesses(const QList<Process> &procs);
};

// ── Main scheduler panel ──────────────────────────────────────────────────────
class SchedulerWidget : public QWidget {
    Q_OBJECT
public:
    explicit SchedulerWidget(QWidget *parent = nullptr);

private slots:
    void onAddProcess();
    void onRemoveProcess();
    void onRunSimulation();
    void onResetSimulation();
    void onAnimationTick();
    void onAlgorithmChanged(int index);
    void onSpeedChanged(int val);

private:
    void setupUI();
    void setupControlPanel();
    void setupMainArea();
    void buildDefaultProcesses();
    void updateStats();
    void applyGlow(QWidget *w, const QColor &color);

    // Engine
    ProcessScheduler m_engine;

    // Controls
    QComboBox *m_algorithmBox = nullptr;
    QSpinBox *m_quantumSpin = nullptr;
    QSpinBox *m_speedSpin = nullptr;
    QPushButton *m_runBtn = nullptr;
    QPushButton *m_resetBtn = nullptr;
    QPushButton *m_addBtn = nullptr;
    QPushButton *m_removeBtn = nullptr;
};
