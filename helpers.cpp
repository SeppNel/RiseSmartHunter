#include <helpers.h>
#include <codecvt>

//Function to find the window and return de handle (duh)
HANDLE find_window() {
    HWND window;
    do
    {
        window = FindWindowA(NULL, "MonsterHunterRise");
        Sleep(500);
    } while (window == NULL);

    DWORD pid;
    GetWindowThreadProcessId(window, &pid);

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

    return hProcess;
}

std::string get_hunter_name() {
    HANDLE hProcess = find_window();
    std::wstring wname;

    float* nameP = ReadMultiLevelPointer<float>(hProcess, (void*)0x14C079130, { 0x68, 0x158, 0x50, 0x2C4 });
    ReadProcessMemory(hProcess, (void*)nameP, &wname, 20, 0);

    //setup converter
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    std::string name = converter.to_bytes(wname);
    name.erase(std::find(name.begin(), name.end(), '\0'), name.end());

    return name;
}


union Char_to_Float
{
    unsigned char byte[4];
    float real;
};

float decodeHealth(unsigned int value, unsigned int key) {
    char decoded[4];
    unsigned int edx, eax, ecx, r8d;

    edx = (value & 0xFF);
    ecx = key + (key * 2);
    edx = edx + ecx;
    eax = ecx;
    r8d = ecx;
    ecx = eax * 0x3Fu;

    r8d = r8d + 4;
    key = r8d;
    r8d = r8d + 4;

    // First byte
    eax = (key >> 8);
    key = (key & 0x00FFFFFF) | (r8d & 0xFF000000);
    r8d = r8d + 4;
    edx = edx ^ ecx;
    ecx = eax * 0x3Fu;
    decoded[0] = edx;

    // Second byte
    edx = (value >> 8);
    edx = edx + eax;
    eax = (key >> 16);
    key = r8d;
    r8d = 0;
    edx = edx ^ ecx;
    ecx = eax * 0x3Fu;
    decoded[1] = edx;

    // Third byte
    edx = (value >> 16);
    edx = edx + eax;
    eax = (key >> 24);
    edx = edx ^ ecx;
    ecx = eax * 0x3Fu;
    decoded[2] = edx;

    // Fourth byte
    edx = (value >> 24);
    edx = edx + eax;
    edx = edx ^ ecx;
    decoded[3] = edx;

    Char_to_Float hp;
    hp.byte[0] = decoded[0];
    hp.byte[1] = decoded[1];
    hp.byte[2] = decoded[2];
    hp.byte[3] = decoded[3];

    return hp.real;
}