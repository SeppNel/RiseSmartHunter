#include "workers.h"
#include "helpers.h"
#include <windows.h>

void Workers::get_damage()
{
    HANDLE hProcess = find_window();

    QString style = "background-color: rgba(140, 140, 140, 0.6);";
    emit uiSignal("Ready", style);

    std::string name = "IDK";
    int damageval = 0;
    while (true)
    {
        if (name == "IDK") {
            name = get_hunter_name();
        }
        float* damage = ReadMultiLevelPointer<float>(hProcess, (void*)0x14C01E948, { 0x4E0, 0x18 });
        ReadProcessMemory(hProcess, (void*)damage, &damageval, sizeof(damageval), 0);

        QString text;
        if (name == "Akazell") {
            text = QString("Damage: %1").arg(floor(damageval * 0.8));
        }
        else {
            text = QString("Damage: %1").arg(damageval);
        }
        emit uiSignal(text, style);


        Sleep(350);
    }
}

void Workers::get_hp()
{
    HANDLE hProcess = find_window();

    QString style = "background-color: rgba(140, 140, 140, 0.6);";
    emit uiSignal("Ready", style);

    float maxHP;
    uint healthEncoded, healthEncodedMod, healthEncodedKey;

    while (true)
    {
        float* maxHP_p = ReadMultiLevelPointer<float>(hProcess, (void*)0x1487F6BE4, { 0x1C0, 0x2A0, 0x178 });
        bool readHP = ReadProcessMemory(hProcess, (void*)maxHP_p, &maxHP, 4, 0);

        //uint* EncodedPtr = ReadMultiLevelPointer<uint>(hProcess, (void*)0x14C01EBC0, { 0x80, 0x10, 0x20, 0x2A0, 0x48, 0x10, 0x20, 0x10, 0x20, 0x10, 0x18 });
        uint* EncodedPtr = ReadMultiLevelPointer<uint>(hProcess, (void*)0x1487F6BE4, { 0x1C0, 0x2A0, 0x198 });

        uint* mod = ReadMultiLevelPointer<uint>(hProcess, (void*)EncodedPtr, { 0x18 });
        ReadProcessMemory(hProcess, (void*)mod, &healthEncodedMod, 4, 0);
        healthEncodedMod = healthEncodedMod & 3;

        uint* hp = ReadMultiLevelPointer<uint>(hProcess, (void*)EncodedPtr, { healthEncodedMod * 4 + 0x1C });
        ReadProcessMemory(hProcess, (void*)hp, &healthEncoded, 4, 0);

        uint* key = ReadMultiLevelPointer<uint>(hProcess, (void*)EncodedPtr, { 0x14 });
        ReadProcessMemory(hProcess, (void*)(key), &healthEncodedKey, 4, 0);

        float currentHealth = decodeHealth(healthEncoded, healthEncodedKey);

        if (readHP){

            QString text = QString("%1 / %2").arg(int(currentHealth + 0.5)).arg(int(maxHP + 0.5));

            float percent = currentHealth / maxHP;
            percent = roundf(percent * 100) / 100;
            QString style = "background-color: qlineargradient(spread:pad, x1:" + QString::number(percent - 0.01) + ", y1:1, x2:" + QString::number(percent) + ", y2:1, stop:0 red, stop:1 rgba(140, 140, 140, 0.6));";

            emit uiSignal(text, style);
        }
        else {
            emit uiSignal("Waiting", style);
        }

        Sleep(350);
    }

}