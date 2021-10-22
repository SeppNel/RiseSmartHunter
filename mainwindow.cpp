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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}



template<class T>
T* ReadMultiLevelPointerSafe(HANDLE process_handle, void* base_address, const std::vector<uintptr_t>& offsets)
{
    uintptr_t addr = (uintptr_t)base_address;
    for (const auto& offset : offsets)
    {
        uintptr_t tmp = (uintptr_t)nullptr;
        do {
            Sleep(30);
            ReadProcessMemory(process_handle, (void*)addr, &tmp, sizeof(tmp), NULL);
        } while (tmp == (uintptr_t)nullptr);

        addr = tmp;
        addr += offset;
    }

    return (T*)addr;
}


void MainWindow::get_damage()
{
    HWND window;
    do
    {
        window = FindWindowA(NULL, "MonsterHunterRise Demo");
        Sleep(100);
    } while (window == NULL);

    ui->main_label->setText("Listo");

    DWORD pid;
    GetWindowThreadProcessId(window, &pid);

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

    int damageval = 0;
    while (true)
    {
        float* damage = ReadMultiLevelPointerSafe<float>(hProcess, (void*)0x14BF036A8, { 0x4C8, 0x18 });
        ReadProcessMemory(hProcess, (void*)damage, &damageval, sizeof(damageval), 0);

        ui->main_label->setText(QString("Daño: %1").arg(damageval));

        Sleep(20);
    }

}

void MainWindow::get_hp()
{
    HWND window;
    do
    {
        window = FindWindowA(NULL, "MonsterHunterRise Demo");
        Sleep(100);
    } while (window == NULL);

    ui->main_label->setText("Listo");

    DWORD pid;
    GetWindowThreadProcessId(window, &pid);

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

    float monsterHPval = 0;
    int i = 0;
    while (true)
    {
        float* monsterHP = ReadMultiLevelPointerSafe<float>(hProcess, (void*)0x14BEF7738, { 0x80, 0x40, 0x290, 0x48, 0x40, 0x40, 0x10});
        ReadProcessMemory(hProcess, (void*)monsterHP, &monsterHPval, sizeof(monsterHPval), 0);

        ui->main_label->setText(QString("Vida: %1").arg(int(monsterHPval + 0.5)));

        std::cout << i << " - " << int(monsterHPval + 0.5) << std::endl;
        i++;

        Sleep(20);
    }

}


void fly_mode()
{
    HWND window;
    do
    {
        window = FindWindowA(NULL, "MonsterHunterRise Demo");
        Sleep(100);
    } while (window == NULL);

    DWORD pid;
    GetWindowThreadProcessId(window, &pid);

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

    float val = 0;
    while (true)
    {
        float* wirebug = ReadMultiLevelPointerSafe<float>(hProcess, (void*)0x14BEF63E8, { 0x60, 0x20, 0x3B0, 0x28, 0x10});
        ReadProcessMemory(hProcess, (void*)wirebug, &val, sizeof(val), 0);

        if(val > 0){
            val = 0;
            WriteProcessMemory(hProcess, (void*)wirebug, &val, sizeof(val), 0);
        }

        Sleep(16);
    }

}
