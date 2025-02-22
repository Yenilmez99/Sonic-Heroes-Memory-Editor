#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>
#include <Windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <math.h>
#include <tchar.h>
#include <stdlib.h>
#include <string>

#define FrameRateLock 144

int CharacterChanger(int MainCharacterAddress, int Calls) {
    int WriteCode = Calls - (MainCharacterAddress + 0x5);

    return WriteCode;
}

DWORD GetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID); // make snapshot of all modules within process
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
    {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) // if Found Module matches Module we look for -> done!
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32


    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

DWORD GetPointerAddress(HWND hwnd, DWORD gameBaseAddr, DWORD address, std::vector<DWORD> offsets)
{
    DWORD pID = NULL; // Game process ID
    GetWindowThreadProcessId(hwnd, &pID);
    HANDLE phandle = NULL;
    phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if (phandle == INVALID_HANDLE_VALUE || phandle == NULL);

    DWORD offset_null = NULL;
    ReadProcessMemory(phandle, (LPVOID*)(gameBaseAddr + address), &offset_null, sizeof(offset_null), 0);
    DWORD pointeraddress = offset_null; // the address we need
    for (int i = 0; i < offsets.size() - 1; i++) // we dont want to change the last offset value so we do -1
    {
        ReadProcessMemory(phandle, (LPVOID*)(pointeraddress + offsets.at(i)), &pointeraddress, sizeof(pointeraddress), 0);
    }
    return pointeraddress += offsets.at(offsets.size() - 1); // adding the last offset
}

int main() {

    bool SizeEditCheck = 0, CharacterPosEdit = 0, CharacterPointEdit = 0;
    bool TeamBlast = 0, RingCheck = 0, InfiniteFlyCheck = 0, TimeFreezeCheck = 0;
    bool CameraEditCheckbox = 0, CameraFreeze = 0, ColorEditCheck = 0;

    int RunStage = 0, Ring = 0, Live = 0, OverrideStageCode = 1, OverrideTeamCode = -1;
    int SpeedCharacterPower = 0x0, FlyCharacterPower = 0x0, PowerCharacterPower = 0x0;
    int ActiveCharacter = 0, Time[3] = { 0,0,0 };
    int CharacterSpeedPoint = 0, CharacterFlyPoint = 0, CharacterPowerPoint = 0;
    int AssignColor[4] = { 0,0,0,0 };
    signed int CameraRotation[3] = { 0,0,0 };

    float TeamBlastBar = 0.0f, FlyBar = 0.0f, MoonJumpForce = 5.0f;
    float HortizonalAccelerationForce = 5.0f;
    float SpeedSizeX = 1.0f, SpeedSizeY = 1.0f, SpeedSizeZ = 1.0f;
    float FlySizeX = 1.0f, FlySizeY = 1.0f, FlySizeZ = 1.0f;
    float PowerSizeX = 1.0f, PowerSizeY = 1.0f, PowerSizeZ = 1.0f;
    float SHMEOpacityBefore = 1.0f, SHMEOpacityAfter = 1.0f;
    float CharacterPosX = 0.0f, CharacterPosY = 0.0f, CharacterPosZ = 0.0f;
    float TPCharacterPosX = 0.0f, TPCharacterPosY = 100.0f, TPCharacterPosZ = 0.0f;
    float CameraPosition[3] = { 0.0f,0.0f,0.0f }, ColorSelect[4] = { 0.0f,0.0f,0.0f,1.0f };

    char MoonJumpHotkey[2] = {'E','\0'}, HorizontalAccelerationHotkey[2] = { 'R','\0' };
    char ColorRamAdress[10] = "8C729C";

    const char* CharacterOverrideListbox[12] = { "Sonic\0","Knuckles\0","Tails\0",
                                                 "Shadow\0","Omega\0","Rouge\0",
                                                 "Amy\0","Big\0","Cream\0",
                                                 "Espio\0","Vector\0","Charmy\0" };

    int CharacterOverrideListboxSelect[4][3];
    for (short i = 0; i < 4; i++) {
        for (short j = 0; j < 3; j++) {
            CharacterOverrideListboxSelect[i][j] = j + i*3;
        }
    }

    const char* StageCodesListbox[38] = { "Dont Override" ,"Seaside Hill" ,"Ocean Place" ,
                                          "Grand Metropolis" ,"Power Plant" ,"Casino Park" ,
                                          "Bingo Highway" ,"Rail Canyon" ,"Bullet Station" ,
                                          "Frog Forest" ,"Lost Jungle" ,"Hang Castle" ,
                                          "Mystic Mansion" ,"14 - Egg Fleet" ,"Final Fortress" ,
                                          "EGG HAWK" ,"TEAM ?? 1" ,"ROBOT CARNIVAL" ,
                                          "EGG ALBATROS" ,"TEAM ?? 2" ,"ROBOT STORM" ,
                                          "EGG EMPEROR" ,"METAL MADNESS" ,"METAL SONIC" ,
                                          "Bonus Stage 1" ,"Bonus Stage 2" ,"Bonus Stage 3" ,
                                          "Bonus Stage 4" ,"Bonus Stage 5" ,"Bonus Stage 6" ,
                                          "Bonus Stage 7" ,"Emerald Challange 1" ,"Emerald Challange 2" ,
                                          "Emerald Challange 3" ,"Emerald Challange 4" ,"Emerald Challange 5" ,
                                          "Emerald Challange 6" ,"Emerald Challange 7" };
    int StageCodesListboxSelect = 0;

    const char* TeamCodesListbox[5] = { "Dont Override", "Team Sonic","Team Dark", "Team Rose", "Team Chaotix" };
    int TeamCodesListboxSelect = 0;

    // Charcter Adress
    int CharacterAdress[4][3] = { {0x005AAC87,0x005AAC9C,0x005AACB1},
                                  {0x005AACC6,0x005AACD8,0x005AACED},
                                  {0x005AAD02,0x005AAD14,0x005AAD26},
                                  {0x005AAD38,0x005AAD60,0x005AAD88} };

    // Character Calls
    int CharacterCalls[4][3] = { {0x005CB170,0x005B6FB0,0x005C0F20},
                                 {0x005CB510,0x005B7220,0x005C1220},
                                 {0x005CB7D0,0x005B7580,0x005C1580},
                                 {0x005CBB40,0x005B7940,0x005C1890} };
    int CharacterCallsReset[4][3] = { {0x005CB170,0x005B6FB0,0x005C0F20},
                                      {0x005CB510,0x005B7220,0x005C1220},
                                      {0x005CB7D0,0x005B7580,0x005C1580},
                                      {0x005CBB40,0x005B7940,0x005C1890} };

    // Character Change Varible

    int CharacterChangeVarible[4][3];
    for (short i = 0; i < 4; i++) {
        for (short j = 0; j < 3; j++) {
            CharacterChangeVarible[i][j] = 0;
        }
    }

    if (!glfwInit()) {
        return -1;
    }

    int WindowWidth = 1024, WindowHeight = 720;
    float WidthRatio = 1.0f, HeightRatio = 1.0f;
    GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "Sonic Heroes Memory Editor", NULL, NULL);

    int width, height, channel;
    unsigned char* pixels = stbi_load("SHico.png", &width, &height, &channel, 4);
    GLFWimage images[1];
    images[0].width = width;
    images[0].height = height;
    images[0].pixels = pixels;
    glfwSetWindowIcon(window, 1, images);
    // stbi_image_free(images[0].pixels);

    if (window == NULL) {
        std::cout << "Could not create window";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, 1);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

    DWORD AllCharacterPowerAddress = 0x0064C268;
    DWORD FlyBarAddress = 0x00675390;
    DWORD ActiveCharacterAddress = 0x006778AC;
    DWORD CommonPointerAddress = 0x005CE820;
    DWORD CameraFreezeAdress = 0x000041F4;

    std::vector<DWORD> AllCharacterPowerOffset{ 0x208 };
    std::vector<DWORD> FlyBarOffset{ 0x6C };
    std::vector<DWORD> ActiveCharacterOffset{ 0x18, 0x148 };
    std::vector<DWORD> MoonJumpOffset{ 0xe0 };
    std::vector<DWORD> HorizontalAccelerationOffset{ 0xdc };
    std::vector<DWORD> CharacterSizeOffset{ 0x100 };
    std::vector<DWORD> CameraFreezeOffset{ 0x0 };

    DWORD ProcessIDSonicHeroes;
    DWORD SonicHeroesBaseAdress;
    DWORD UseAllCharacterPower;
    DWORD UseActiveCharacter;
    DWORD UseFlyBar;
    DWORD UseMoonJump;
    DWORD UseSpeedSizeX;
    DWORD UseFlySizeX;
    DWORD UsePowerSizeX;
    DWORD ActiveCharacterPosMainAddress;
    DWORD UseActiveCharacterPosX;
    DWORD UseActiveCharacterPosY;
    DWORD UseActiveCharacterPosZ;

    HANDLE HandleSonicHeroes;
    HWND hwnd_SonicHeroesTM;

    while (!glfwWindowShouldClose(window)){
        WidthRatio = WindowWidth;
        HeightRatio = WindowHeight;
        glfwGetWindowSize(window, &WindowWidth, &WindowHeight);
        WidthRatio = WindowWidth / WidthRatio;
        HeightRatio = WindowHeight / HeightRatio;
        hwnd_SonicHeroesTM = FindWindowA(NULL, "SONIC HEROES(TM)"); // HWND SH

        while (hwnd_SonicHeroesTM == NULL)
        {
            hwnd_SonicHeroesTM = FindWindowA(NULL, "SONIC HEROES(TM)"); // HWND SH

            glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Background Color Red (SH OFF)
            glClear(GL_COLOR_BUFFER_BIT);
            glfwSwapBuffers(window);
            glfwPollEvents();
            Sleep(100);

            if (!glfwWindowShouldClose(window)) {
            }
            else {
                glfwTerminate();
                return -1;
            }

            if (hwnd_SonicHeroesTM == NULL) {
                glfwSetWindowTitle(window, "Sonic Heroes Memory Editor (Sonic Heroes OFF)");
            }
            else {
                glfwSetWindowTitle(window, "Sonic Heroes Memory Editor");
                break;
            }
        }

        ProcessIDSonicHeroes = NULL;
        GetWindowThreadProcessId(hwnd_SonicHeroesTM, &ProcessIDSonicHeroes); // ProcessID SH

        HandleSonicHeroes = NULL;
        HandleSonicHeroes = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessIDSonicHeroes); // Handle SH

        char SonicHeroesGameModule[] = "Tsonic_win.exe";
        SonicHeroesBaseAdress = GetModuleBaseAddress(_T(SonicHeroesGameModule), ProcessIDSonicHeroes); // Sonic Heroes Base Address

        // Stage open?
        ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x007C6BD4, &RunStage, sizeof(bool), 0);

        glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // Background Color Green (SH ON)
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame(); // Buranýn Altýna Form Elemanlarý Gelecek

        ImGui::Begin("Sonic Heroes Memory Editor");
        ImGui::SetWindowPos(ImVec2(WindowWidth* WidthRatio / 1.6f, 0.0f));
        ImGui::SetWindowSize(ImVec2(WindowWidth * WidthRatio / 2.63f, WindowHeight * HeightRatio / 4));
        ImGui::Text("It is recommended that the Stage and Speed Character\nbe selected when opening the program.");
        ImGui::Text("\nOtherwise there may be some problems.");
        ImGui::Text("The settings that can be changed inside and\noutside the Stage are different");
        ImGui::SetNextItemWidth(200.0f);
        ImGui::SliderFloat("Application Opacity", &SHMEOpacityAfter, 0.3f, 1.0f);
        if (SHMEOpacityBefore != SHMEOpacityAfter){
            glfwSetWindowOpacity(window, SHMEOpacityAfter);
            SHMEOpacityBefore = SHMEOpacityAfter;
        }

        ImGui::End();

        if (RunStage == 0){
            
            UseAllCharacterPower = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, AllCharacterPowerAddress, AllCharacterPowerOffset);
            UseFlyBar = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, FlyBarAddress, FlyBarOffset);
            UseActiveCharacter = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, ActiveCharacterAddress, ActiveCharacterOffset);
            UseMoonJump = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, CommonPointerAddress, MoonJumpOffset);
            
            // Reads
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD72C, &TeamBlastBar, sizeof(TeamBlastBar), 0);
            if (RingCheck == 0)
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD70C, &Ring, sizeof(Ring), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD74C, &Live, sizeof(Live), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseAllCharacterPower, &SpeedCharacterPower, sizeof(bool), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)(UseAllCharacterPower + 0x1), &FlyCharacterPower, sizeof(bool), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)(UseAllCharacterPower + 0x1 + 0x1), &PowerCharacterPower, sizeof(bool), 0);
            if (InfiniteFlyCheck == 0) {
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseFlyBar, &FlyBar, sizeof(float), 0);
            }
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseActiveCharacter, &ActiveCharacter, sizeof(int), 0);
            if (TimeFreezeCheck == 0) {
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD70A, &Time[0], sizeof(char), 0); // Minute
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD709, &Time[1], sizeof(char), 0); // Second
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD708, &Time[2], sizeof(char), 0); // Split Second
            }

            ImGui::Begin("Stage On");
            ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SetWindowSize(ImVec2(WindowWidth * WidthRatio / 2.84f,WindowHeight * HeightRatio));

            ImGui::Checkbox("Team Blast Always On", &TeamBlast);
            if (ImGui::Button("Off Team Blast", ImVec2(130.0f, 40.0f))) {
                if (TeamBlastBar > 91.0f) {
                    TeamBlastBar = 91.0f;
                }
                else {
                }
                TeamBlast = 0;
            }

            ImGui::Text("Team Blast Bar");
            ImGui::SetNextItemWidth(300.0f);
            ImGui::SliderFloat("0-91", &TeamBlastBar, 0.0f, 91.0f);

            ImGui::Text("Fly Bar");
            ImGui::SetNextItemWidth(300.0f);
            ImGui::SliderFloat("0 - 180", &FlyBar, 0.0f, 180.0f);
            ImGui::Checkbox("Infinite Fly", &InfiniteFlyCheck);

            ImGui::PushItemWidth(100.0f);
            ImGui::InputInt("Ring 0 - 999", &Ring);
            ImGui::Checkbox("Ring Freeze", &RingCheck);
            ImGui::InputInt("Live 0 - 99", &Live);

            ImGui::PushItemWidth(80.0f);
            ImGui::SliderInt("Character Powers Speed", &SpeedCharacterPower, 0, 3);
            ImGui::SliderInt("Character Powers Fly", &FlyCharacterPower, 0, 3);
            ImGui::SliderInt("Character Powers Power", &PowerCharacterPower, 0, 3);
            
            ImGui::SliderInt("Active Role", &ActiveCharacter, 0, 2);

            ImGui::Text("Time Freeze is a bit buggy");
            ImGui::InputInt3("Time m/s/ss ", Time);
            ImGui::Checkbox("Time Freeze", &TimeFreezeCheck);

            ImGui::Text("Moon Jump Hotkey (Upper Case)");
            ImGui::InputText("Default 'E'", &MoonJumpHotkey[0], 2);
            ImGui::InputFloat("Moon Jump Force", &MoonJumpForce);
            ImGui::Text("Horizontal Acceleration Hotkey (Upper Case)");
            ImGui::InputText("Default 'R'", &HorizontalAccelerationHotkey[0], 2);
            ImGui::InputFloat("Horizontal Acceleration Force", &HortizonalAccelerationForce);
            ImGui::Checkbox("Character Size Edit (Experimental)",&SizeEditCheck);
            ImGui::Checkbox("Active Character Position Edit", &CharacterPosEdit);
            ImGui::Checkbox("Character Point Edit", &CharacterPointEdit);
            ImGui::Checkbox("Camera Edit", &CameraEditCheckbox);
            ImGui::Checkbox("Color Edit", &ColorEditCheck);

            ImGui::End();

            if (SizeEditCheck == 1){
                UseSpeedSizeX = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, CommonPointerAddress, CharacterSizeOffset);
                UseFlySizeX = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, CommonPointerAddress + 0x4, CharacterSizeOffset);
                UsePowerSizeX = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, CommonPointerAddress + 0x8, CharacterSizeOffset);

                ImGui::Begin("Character Size Edit (Experimental)");
                ImGui::SetWindowPos(ImVec2(WindowWidth * WidthRatio / 2.84f, 0.0f));
                ImGui::SetWindowSize(ImVec2(WindowWidth * WidthRatio / 3.65f, WindowHeight * HeightRatio / 2.4f));
                ImGui::PushItemWidth(60.0f);

                ImGui::Text("Speed Character Size");
                ImGui::InputFloat("Speed Character Size X", &SpeedSizeX);
                ImGui::InputFloat("Speed Character Size Y", &SpeedSizeY);
                ImGui::InputFloat("Speed Character Size Z", &SpeedSizeZ);

                ImGui::Text("Fly Character Size");
                ImGui::InputFloat("Fly Character Size X", &FlySizeX);
                ImGui::InputFloat("Fly Character Size Y", &FlySizeY);
                ImGui::InputFloat("Fly Character Size Z", &FlySizeZ);

                ImGui::Text("Power Character Size");
                ImGui::InputFloat("Power Character Size X", &PowerSizeX);
                ImGui::InputFloat("Power Character Size Y", &PowerSizeY);
                ImGui::InputFloat("Power Character Size Z", &PowerSizeZ);

                ImGui::End();

                // Speed
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UseSpeedSizeX, &SpeedSizeX, sizeof(SpeedSizeX), 0);
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(UseSpeedSizeX + 0x4), &SpeedSizeY, sizeof(SpeedSizeY), 0);
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(UseSpeedSizeX + 0x8), &SpeedSizeZ, sizeof(SpeedSizeZ), 0);
                // Fly
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UseFlySizeX, &FlySizeX, sizeof(FlySizeX), 0);
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(UseFlySizeX + 0x4), &FlySizeY, sizeof(FlySizeY), 0);
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(UseFlySizeX + 0x8), &FlySizeZ, sizeof(FlySizeZ), 0);
                // Power
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UsePowerSizeX, &PowerSizeX, sizeof(PowerSizeX), 0);
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(UsePowerSizeX + 0x4), &PowerSizeY, sizeof(PowerSizeY), 0);
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(UsePowerSizeX + 0x8), &PowerSizeZ, sizeof(PowerSizeZ), 0);
            }

            if (CharacterPosEdit == 1){
                ActiveCharacterPosMainAddress = 0x005CE820;

                std::vector<DWORD> ActiveCharacterPosXOffset{ 0xe8 };
                std::vector<DWORD> ActiveCharacterPosYOffset{ 0xec };
                std::vector<DWORD> ActiveCharacterPosZOffset{ 0xf0 };

                UseActiveCharacterPosX = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, ActiveCharacterPosMainAddress, ActiveCharacterPosXOffset);
                UseActiveCharacterPosY = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, ActiveCharacterPosMainAddress, ActiveCharacterPosYOffset);
                UseActiveCharacterPosZ = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, ActiveCharacterPosMainAddress, ActiveCharacterPosZOffset);

                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseActiveCharacterPosX, &CharacterPosX, sizeof(CharacterPosX), 0);
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseActiveCharacterPosY, &CharacterPosY, sizeof(CharacterPosY), 0);
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseActiveCharacterPosZ, &CharacterPosZ, sizeof(CharacterPosZ), 0);

                ImGui::Begin("Active Character Position");
                ImGui::SetWindowPos(ImVec2(WindowWidth* WidthRatio / 2.84f, WindowHeight* HeightRatio / 2.4f));
                ImGui::SetWindowSize(ImVec2(WindowWidth* WidthRatio / 3.0f, WindowHeight* HeightRatio / 3.5f));

                ImGui::PushItemWidth(106.0f);

                ImGui::Text("X = %f\nY = %f\nZ = %f", CharacterPosX, CharacterPosY, CharacterPosZ);
                ImGui::Text("\n Teleport This Position;");
                ImGui::InputFloat("X", &TPCharacterPosX);
                ImGui::InputFloat("Y", &TPCharacterPosY);
                ImGui::InputFloat("Z", &TPCharacterPosZ);
                if (ImGui::Button("Teleport There")){
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UseActiveCharacterPosX, &TPCharacterPosX, sizeof(TPCharacterPosX), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UseActiveCharacterPosY, &TPCharacterPosY, sizeof(TPCharacterPosY), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UseActiveCharacterPosZ, &TPCharacterPosZ, sizeof(TPCharacterPosZ), 0);
                }

                ImGui::End();
            }

            if (CharacterPointEdit == 1){
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD6C0, &CharacterSpeedPoint, sizeof(CharacterSpeedPoint), 0);
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD6C4, &CharacterFlyPoint, sizeof(CharacterFlyPoint), 0);
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD6C8, &CharacterPowerPoint, sizeof(CharacterPowerPoint), 0);

                ImGui::Begin("Character Point Edit");
                ImGui::SetWindowPos(ImVec2(WindowWidth * WidthRatio / 1.6f, WindowHeight * HeightRatio / 4));
                ImGui::SetWindowSize(ImVec2(WindowWidth * WidthRatio / 2.63f, WindowHeight * HeightRatio / 6));

                ImGui::PushItemWidth(150.0f);

                ImGui::InputInt("Speed Character Point", &CharacterSpeedPoint);
                ImGui::InputInt("Fly Character Point", &CharacterFlyPoint);
                ImGui::InputInt("Power Character Point", &CharacterPowerPoint);

                ImGui::End();

                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD6C0, &CharacterSpeedPoint, sizeof(CharacterSpeedPoint), 0);
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD6C4, &CharacterFlyPoint, sizeof(CharacterFlyPoint), 0);
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD6C8, &CharacterPowerPoint, sizeof(CharacterPowerPoint), 0);

            }

            if (CameraEditCheckbox == 1) {
                ImGui::Begin("Camera Edit");

                ImGui::SetWindowPos(ImVec2(WindowWidth * WidthRatio / 2.84f, WindowHeight * HeightRatio * 59 / 84));
                ImGui::SetWindowSize(ImVec2(WindowWidth * WidthRatio / 3.0f, WindowHeight * HeightRatio / 3.33f));

                ImGui::PushItemWidth(240.0f);
                ImGui::Checkbox("Camera Freeze", &CameraFreeze);
                if (CameraFreeze == 1) {
                    CameraFreeze = !CameraFreeze;
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A69880, &CameraFreeze, sizeof(short), 0);
                    CameraFreeze = !CameraFreeze;
                }
                else {
                    CameraFreeze = !CameraFreeze;
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A69880, &CameraFreeze, sizeof(short), 0);
                    CameraFreeze = !CameraFreeze;
                }

                for (short i = 0;i < 3;i++) {
                    // Camera Position
                    ReadProcessMemory(HandleSonicHeroes, (PBYTE*)(0x00A60C30 + 0x4 * i), &CameraPosition[i], sizeof(float), 0);
                    // Camera Rotation
                    ReadProcessMemory(HandleSonicHeroes, (PBYTE*)(0x00A60C44 - 0x4 * i), &CameraRotation[i], sizeof(int), 0);
                }
                
                ImGui::Text("Camera Position X/Y/Z");
                ImGui::InputFloat3("CPos", CameraPosition, "%.2f");
                ImGui::Text("Camera Rotation Min: -32768 Max: 32767");
                ImGui::InputInt3("CRot", CameraRotation);

                for (short i = 0; i < 3; i++) {
                    if (CameraRotation[i] > 32767)
                        CameraRotation[i] = 32767;

                    if (CameraRotation[i] < -32768)
                        CameraRotation[i] = -32768;
                }

                for (short i = 0;i < 3;i++) {
                    // Camera Position
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(0x00A60C30 + 0x4 * i), &CameraPosition[i], sizeof(float), 0);
                    // Camera Rotation
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(0x00A60C44 - 0x4 * i), &CameraRotation[i], sizeof(int), 0);
                }
                ImGui::End();
            }

            if (ColorEditCheck == 1) {
                ImGui::Begin("Color Edit");
                ImGui::SetWindowPos(ImVec2(WindowWidth* WidthRatio / 1.46, WindowHeight* HeightRatio * 5 / 12));
                ImGui::SetWindowSize(ImVec2(WindowWidth* WidthRatio / 3.1f, WindowHeight* HeightRatio * 7 / 12 * 1.01f));
                ImGui::PushItemWidth(220.0f);

                ImGui::ColorPicker4("Color Select", ColorSelect);
                ImGui::Text("Write the Color Code address below.\n(without 0x)");
                ImGui::Text("If you enter an incorrect, invalid or unknown\naddress,the application and game may crash!");
                ImGui::InputText("",ColorRamAdress,10);
                if (ImGui::Button("Assign Color")) {
                    for (short i = 0;i < 4;i++) {
                        AssignColor[i] = ColorSelect[i] * 255;
                        WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(std::stoi(ColorRamAdress, nullptr, 16) + i), &AssignColor[i], sizeof(unsigned char), 0);
                    }
                }

                ImGui::End();
            }

            // Writes
            if (TeamBlastBar > 91.0f) {
            }
            else {
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD73C, &TeamBlast, sizeof(TeamBlast), 0);
            }
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD70A, &Time[0], sizeof(char), 0); // Minute
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD709, &Time[1], sizeof(char), 0); // Second
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD708, &Time[2], sizeof(char), 0); // Split Second
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD72C, &TeamBlastBar, sizeof(TeamBlastBar), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD70C, &Ring, sizeof(Ring), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD74C, &Live, sizeof(Live), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UseAllCharacterPower, &SpeedCharacterPower, sizeof(bool), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(UseAllCharacterPower + 0x1), &FlyCharacterPower, sizeof(bool), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(UseAllCharacterPower + 0x1 + 0x1), &PowerCharacterPower, sizeof(bool), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UseFlyBar, &FlyBar, sizeof(FlyBar), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UseActiveCharacter, &ActiveCharacter, sizeof(ActiveCharacter), 0);

            if (GetAsyncKeyState(int(MoonJumpHotkey[0])))
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UseMoonJump, &MoonJumpForce, sizeof(float), 0);
            
            if (GetAsyncKeyState(int(HorizontalAccelerationHotkey[0])))
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(UseMoonJump - 0x4), &HortizonalAccelerationForce, sizeof(float), 0);

        }

        else{
            ImGui::Begin("Stage Off - Overrides");
            ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SetWindowSize(ImVec2(WindowWidth* WidthRatio / 2.84f, WindowHeight* HeightRatio));
            ImGui::PushItemWidth(210.0f);

            ImGui::Text("All overrides may cause the game to crash!");
            ImGui::Text(" ");
            ImGui::Text("Character Override");
            if (ImGui::Button("Character Change")) {

                for (short i = 0; i < 4; i++) {
                    for (short j = 0; j < 3; j++) {
                        CharacterChangeVarible[i][j] = CharacterChanger(CharacterAdress[i][j], CharacterCalls[i][j]);
                        WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(CharacterAdress[i][j] + 0x1), &CharacterChangeVarible[i][j], sizeof(int), 0);
                    }
                }
            }

            if (ImGui::Button("Character Override Reset")) {

                for (short i = 0;i < 4;i++) {
                    for (short j = 0;j < 3;j++) {
                        CharacterCalls[i][j] = CharacterCallsReset[i][j];
                        CharacterChangeVarible[i][j] = CharacterChanger(CharacterAdress[i][j], CharacterCalls[i][j]);
                        CharacterOverrideListboxSelect[i][j] = i * 3 + j;
                        WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(CharacterAdress[i][j] + 0x1), &CharacterChangeVarible[i][j], sizeof(int), 0);
                    }
                }
            }

            for (short i = 0; i < 4; i++) {
                for (short j = 0; j < 3; j++) {
                    ImGui::Combo(CharacterOverrideListbox[i * 3 + j], &CharacterOverrideListboxSelect[i][j], CharacterOverrideListbox, IM_ARRAYSIZE(CharacterOverrideListbox));
                    CharacterCalls[i][j] = CharacterCallsReset[(CharacterOverrideListboxSelect[i][j] - (CharacterOverrideListboxSelect[i][j] % 3)) / 3][CharacterOverrideListboxSelect[i][j] % 3];
                }
                ImGui::Text("");
            }

            ImGui::Text("Stage - Team Override");
            ImGui::Combo("Stage Override", &StageCodesListboxSelect, StageCodesListbox, IM_ARRAYSIZE(StageCodesListbox));
            switch (StageCodesListboxSelect) {
            case 1:
                OverrideStageCode = 2;
                break;
            case 2:
                OverrideStageCode = 3;
                break;
            case 3:
                OverrideStageCode = 4;
                break;
            case 4:
                OverrideStageCode = 5;
                break;
            case 5:
                OverrideStageCode = 6;
                break;
            case 6:
                OverrideStageCode = 7;
                break;
            case 7:
                OverrideStageCode = 8;
                break;
            case 8:
                OverrideStageCode = 9;
                break;
            case 9:
                OverrideStageCode = 10;
                break;
            case 10:
                OverrideStageCode = 11;
                break;
            case 11:
                OverrideStageCode = 12;
                break;
            case 12:
                OverrideStageCode = 13;
                break;
            case 13:
                OverrideStageCode = 14;
                break;
            case 14:
                OverrideStageCode = 15;
                break;
            case 15:
                OverrideStageCode = 16;
                break;
            case 16:
                OverrideStageCode = 17;
                break;
            case 17:
                OverrideStageCode = 18;
                break;
            case 18:
                OverrideStageCode = 19;
                break;
            case 19:
                OverrideStageCode = 20;
                break;
            case 20:
                OverrideStageCode = 21;
                break;
            case 21:
                OverrideStageCode = 22;
                break;
            case 22:
                OverrideStageCode = 23;
                break;
            case 23:
                OverrideStageCode = 24;
                break;
            case 24:
                OverrideStageCode = 29;
                break;
            case 25:
                OverrideStageCode = 30;
                break;
            case 26:
                OverrideStageCode = 31;
                break;
            case 27:
                OverrideStageCode = 32;
                break;
            case 28:
                OverrideStageCode = 33;
                break;
            case 29:
                OverrideStageCode = 34;
                break;
            case 30:
                OverrideStageCode = 35;
                break;
            case 31:
                OverrideStageCode = 52;
                break;
            case 32:
                OverrideStageCode = 53;
                break;
            case 33:
                OverrideStageCode = 54;
                break;
            case 34:
                OverrideStageCode = 55;
                break;
            case 35:
                OverrideStageCode = 56;
                break;
            case 36:
                OverrideStageCode = 57;
                break;
            case 37:
                OverrideStageCode = 58;
                break;
            default:
                OverrideStageCode = 1;
                break;
            }

            ImGui::Combo("Team Override", &TeamCodesListboxSelect, TeamCodesListbox, IM_ARRAYSIZE(TeamCodesListbox));
            switch (TeamCodesListboxSelect) {
            case 1:
                OverrideTeamCode = 0;
                break;
            case 2:
                OverrideTeamCode = 1;
                break;
            case 3:
                OverrideTeamCode = 2;
                break;
            case 4:
                OverrideTeamCode = 3;
                break;
            default:
                OverrideTeamCode = -1;
                break;
            }

            ImGui::End();

            if (OverrideStageCode != 1){
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x008D6720, &OverrideStageCode, sizeof(OverrideStageCode), 0);
            }

            if (OverrideTeamCode != -1){
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x008D6920, &OverrideTeamCode, sizeof(OverrideTeamCode), 0);
            }
            
        }

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();

        Sleep(1000 / FrameRateLock); // Frame Rate Lock 1000/FPS
    }

    return 0;
}