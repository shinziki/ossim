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

        auto *v1 = new QVBoxLayout(this);
        v1->setContentsMargins(10,8,10,8);
        v1->setSpacing(3);

        m_title = new QLabel(title);
        m_title->setFont(QFont("Courier New", 8));
        m_title->setStyleSheet("color:" + DIM_TEXT.name() + "; border:none;");
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
