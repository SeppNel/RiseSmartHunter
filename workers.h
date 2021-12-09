#pragma once
#include <QObject>

void fly_mode();

class Workers : public QObject {
    Q_OBJECT

public:
    void get_damage();
    void get_hp();

signals:
    void uiSignal(QString, QString);
};