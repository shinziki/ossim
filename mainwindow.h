#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QTimer>
#include <QDateTime>

class SchedulerWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void updateClock();

private:
    void setupUI();

    QTabWidget *m_tabs = nullptr;
    QLabel *m_clockLabel = nullptr;
    QLabel *m_updateLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
    QTimer *m_clockTimer = nullptr;
    QTimer *m_statusTimer = nullptr;

    SchedulerWidget *m_scheduler = nullptr;
    int m_fakeUptime = 0;
};
