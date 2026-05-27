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
}

void MainWindow::updateClock()
{

}
