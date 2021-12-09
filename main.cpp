#include "mainwindow.h"
#include "workers.h"
#include <QApplication>
#include <QThread>
#include <QScreen>
#include <QSettings>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSize screenSize = a.primaryScreen()->size();

    // Create config file if not present
    if (!QFile("config.ini").exists()) {
        QSettings settings("config.ini", QSettings::IniFormat);
        settings.setValue("multipleWindows", 0);
        settings.setValue("fontColor", "red");
        settings.setValue("fontSize", 20);
        settings.setValue("flyMode", 0);
    }

    // Read config file
    QSettings settings("config.ini", QSettings::IniFormat);
    int multiWindows = settings.value("multipleWindows").toInt();
    QString fontColor = settings.value("fontColor").toString();
    QString fontSize = settings.value("fontSize").toString();
    int flyMode = settings.value("flyMode").toInt();



// Damage Window

    MainWindow damageDisp;
    Workers wDamage;

    QObject::connect(&wDamage, SIGNAL(uiSignal(QString, QString)), &damageDisp, SLOT(updateUI(QString, QString)));

    damageDisp.setAttribute(Qt::WA_TranslucentBackground);
    damageDisp.setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    damageDisp.move(5, (screenSize.height() - damageDisp.height()) / 2);
    damageDisp.resize(150, damageDisp.height());

    QString style = "QLabel { color: " + fontColor + "; font-size: " + fontSize + "pt; font-weight: bold }";
    damageDisp.setStyleSheet(style);

    QThread *thread = QThread::create([&wDamage]{
            wDamage.get_damage();
    });
    thread->start();

    damageDisp.show();

// HP Window

    MainWindow hpDisp;
    Workers wHP;

    QObject::connect(&wHP, SIGNAL(uiSignal(QString, QString)), &hpDisp, SLOT(updateUI(QString, QString)));

    if (!multiWindows){
        hpDisp.setParent(&damageDisp);
    }
    hpDisp.setAttribute(Qt::WA_TranslucentBackground);
    hpDisp.setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    hpDisp.move((screenSize.width() - hpDisp.width()) / 2, 5);

    QThread *thread2 = QThread::create([&wHP]{
            wHP.get_hp();
    });
    thread2->start();

    hpDisp.show();

// Fly Mode

    if (flyMode){
        QThread *thread3 = QThread::create([]{
                fly_mode();
        });
        thread3->start();
    }

    return a.exec();
}
