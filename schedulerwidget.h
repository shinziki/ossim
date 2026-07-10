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
};
