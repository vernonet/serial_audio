#include "mainwindow.h"
#include <QApplication>
#include <QtMultimedia/QAudioOutput>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>

bool play_start = false;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();




    return a.exec();
}
