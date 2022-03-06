#include <Windows.h>
#include <vector>
#include <iostream>
#include <tlhelp32.h>
#include <comdef.h>

#include "Offset-Values.hpp"
#include "MemMan.h"


int GetProcessIdByName() {
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    bool hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        _bstr_t b(pEntry.szExeFile);
        if (strcmp(b, "csgo.exe") == 0)
        {
            return (int)pEntry.th32ProcessID;
            break;
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
}


int main()
{
    std::ios_base::sync_with_stdio(false);
    std::vector<int> players;
    MemMan MemClass;
    int PID = MemClass.getProcess(L"csgo.exe");
    if (PID != GetProcessIdByName())
    {
        std::cout << "Valeleu valeleu vezi PIDu\n";
        return -1;
    }
    

    std::cout << "\aMerge si am PID = " << PID << '\n';

    int gameModule = MemClass.getModule(PID, L"client.dll");
    int localPlayer = MemClass.readMem<DWORD>(gameModule + hazedumper::signatures::dwLocalPlayer);


    for (short int i = 0; i < 10; i++)
    {
        int entity = MemClass.readMem<int>(gameModule + hazedumper::signatures::dwEntityList + i * 0x10);
        if (entity != localPlayer)
            players.push_back(entity);
    }
    std::cout << players.size() << " playeri\n";
    int flashDur = 0;
    int def = 0;
    int i = 0;
    while (1)
    {
        def = MemClass.readMem<int>(localPlayer + hazedumper::netvars::m_hBombDefuser);
        if (def > 0)
        {
            std::cout << "OK si?\n";
            MemClass.writeMem<int>(localPlayer + hazedumper::netvars::m_flFlashDuration, 3);
        }
        MemClass.writeMem<int>(localPlayer + hazedumper::netvars::m_bHasDefuser, 1);

        flashDur = MemClass.readMem<int>(localPlayer + hazedumper::netvars::m_flFlashDuration);
        //if (flashDur > 0)
        //    MemClass.writeMem<int>(localPlayer + hazedumper::netvars::m_flFlashDuration, 0);
        for (i = 0; i < players.size(); i++)
            MemClass.writeMem<bool>(players.at(i) + hazedumper::netvars::m_bSpotted, true);
    }
    return 0;
}
