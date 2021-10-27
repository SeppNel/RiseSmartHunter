#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <vector>
#include <windows.h>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this, SIGNAL(uiSignal(QString, QString)), this, SLOT(updateUI(QString, QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


// This 2 functions to drag the windows without bar

void MainWindow::mousePressEvent(QMouseEvent *event) {
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}


// This to update the UI in a thread safe way, fuck race conditions

void MainWindow::updateUI(QString text, QString style)
{
    ui->main_label->setText(text);
    ui->main_label->setStyleSheet(style);
}


// Function to read pointers (https://github.com/Ezekial711/MonsterHunterWorldModding/wiki/Making-a-persistent-loop-plugin)

template<class T>
T* ReadMultiLevelPointerSafe(HANDLE process_handle, void* base_address, const std::vector<uintptr_t>& offsets)
{
    uintptr_t addr = (uintptr_t)base_address;
    for (const auto& offset : offsets)
    {
        uintptr_t tmp = (uintptr_t)nullptr;
        do {
            Sleep(20);
            ReadProcessMemory(process_handle, (void*)addr, &tmp, sizeof(tmp), NULL);
        } while (tmp == (uintptr_t)nullptr);

        addr = tmp;
        addr += offset;
    }

    return (T*)addr;
}


//Function to find the window and return de handle (duh)

HANDLE find_window() {
    HWND window;
    do
    {
        window = FindWindowA(NULL, "MonsterHunterRise Demo");
        Sleep(500);
    } while (window == NULL);

    DWORD pid;
    GetWindowThreadProcessId(window, &pid);

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

    return hProcess;
}


void MainWindow::get_damage()
{
    HANDLE hProcess = find_window();

    QString style = "background-color: rgba(140, 140, 140, 0.6);";
    emit uiSignal("Listo", style);

    int damageval = 0;
    while (true)
    {
        float* damage = ReadMultiLevelPointerSafe<float>(hProcess, (void*)0x14BF036A8, { 0x4C8, 0x18 });
        ReadProcessMemory(hProcess, (void*)damage, &damageval, sizeof(damageval), 0);

        QString text = QString("Daño: %1").arg(damageval);
        emit uiSignal(text, style);


        Sleep(100);
    }

}

void MainWindow::get_hp()
{
    HANDLE hProcess = find_window();

    //ui->main_label->setText("Listo");
    QString style = "background-color: rgba(140, 140, 140, 0.6);";
    emit uiSignal("Listo", style);

    float monsterHPval = 0;
    char buffer[8];
    float maxHP = 0;

    while (true)
    {

        float* monsterHP = ReadMultiLevelPointerSafe<float>(hProcess, (void*)0x14BEF7738, { 0x80, 0x40, 0x290, 0x48, 0x40, 0x40, 0x10 });
        //ReadProcessMemory(hProcess, (void*)monsterHP, &monsterHPval, sizeof(monsterHPval), 0);
        ReadProcessMemory(hProcess, (void*)monsterHP, &buffer, 8, 0);

        memcpy(&monsterHPval, &buffer, 4);
        memcpy(&maxHP, &buffer[4], 4);

        //qDebug() << monsterHPval << " - " << maxHP;
        
        QString text = QString("%1 / %2").arg(int(monsterHPval + 0.5)).arg(int(maxHP + 0.5));

        float percent = monsterHPval / maxHP;
        percent = roundf(percent * 100) / 100;
        QString style = "background-color: qlineargradient(spread:pad, x1:" + QString::number(percent - 0.01) + ", y1:1, x2:" + QString::number(percent) + ", y2:1, stop:0 red, stop:1 rgba(140, 140, 140, 0.6));";

        emit uiSignal(text, style);


        Sleep(100);
    }

}


void fly_mode()
{
    HANDLE hProcess = find_window();

    float val = 0;
    while (true)
    {
        //Read monster HP at start to block execution if not in a hunt, because otherwise this function can read a pointer mid change in the menu and softlock itself, he.
        ReadMultiLevelPointerSafe<float>(hProcess, (void*)0x14BEF7738, { 0x80, 0x40, 0x290, 0x48, 0x40, 0x40, 0x10 });

        float* wirebug = ReadMultiLevelPointerSafe<float>(hProcess, (void*)0x14BEF63E8, { 0x60, 0x20, 0x3B0, 0x28, 0x10 });
        ReadProcessMemory(hProcess, (void*)wirebug, &val, sizeof(val), 0);

        if(val > 0){
            val = 0;
            int returnval = WriteProcessMemory(hProcess, (void*)wirebug, &val, sizeof(val), 0);
        }

        Sleep(16);
    }

}
