#include "mainwindow.h"
#include <QApplication>
#include <QThread>
#include <QScreen>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSize screenSize = a.primaryScreen()->size();

    QSettings settings("config.ini", QSettings::IniFormat);
    //settings.setValue("multipleWindows", 0);
    int multiWindows = settings.value("multipleWindows").toInt();
    QString fontColor = settings.value("fontColor").toString();
    QString fontSize = settings.value("fontSize").toString();
    //settings.sync();


// Damage Window

    MainWindow damageDisp;

    damageDisp.setAttribute(Qt::WA_TranslucentBackground);
    damageDisp.setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    damageDisp.move(0, (screenSize.height() - damageDisp.height()) / 2);

    QString style = "QLabel { color: " + fontColor + "; font-size: " + fontSize + "pt; }";
    damageDisp.setStyleSheet(style);

    QThread *thread = QThread::create([&damageDisp]{
            damageDisp.get_damage();
    });
    thread->start();

    damageDisp.show();

// HP Window

    MainWindow hpDisp;

    if (!multiWindows){
        hpDisp.setParent(&damageDisp);
    }
    hpDisp.setAttribute(Qt::WA_TranslucentBackground);
    hpDisp.setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    hpDisp.move((screenSize.width() - hpDisp.width()) / 2, 0);

    QThread *thread2 = QThread::create([&hpDisp]{
            hpDisp.get_hp();
    });
    thread2->start();

    hpDisp.show();

//


    return a.exec();
}
