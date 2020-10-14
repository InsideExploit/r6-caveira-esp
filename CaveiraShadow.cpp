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
        uintptr_t EntityCount();
        uintptr_t EntityInfo(uintptr_t entity);
        bool GameState();
    };

    class Features {
    public:
        void CaveiraESP(bool active1);
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
    uint64_t entityList = memory->Read<uint64_t>(hProcess, base + Config::Offsets::pGameManager);
    entityList = memory->Read<uint64_t>(hProcess, entityList + 0xE0);

    entityList ^= 0x53;
    entityList += 0xEEBD43B91E3D5D54;
    entityList ^= 0x1FEC13843E78A654;

    return entityList;
}

uintptr_t R6::Functions::EntityInfo(uintptr_t entity)
{
    uint64_t info = memory->Read<uint64_t>(hProcess, entity + 0x50);
    info = _rotl64(info, 1);
    info -= 0x53;
    info = info ^ 0x84B4E3BD4F9014AF;

    return info;
}

bool R6::Functions::GameState()
{
    BYTE phase = memory->Read<BYTE>(hProcess, RoundManager() + 0x300);

    if (phase == 2 || phase == 3)
    {
        return true;
    }
    else
    {
        return false;
    }
}

uintptr_t R6::Functions::EntityCount()
{
    uintptr_t entity_count = 0x0;

    entity_count = memory->Read<uint64_t>(hProcess, r6->functions->GameManager() + 0xE8);
    
    entity_count ^= 0x53;
    entity_count += 0xEEBD43B91E3D5D54;
    entity_count ^= 0x1FEC13843E78A654;

    int countNumber = (int)(entity_count ^ 0x18C0000000);

    return countNumber;
}

void R6::Features::CaveiraESP(bool active)
{

    int playerCount = r6->functions->EntityCount();

    for (int player = 0; player < playerCount; player++)
    {
        auto entity_object = memory->Read<uint64_t>(hProcess, r6->functions->EntityList() + (player * 0x8));
        entity_object = r6->functions->EntityInfo(entity_object);

        auto entity_info = memory->Read<uint64_t>(hProcess, entity_object + 0x18);
        entity_info = memory->Read<uint64_t>(hProcess, entity_info + 0xD8);

        for (uint32_t current = 0x80; current < 0xF0; current += 4)
        {
            auto markerIcon = memory->Read<uint64_t>(hProcess, entity_info + current); 
            
            if (markerIcon == 0) 
                continue;

            auto checkForInvalid = memory->Read<uint64_t>(hProcess, markerIcon); 
            if (checkForInvalid != (base + Config::Offsets::pVTable))
             continue;

            bool game_state = r6->functions->GameState();

            if(game_state && active)
                memory->Write<uint8_t>(hProcess, markerIcon + 0x220, 0x85);
            else
                memory->Write<uint8_t>(hProcess, markerIcon + 0x220, 0x84);
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
            r6->features->CaveiraESP(true);
        }

        if (GetAsyncKeyState(hotkeyDisable) & 0x1)
        {
            std::cout << "[SUCCESS] Caveira ESP got disabled.\n";
            r6->features->CaveiraESP(false);
        }

    }

}
