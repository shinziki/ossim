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
    QStringList headers = {"PID", "Name", "Arrival", "Burst", "Remaining", "Priority", "State", "Wait", "Turnaround"};
    setColumnCount(headers.size());
    setHorizontalHeaderLabels(headers);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAlternatingRowColors(false);
    verticalHeader()->setVisible(false);
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(QHeaderView::RezieToContents);
    setShowGrid(false);
    setStyleSheet(R"(
        QTableWidget {
            background: #050b18;
            color: #7fa8c9;
            font-family: 'Courier New';
            font-size: 10px;
            border: none;
            gridline-color: transparent;
        }
        QTableWidget::item {
            padding: 4px 8px;
            border-bottom: 1px solid #0d1e38;
        }
        QTableWidget::item:selected {
            background: #0d2040;
            color: #00e5ff;
        }
        QHeaderView::section {
            background: #080f1e;
            color: #4a6080;
            font-family: 'Courier New';
            font-size: 9px;
            font-weight: bold;
            border: none;
            border-bottom: 1px solid #00e5ff30;
            padding: 6px 8px;
        }
        QScrollBar:vertical, QScrollBar:horizontal {
            background: #080f1e;
            width: 6px; height: 6px;
        }
        QScrollBar::handle { background: #1a3050; border-radius: 3px; }
    )");
}


void ProcessTableWidget::refreshProcess(const QList<Process> &procs) {
    setRowCount(procs.size());
    for (int i = 0; i < procs.size(); ++i) {
        const Process &p = procs[i];
        QColor sc = stateColor(p.state);

        auto cell = [&](int col, const QString &text, const QColor &fg = QColor("#7fa8c9")) {
            auto *item = new QTableWidgetItem(text);
            item->setForeground(fg);
            item->setTextAlignment(Qt::AlignCenter);
            setItem(i, col, item);
        };

        // Colour dot in PID column
        auto *pidItem = new QTableWidgetItem(
            QString("● P%1").arg(p.pid));
        pidItem->setForeground(p.color);
        pidItem->setTextAlignment(Qt::AlignCenter);
        pidItem->setFont(QFont("Courier New", 10, QFont::Bold));
        setItem(i, 0, pidItem);

        cell(1, p.name);
        cell(2, QString::number(p.arrivalTime));
        cell(3, QString::number(p.burstTime));
        cell(4, p.state == ProcState::TERMINATED ? "-" : QString::number(p.remainingTime));
        cell(5, QString::number(p.priority));
        cell(6, stateString(p.state), sc);
        cell(7, p.waitingTime    >= 0 ? QString::number(p.waitingTime)    : "-");
        cell(8, p.turnaroundTime >= 0 ? QString::number(p.turnaroundTime) : "-");

        // Row background based on state
        for (int c = 0; c < columnCount(); ++c) {
            if (auto *it = item(i, c)) {
                QColor bg("#050b18");
                if (p.state == ProcState::RUNNING)    bg = QColor("#001a08");
                if (p.state == ProcState::TERMINATED) bg = QColor("#0a0508");
                it->setBackground(bg);
            }
        }
    }
}

// SchedulerWidget
static QLabel* sectionTitle(const QString &text) {
    auto *l = new QLabel(text);
    l->setFont(QFont("Courier New", 8, QFont::Bold));
    l->setStyleSheet("color: #4a6080; letter-spacing: 2px;");
    return l;
}

static QFrame* hLine() {
    auto *f = new QFrame();
    f->setFrameShape(QFrame::HLine);
    f->setStyleSheet("border: none; border-top: 1px solid #0d1e38");
    f->setFixedHeight(1);
    return f;
}

static QPushButton* neonButton(const QString &text, const QColor &color) {
    auto *btn = new QPushButton(text);
    btn->setFont(QFont("Courier New", 9, QFont::Bold));
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedHeight(32);
    btn->setStyleSheet(
        QString("QPushButton {"
                "  background: transparent;"
                "  color: %1;"
                "  border: 1px solid %1;"
                "  border-radius: 3px;"
                "  padding: 0 12px;"
                "}"
                "QPushButton:hover {"
                "  background: %2;"
                "  color: #050b18;"
                "}"
                "QPushButton:pressed { opacity: 0.7; }"
                "QPushButton:disabled { color: #1a2a40; border-color: #1a2a40; }")
            .arg(color.name())
            .arg(color.name()));
    return btn;
}

static QSpinBox* neonSpin(int min, int max, int val) {
    auto *s = new QSpinBox();
    s->setRange(min, max);
    s->setValue(val);
    s->setFont(QFont("Courier New", 10));
    s->setFixedHeight(28);
    s->setStyleSheet(R"(
        QSpinBox {
            background: #080f1e;
            color: #7fa8c9;
            border: 1px solid #1a3050;
            border-radius: 3px;
            padding: 0 4px;
        }
        QSpinBox:focus { border-color: #00e5ff; color: #00e5ff; }
        QSpinBox::up-button, QSpinBox::down-button {
            background: #0d1526;
            border: none;
            width: 16px;
        }
    )");
    return s;
}

SchedulerWidget::SchedulerWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
    buildDefaultProcesses();

    m_animTimer = new QTimer(this);
    m_animTimer->setInterval(200);
    connect(m_animTimer, &QTimer::timeout, this, &SchedulerWidget::onAnimationTick);
}

void SchedulerWidget::setupUI() {
    auto *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // Left control panel
    auto *ctrlPanel = new QWidget();
    ctrlPanel->setFixedWidth(240);
    ctrlPanel->setStyleSheet("background: #080f1e; border-right: 1px solid #0d1e38;");
    auto *ctrlLayout = new QVBoxLayout(ctrlPanel);
    ctrlLayout->setContentsMargins(14, 14, 14, 14);
    ctrlLayout->setSpacing(8);

    // Algorithm
    ctrlLayout->addWidget(sectionTitle("ALGORITHM"));
    m_algorithmBox = new QComboBox();
    m_algorithmBox->addItems({
        "Round Robin",
        "FCFS",
        "SJF",
        "Priority"
    });
    m_algorithmBox->setFont(QFont("Courier New", 9));
    m_algorithmBox->setStyleSheet(R"(
        QComboBox {
            background: #080f1e; color: #00e5ff;
            border: 1px solid #00e5ff50; border-radius: 3px;
            padding: 4px 8px; font-family: 'Courier New'; font-size: 9px;
        }
        QComboBox::drop-down { border: none; }
        QComboBox QAbstractItemView {
            background: #0d1526; color: #7fa8c9; border: 1px solid #00e5ff30;
        }
    )");
    connect(m_algorithmBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SchedulerWidget::onAlgorithmChanged);

    ctrlLayout->addWidget(m_algorithmBox);

    // Quantum
    ctrlLayout->addWidget(sectionTitle("TIME QUANTUM"));
    m_quantumSpin = neonSpin(1, 10, 2);
    ctrlLayout->addWidget(m_quantumSpin);

    ctrlLayout->addWidget(hLine());

    // Process builder
    ctrlLayout->addWidget(sectionTitle("ADD PROCESS"));

    auto makeRow = [&](const QString &lbl, QSpinBox *spin) {
        auto *row = new QWidget();
        auto *rl = new QHBoxLayout(row);
        rl->setContentsMargins(0,0,0,0);
        auto *label = new QLabel(lbl);
        label->setFont(QFont("Courier New", 8));
        label->setStyleSheet("color: #4A6080;");
        label->setFixedWidth(70);
        rl->addWidget(label);
        rl->addWidget(spin);
        return row;
    };

    m_burstSpin = neonSpin(1, 20, 5);
    m_arrivalSpin = neonSpin(0);
}