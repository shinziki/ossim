#include "mainwindow.h"
#include "schedulerwidget.h"
#include <QPainter>
#include <QFrame>
#include <QSplitter>
#include <QProgressBar>
#include <QRandomGenerator>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QDateTime>

// Palette
static const QColor BG_DEEP ("#050b18");
static const QColor NEON_CYAN ("#00e5ff");
static const QColor NEON_GREEN ("#39ff14");
static const QColor NEON_RED ("#ff2d55");
static const QColor DIM_TEXT ("#4a6080");

// MetricCard
class MetricCard : public QFrame {
    Q_OBJECT
public:
    MetricCard(const QString &title, const QColor &accent, QWidget *parent = nullptr) : QFrame(parent), m_accent(accent) {
        setFixedHeight(88);
        setStyleSheet(QString(
            "background:#0d1526; border:1px solid %1; border-radius:4px;")
                          .arg(accent.name()));

        auto *vl = new QVBoxLayout(this);
        vl->setContentsMargins(10,8,10,8);
        vl->setSpacing(3);

        m_title = new QLabel(title);
        m_title->setFont(QFont("Courier New", 8));
        m_title->setStyleSheet("color:" + DIM_TEXT.name() + "; border:none;");

        m_value = new QLabel("--");
        m_value->setFont(QFont("Courier New", 18, QFont::Bold));
        m_title->setStyleSheet("color:" + accent.name() + "; border:none;");

        m_bar = new QProgressBar();
        m_bar->setRange(0,100);
        m_bar->setValue(0);
        m_bar->setFixedHeight(4);
        m_bar->setTextVisible(false);
        m_bar->setStyleSheet(
            QString("QProgressBar{background:#0a1020; border:none; border-radius:2px;}"
                    "QProgressBar::chunk{background:%1; border-radius:2px;}").arg(accent.name()));

        vl->addWidget(m_title);
        vl->addWidget(m_value);
        vl->addWidget(m_bar);
    }

    void setValue(const QString &v, int pct=-1) {
        m_value->setText(v);
        if (pct>=0) m_bar->setValue(pct);
    }
private:
    QLabel *m_title, *m_value;
    QProgressBar *m_bar;
    QColor m_accent;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("NexOS - OS Behavior Simulator");
    setMinimumSize(1200, 750);
    resize(1420, 860);
    void setupUI();
}

void MainWindow::setupUI()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);
    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // Title bar
    auto *bar = new QWidget();
    bar->setFixedHeight(50);
    bar->setStyleSheet("background: #060d1e; border-bottom: 1px solid #00e5ff25;");
    auto *bl = new QHBoxLayout(bar);
    bl->setContentsMargins(16,0,16,0);

    auto mkLabel = [](const QString &t, const QColor &c, int pt, bool bold=false)
    {
        auto *l = new QLabel(t);
        QFont f("Courier New", pt);
        f.setBold(bold);
        l->setFont(t);
        l->setStyleSheet("color:" + c.name() + ";");
        return l;
    };

    bl->addWidget(mkLabel("⬡ NexOS", NEON_CYAN, 14, true));
    bl->addSpacing(12);
    bl->addWidget(mkLabel("OS Behavior Simulator v1.0-stage1", DIM_TEXT, 9));
    bl->addStretch();
    m_clockLabel = mkLabel("00:00:00", NEON_GREEN, 12, true);
    bl->addWidget(m_clockLabel);
    root->addWidget(bar);

    // Body splitter
    auto *splitter = new QSplitter(Qt::Horizontal);
    splitter->setHandleWidth(1);
    splitter->setStyleSheet("QSplitter::handle{background:#00e5ff15;}");

    // Side panel
    auto *side = new QWidget();
    side->setFixedWidth(196);
    side->setStyleSheet("background: #080f1e; border-right: 1px solid #0d1e38;");
    auto *sl = new QVBoxLayout(side);
    sl->setContentsMargins(10,16,10,16);
    sl->setSpacing(8);

    auto *sysLabel = new QLabel("SYSTEM METRICS");
    sysLabel->setFont(QFont("Courier New", 8, QFont::Bold));
    sysLabel->setStyleSheet("color: #4a6080; letter-spacing: 2px;");
    sl->addWidget(sysLabel);
    sl->addSpacing(4);

    auto *cpuCard = new MetricCard("CPU LOAD", NEON_CYAN);
    auto *memCard = new MetricCard("MEMORY", NEON_GREEN);
    auto *procCard = new MetricCard("PROCESSES", QColor("#ff9f0a"));
    auto *ioCard = new MetricCard("I/O WAIT", NEON_RED);
    sl->addWidget(cpuCard);
    sl->addWidget(memCard);
    sl->addWidget(procCard);
    sl->addWidget(ioCard);
    sl->addStretch();

    auto *verLabel = new QLabel("BUILD 2025.1\nSTAGE 1 OF 4");
    verLabel->setFont(QFont("Courier New", 8));
    verLabel->setStyleSheet("color:#2a3a55;");
    verLabel->setAlignment(Qt::AlignCenter);
    sl->addWidget(verLabel);

    // Tab widget
    m_tabs = new QTabWidget();
    m_tabs->setStyleSheet(R"(
        QTabWidget::pane { border:none; background:#050b18; }
        QTabBar::tab {
            background:#0d1526; color:#4a6080;
            font-family:'Courier New'; font-size:10px; font-weight:bold;
            padding:10px 20px; border:none;
            border-top:2px solid transparent; margin-right:2px;
        }
        QTabBar::tab:selected { background:#0a1a35; color:#00e5ff; border-top:2px solid #00e5ff; }
        QTabBar::tab:hover:!selected { color:#7fa8c9; background:#0c1828; }
    )");

    m_scheduler = new SchedulerWidget();
    m_tabs->addTab(m_scheduler, "⚙ PROCESS SCHEDULER");

    auto placeholder = [&](const QString &name) -> QWidget* {
        auto *w = new QWidget();
        w->setStyleSheet("background: #050b18");
        auto *vl = new QVBoxLayout(w);
        auto *lb = new QLabel(name + "\n\nCOMING IN NEXT STAGE");
        lb->setFont(QFont("Courier New", 14));
        lb->setStyleSheet("color: #2a3a55;");
        lb->setAlignment(Qt::AlignCenter);
        vl->addWidget(lb);
        return w;
    };

    m_tabs->addTab(placeholder("MEMORY ALLOCATOR"), "▦  MEMORY");
    m_tabs->addTab(placeholder("CPU CORE ASSIGNMENT"), "◈  CPU CORES");
    m_tabs->addTab(placeholder("DEADLOCK SIMULATOR"), "☠  DEADLOCK");
    m_tabs->addTab(placeholder("FILE SYSTEM BLOCKS"),  "⊞  FILE SYSTEM");

    splitter->addWidget(side);
    splitter->addWidget(m_tabs);
    splitter->setStretchFactor(1, 1);
    root->addWidget(splitter, 1);

    // Status Bar
    auto *sb = statusBar();
    sb->setStyleSheet("background:#060d1e; color:#4a6080; "
                      "font-family:'Courier New'; font-size:9px; "
                      "border-top:1px solid #00e5ff15;");
    m_statusLabel  = new QLabel("SYSTEM NOMINAL  |  ALL SUBSYSTEMS ONLINE");
    m_uptimeLabel  = new QLabel("UPTIME: 00:00:00");
    m_statusLabel->setStyleSheet("color:#39ff14;");
    m_uptimeLabel->setStyleSheet("color:#4a6080;");
    sb->addWidget(m_statusLabel);
    sb->addPermanentWidget(m_uptimeLabel);

    // Timers
    m_clockTimer = new QTimer(this);
    connect(m_clockTimer, &QTimer::timeout, this, &MainWindow::updateClock);
    m_clockTimer->start(1000);
    updateClock();

    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, &QTimer::timeout, this, [=]() {
        static int cpu=35, mem=48, proc=12, io=8;
        cpu = qBound(5, cpu + QRandomGenerator::global()->bounded(-8,9), 95);
        mem = qBound(20, mem + QRandomGenerator::global()->bounded(-3,4), 90);
        proc = qBound(3, proc + QRandomGenerator::global()->bounded(-2,3), 64);
        io = qBound(0, io + QRandomGenerator::global()->bounded(-5,6), 40);
        cpuCard->setValue(QString("%1%").arg(cpu), cpu);
        memCard->setValue(QString("%1%").arg(mem), mem);
        procCard->setValue(QString::number(proc), proc*100/64);
        ioCard->setValue(QString("%1%").arg(io), io);
        m_fakeUptime++;
        int h=m_fakeUptime/3600, m2=(m_fakeUptime%3600)/60, s=m_fakeUptime%60;
        m_uptimeLabel->setText(
            QString("UPTIME: %1:%2:%3")
                .arg(h,2,10,QLatin1Char('0'))
                .arg(m2,2,10))
    });
}

void MainWindow::updateClock()
{

}
