#pragma once
#include <windows.h>
#include <string>
#include <vector>

// Function to read pointers, using the non safe version because the safe version enters an infinite loop in some cases (https://github.com/Ezekial711/MonsterHunterWorldModding/wiki/Making-a-persistent-loop-plugin)
template<class T>
T* ReadMultiLevelPointer(HANDLE process_handle, void* base_address, const std::vector<uintptr_t>& offsets)
{
    uintptr_t addr = (uintptr_t)base_address;
    for (const auto& offset : offsets)
    {
        ReadProcessMemory(process_handle, (void*)addr, &addr, sizeof(addr), NULL);
        addr += offset;
    }

    return (T*)addr;
}

HANDLE find_window();

std::string get_hunter_name();

float decodeHealth(unsigned int value, unsigned int key);