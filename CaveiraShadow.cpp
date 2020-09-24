#include <Windows.h>
#include <iostream>

#include "Memory.h"
#include "Config.h"

#include <fstream>
#include <filesystem>
#include <direct.h>

#include "Json.h"
using namespace nlohmann;

Memory* memory;

std::string CurrentPath()
{
    char* cwd = _getcwd(0, 0);
    std::string directory(cwd);
    std::free(cwd);
    return directory;

}

DWORD ProcessID = memory->GetProcessID(L"RainbowSix.exe");
HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
unsigned long long base = memory->BaseAddress(ProcessID);


class R6 {
public:
    class Functions {
    public:
        uintptr_t GameManager();
        uintptr_t RoundManager();
        uintptr_t EntityList();
        bool GameState();
    };

    class Features {
    public:
        void CaveiraESP(BYTE value);
    };

    Functions* functions;
    Features* features;
};

R6* r6;

uintptr_t R6::Functions::GameManager()
{
    return memory->Read<uintptr_t>(hProcess, base + Config::Offsets::pGameManager);
}

uintptr_t R6::Functions::RoundManager()
{
    return memory->Read<uintptr_t>(hProcess, base + Config::Offsets::pRoundManager);
}


uintptr_t R6::Functions::EntityList()
{
    uintptr_t encrypted = memory->Read<uintptr_t>(hProcess, GameManager() + Config::Offsets::EntityList::pChain);
    encrypted = ((encrypted ^ Config::Decryption::EntityList::pDecryption1) + Config::Decryption::EntityList::pDecryption2) ^ Config::Decryption::EntityList::pDecryption3;

    return encrypted;
}

bool R6::Functions::GameState()
{
    BYTE phase = memory->Read<BYTE>(hProcess, RoundManager() + Config::Offsets::EntityList::State::pChain_1);

    if (phase == 2 || phase == 3)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void R6::Features::CaveiraESP(BYTE value)
{
    for (int i = 0; i < 11; i++)
    {
        auto entityObject = memory->Read<uint64_t>(hProcess, r6->functions->EntityList() + (i * Config::Offsets::EntityList::pObject)); if (!entityObject) return;
        entityObject = memory->Read<uint64_t>(hProcess, entityObject + Config::Offsets::EntityList::pPawn); if (!entityObject) return;

        auto entityInfo = memory->Read<uint64_t>(hProcess, entityObject + Config::Offsets::EntityList::Info::pChain_1); if (!entityInfo) return;
        entityInfo = memory->Read<uint64_t>(hProcess, entityInfo + Config::Offsets::EntityList::Info::pChain_2); if (!entityInfo) return;

        for (uint32_t current = Config::Offsets::EntityList::Entity::pChain_1; current < Config::Offsets::EntityList::Entity::pChain_2; current += 4)
        {
            auto marker = memory->Read<uint64_t>(hProcess, entityInfo + current); if (!marker != 0) return;

            if (memory->Read<uintptr_t>(hProcess, marker) != (base + Config::Offsets::pVTable))
                continue;

            if (r6->functions->GameState())
            {
                memory->Write<byte>(hProcess, marker + Config::Offsets::EntityList::Marker::pSpotted, value);
            }
        }
    }
    return;
}

int main()
{
    SetConsoleTitleA("R6 Caveira ESP | Shadow Legacy | InsideExploit[UC]");

    json js;

    std::string configPath = CurrentPath() + "\\config.json";

    std::ifstream configR(configPath);  

    if (!configR)
    {

        std::cout << "[LOADING] Creating the json config file because is missing...\n";

        Sleep(300);

        std::ofstream configW;
        configW.open(configPath);
        configW << "{\n";
        configW << "  \"Enable\": 112,\n";
        configW << "  \"Disable\": 113\n";
        configW << "}";

        configW.close();

        Sleep(300);

        std::cout << "[SUCCESS] The json config file got created... please restart application.\n";

        Sleep(300);
        exit(-0.1);
    }

    configR >> js;

    uintptr_t hotkeyEnable = js["Enable"];
    uintptr_t hotkeyDisable = js["Disable"];

    if (!ProcessID)
    {
        std::cout << "[ERROR] Could not get process id. [-1]\n";
        Sleep(1500);
        exit(-1);
    }

    if (!hProcess)
    {
        std::cout << "[ERROR] Could not handle the process. [-2]\n";
        Sleep(1500);
        exit(-2);
    }

    if (!base)
    {
        std::cout << "[ERROR] Could not get base address. [-3]\n";
        Sleep(1500);
        exit(-3);
    }

    std::cout << "[INFO] Please use an command.\n\n";
    std::cout << "[HOTKEY] F1: Enable ESP.\n";
    std::cout << "[HOTKEY] F2: Disable ESP.\n";

    std::cout << "\n[IMPORTANT] Remember, that one showed key are the default one.\n";


    std::cout << "" << std::endl;

    while (1)
    {
        if (GetAsyncKeyState(hotkeyEnable) & 0x1)
        {
            std::cout << "[SUCCESS] Caveira ESP got enabled.\n";
            r6->features->CaveiraESP(1);
        }

        if (GetAsyncKeyState(hotkeyDisable) & 0x1)
        {
            std::cout << "[SUCCESS] Caveira ESP got disabled.\n";
            r6->features->CaveiraESP(0);
        }

    }

}