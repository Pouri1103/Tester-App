#include "mainwindow.h"
#include <QScreen>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    if (QGuiApplication::primaryScreen()->size().width() == 1920)
        w.show();
    else
        w.showMaximized();

    return a.exec();
}
