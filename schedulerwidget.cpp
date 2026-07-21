#include "schedulerwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFrame>
#include <QSplitter>
#include <QScrollBar>
#include <QFont>
#include <QFontMetrics>
#include <QRandomGenerator>
#include <cmath>

// Pallete
const QVector<QColor> SchedulerWidget::PROC_COLORS = {
    QColor("#00e5ff"), QColor("#39ff14"), QColor("#ff9f0a"),
    QColor("#bf5af2"), QColor("#ff2d55"), QColor("#30d158"),
    QColor("#ff6b6b"), QColor("#4ecdc4"), QColor("#ffe66d"),
    QColor("#a8e6cf")
};

// GanttWidget
GanttWidget::GanttWidget(QWidget *parent) : QWidget(parent) {
    setMinimumHeight(120);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void GanttWidget::setGanttData(const QList<GanttEntry> &entries, int totalTicks, int currentTick) {
    m_entries = entries;
    m_totalTicks = totalTicks;
    m_currentTick = currentTick;

    // Compute requried width
    if (totalTicks > 0) {
        double pxPerTick = (double)(width() - LEFT_MARGIN - RIGHT_MARGIN) / totalTicks;
        pxPerTick = qMax(pxPerTick, 18.0);
        setMinimumWidth(LEFT_MARGIN + (int)(pxPerTick * totalTicks) + RIGHT_MARGIN);
    }

    update();
}

void GanttWidget::setCurrentTick(int tick) {
    m_currentTick = tick;
    update();
}

void GanttWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (m_totalTicks > 0) setGanttData(m_entries, m_totalTicks, m_currentTick);
}

void GanttWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Background
    p.fillRect(rect(), QColor("#050b18"));

    if (m_entries.isEmpty() || m_totalTicks == 0) {
        p.setPen(QColor("#2a3a55"));
        p.setFont(QFont("Courier New", 11));
        p.drawText(rect(), Qt::AlignCenter, "No schedule data - press RUN");
        return;
    }

    const int W = width();
    const double pxPerTick = qMax(18.0, (double)(W - LEFT_MARGIN - RIGHT_MARGIN) / m_totalTicks);

    // Collect unique process rows
    QMap<int, QPair<QString, QColor>> pidInfo;
    for (const auto &g : m_entries) pidInfo[g.pid] = {g.name, g.color};

    QList<int> pids = pidInfo.keys();
    int rowCount = pids.size();
    int totalH = TOP_MARGIN + TICK_LABEL_H + rowCount * (m_rowH + 6) + 20;
}

schedulerwidget::schedulerwidget() {}
