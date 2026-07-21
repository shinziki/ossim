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
}

schedulerwidget::schedulerwidget() {}
