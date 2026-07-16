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
    QColor("#bf5af2"), QColor("#ff2d55"), QColor("#30d158")
};

schedulerwidget::schedulerwidget() {}
