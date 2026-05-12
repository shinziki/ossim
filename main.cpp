#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("OS Behavior Simulator");
    a.setOrganizationName("NexOS Lab");

    // Load global stylesheet
    QFile styleFile(":/styles/futuristic.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = QLatin1String(styleFile.readAll());
        a.setStyleSheet(style);
        styleFile.close();
    }

    MainWindow w;
    w.show();
    return a.exec();
}
