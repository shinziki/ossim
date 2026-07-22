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
    setMinimumHeight(totalH);

    // Draw tick grid
    p.setPen(QPen(QColor("#0d1e38"), 1));
    p.setFont(QFont("Courier New", 7));
    int step = qMax(1, (int)(30.0 / pxPerTick));
    for (int t = 0; t <= m_totalTicks; t += step) {
        int x = LEFT_MARGIN + (int)(t * pxPerTick);
        p.setPen(QPen(QColor("#0d2040"), 1, Qt::DotLine));
        p.drawLine(x, TOP_MARGIN + TICK_LABEL_H, x, TOP_MARGIN + TICK_LABEL_H + rowCount * (m_rowH + 6));
        p.setPen(QColor("#3a5070"));
        p.drawText(x - 8, TOP_MARGIN + TICK_LABEL_H - 2, 24, 14, Qt::AlignCenter, QString::number(t));
    }

    // Draw Gantt bars
    int rowY = TOP_MARGIN + TICK_LABEL_H;
    for (int pid : pids) {
        // Row label
        p.setFont(QFont("Courier New", 9, QFont::Bold));
        p.setPen(pidInfo[pid].second);
        p.drawText(0, rowY, LEFT_MARGIN - 6, m_rowH, Qt::AlignRight | Qt::AlignVCenter, pidInfo[pid].first);

        // Bars for this pid
        for (const auto &g : m_entries) {
            if (g.pid != pid) continue;

            int x1 = LEFT_MARGIN + (int)(g.startTick * pxPerTick);
            int x2 = LEFT_MARGIN + (int)(g.endTick   * pxPerTick);
            int barW = x2 - x1 - 2;
            if (barW < 1) barW = 1;

            // Determine how much of this bar has been "executed"
            double filled = 0.0;
            if (m_currentTick >= g.endTick) {
                filled = 1.0;
            } else if (m_currentTick > g.startTick) {
                filled = (double)(m_currentTick - g.startTick) /
                         (g.endTick - g.startTick);
            }

            QRect barRect(x1 + 1, rowY + 4, barW, m_rowH - 8);

            // Dark base
            p.fillRect(barRect, QColor(g.color.red(),
                                       g.color.green(),
                                       g.color.blue(), 30));

            // Filled portion
            if (filled > 0.0) {
                QRect filled_rect(barRect.x(), barRect.y(),
                                  (int)(barRect.width() * filled),
                                  barRect.height());
                QLinearGradient grad(filled_rect.topLeft(), filled_rect.topRight());
                grad.setColorAt(0, QColor(g.color.red(),
                                          g.color.green(),
                                          g.color.blue(), 200));
                grad.setColorAt(1, g.color);
                p.fillRect(filled_rect, grad);

                // Glow line at right edge
                if (filled < 1.0) {
                    int ex = filled_rect.right();
                    p.setPen(QPen(g.color, 2));
                    p.drawLine(ex, barRect.top(), ex, barRect.bottom());
                }
            }

            // Border
            p.setPen(QPen(g.color, 1));
            p.drawRect(barRect);

            // Label inside bar
            if (barW > 30) {
                p.setPen(Qt::white);
                p.setFont(QFont("Courier New", 8));
                p.drawText(barRect, Qt::AlignCenter, g.name);
            }
        }
        rowY += m_rowH + 6;
    }

    // Current tick cursor
    if (m_currentTick > 0 && m_currentTick <= m_totalTicks) {
        int cx = LEFT_MARGIN + (int)(m_currentTick * pxPerTick);
        p.setPen(QPen(QColor("#ffffff60"), 1, Qt::DashLine));
        p.drawLine(cx, TOP_MARGIN, cx, TOP_MARGIN + TICK_LABEL_H + rowCount * (m_rowH + 6));

        // Tick label above cursor
        p.setFont(QFont("Courier New", 8, QFont::Bold));
        p.setPen(Qt::white);
        QRect tickRect(cx - 16, 4, 32, 16);
        p.fillRect(tickRect, QColor("#00e5ff"));
        p.setPen(QColor("#050b18"));
        p.drawText(tickRect, Qt::AlignCenter, QString::number(m_currentTick));
    }
}

// ProcessTableWidget
ProcessTableWidget::ProcessTableWidget(QWidget *parent) : QTableWidget(parent) {
    QStringList headers = {"PID", "Name", "Arrival", "Burst", "Remaining", "Priority", "State", "Wait"}
}

schedulerwidget::schedulerwidget() {}
