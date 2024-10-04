#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>
#include <Windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <math.h>
#include <tchar.h>
#include <stdlib.h>

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

    const int FrameRateLock = 120;

    bool StageCodesMenu = 0, TeamCodesMenu = 0, SizeEditCheck = 0, CharacterPosEdit = 0, CharacterPointEdit = 0;
    bool TeamBlast = 0, RingCheck = 0, InfiniteFlyCheck = 0, AdvancedMenu = 0, TimeFreezeCheck = 0;
    bool CharacterChangerMenu = 0, CharacterChangerCodeMenu = 0;

    int RunStage = 0, Ring = 0, Live = 0, RingLock = 1, OverrideStageCode = -1, OverrideTeamCode = -1;
    int SpeedCharacterPower = 0x0, FlyCharacterPower = 0x0, PowerCharacterPower = 0x0;
    int ActiveCharacter = 0, Time = 0, TimeFreeze = 0;
    int CharacterSpeedPoint = 0, CharacterFlyPoint = 0, CharacterPowerPoint = 0;

    float TeamBlastBar = 0.0f, FlyBar = 0.0f, MoonJump = 0.0f, MoonJumpForce = 5.0f;
    float HortizonalAcceleration = 0.0f, HortizonalAccelerationForce = 5.0f;
    float SpeedSizeX = 1.0f, SpeedSizeY = 1.0f, SpeedSizeZ = 1.0f;
    float FlySizeX = 1.0f, FlySizeY = 1.0f, FlySizeZ = 1.0f;
    float PowerSizeX = 1.0f, PowerSizeY = 1.0f, PowerSizeZ = 1.0f;
    float SHMEOpacityBefore = 1.0f, SHMEOpacityAfter = 1.0f;
    float CharacterPosX = 0.0f, CharacterPosY = 0.0f, CharacterPosZ = 0.0f;
    float TPCharacterPosX = 0.0f, TPCharacterPosY = 100.0f, TPCharacterPosZ = 0.0f;

    char MoonJumpHotkey = 'E', HorizontalAccelerationHotkey = 'R';

    // Charcter Adress
    int Sonic = 0x005AAC87;
    int Knuckles = 0x005AAC9C;
    int Tails = 0x005AACB1;

    int Shadow = 0x005AACC6;
    int Omega = 0x005AACD8;
    int Rouge = 0x005AACED;

    int Amy = 0x005AAD02;
    int Big = 0x005AAD14;
    int Cream = 0x005AAD26;

    int Espio = 0x005AAD38;
    int Vector = 0x005AAD60;
    int Bee = 0x005AAD88;

    // Character Calls

    int SonicC = 0x005CB170;
    int KnucklesC = 0x005B6FB0;
    int TailsC = 0x005C0F20;

    int ShadowC = 0x005CB510;
    int OmegaC = 0x005B7220;
    int RougeC = 0x005C1220;

    int AmyC = 0x005CB7D0;
    int BigC = 0x005B7580;
    int CreamC = 0x005C1580;

    int EspioC = 0x005CBB40;
    int VectorC = 0x005B7940;
    int BeeC = 0x005C1890;

    int WrSonic = 0;
    int WrKnuckles = 0;
    int WrTails = 0;

    int WrShadow = 0;
    int WrOmega = 0;
    int WrRouge = 0;

    int WrAmy = 0;
    int WrBig = 0;
    int WrCream = 0;

    int WrEspio = 0;
    int WrVector = 0;
    int WrBee = 0;

    if (!glfwInit()) {
        return -1;
    }
    else {
    }

    GLFWwindow* window = glfwCreateWindow(1200, 600, "Sonic Heroes Memory Editor", NULL, NULL);

    if (window == NULL) {
        std::cout << "Could not create window";
        glfwTerminate();
        return -1;
    }
    else {
    }

    glfwMakeContextCurrent(window);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, 1);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

    DWORD AllCharacterPowerAddress = 0x0064C268; // 0x0S 0x000F 0x00000P
    DWORD FlyBarAddress = 0x00675390;
    DWORD ActiveCharacterAddress = 0x006778AC;
    DWORD MoonJumpAddress = 0x005CE820;
    DWORD HorizontalAccelerationAddress = 0x005CE820;
    DWORD SpeedSizeAdress = 0x005CE820;
    DWORD FlySizeAdress = 0x005CE824;
    DWORD PowerSizeAdress = 0x005CE828;
    DWORD CameraFreezeAdress = 0x000041F4;

    std::vector<DWORD> AllCharacterPowerOffset{ 0x208 };
    std::vector<DWORD> FlyBarOffset{ 0x6C };
    std::vector<DWORD> ActiveCharacterOffset{ 0x18, 0x148 };
    std::vector<DWORD> MoonJumpOffset{ 0xe0 };
    std::vector<DWORD> HorizontalAccelerationOffset{ 0xdc };
    std::vector<DWORD> SpeedSizeOffset{ 0x100 };
    std::vector<DWORD> FlySizeOffset{ 0x100 };
    std::vector<DWORD> PowerSizeOffset{ 0x100 };
    std::vector<DWORD> CameraFreezeOffset{ 0x0 };

    while (!glfwWindowShouldClose(window)){

        HWND hwnd_SonicHeroesTM = FindWindowA(NULL, "SONIC HEROES(TM)"); // HWND SH

        while (hwnd_SonicHeroesTM == NULL)
        {
            HWND hwnd_SonicHeroesTM = FindWindowA(NULL, "SONIC HEROES(TM)"); // HWND SH

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
            }
            else {
                break;
            }
        }

        DWORD ProcessIDSonicHeroes = NULL;
        GetWindowThreadProcessId(hwnd_SonicHeroesTM, &ProcessIDSonicHeroes); // ProcessID SH

        HANDLE HandleSonicHeroes = NULL;
        HandleSonicHeroes = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessIDSonicHeroes); // Handle SH

        char SonicHeroesGameModule[] = "Tsonic_win.exe";
        DWORD SonicHeroesBaseAdress = GetModuleBaseAddress(_T(SonicHeroesGameModule), ProcessIDSonicHeroes); // Sonic Heroes Base Address

        // Stage open?
        ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x007C6BD4, &RunStage, sizeof(bool), 0);

        glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // Background Color Green (SH ON)
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame(); // Buranýn Altýna Form Elemanlarý Gelecek

        ImGui::Begin("Sonic Heroes Memory Editor");
        ImGui::Text("It is recommended that the Stage and Speed Character\nbe selected when opening the program.");
        ImGui::Text("\nOtherwise there may be some problems.");
        ImGui::Text("The settings that can be changed inside and\noutside the Stage are different");
        ImGui::SetNextItemWidth(200.0f);
        ImGui::SliderFloat("Application Opacity", &SHMEOpacityAfter, 0.3f, 1.0f);
        if (SHMEOpacityBefore != SHMEOpacityAfter){
            glfwSetWindowOpacity(window, SHMEOpacityAfter);
            SHMEOpacityBefore = SHMEOpacityAfter;
        }
        else{
        }

        ImGui::End();

        if (RunStage == 0){
            
            DWORD UseAllCharacterPower = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, AllCharacterPowerAddress, AllCharacterPowerOffset);
            DWORD UseFlyBar = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, FlyBarAddress, FlyBarOffset);
            DWORD UseActiveCharacter = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, ActiveCharacterAddress, ActiveCharacterOffset);
            DWORD UseMoonJump = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, MoonJumpAddress, MoonJumpOffset);
            
            // Reads
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD72C, &TeamBlastBar, sizeof(TeamBlastBar), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD70C, &Ring, sizeof(Ring), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD74C, &Live, sizeof(Live), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseAllCharacterPower, &SpeedCharacterPower, sizeof(bool), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)(UseAllCharacterPower + 0x1), &FlyCharacterPower, sizeof(bool), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)(UseAllCharacterPower + 0x1 + 0x1), &PowerCharacterPower, sizeof(bool), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseFlyBar, &FlyBar, sizeof(FlyBar), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseActiveCharacter, &ActiveCharacter, sizeof(ActiveCharacter), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseMoonJump, &MoonJump, sizeof(MoonJump), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)(UseMoonJump - 0x4), &HortizonalAcceleration, sizeof(HortizonalAcceleration), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD708, &Time, sizeof(Time), 0);

            ImGui::Begin("Stage On");

            ImGui::Checkbox("Team Blast Always On", &TeamBlast);
            if (ImGui::Button("Off Team Blast", ImVec2(130, 40))) {
                if (TeamBlastBar > 91.0f) {
                    TeamBlastBar = 91.0f;
                }
                else {
                }
                TeamBlast = 0;
            }
            else {
            }

            ImGui::SetNextItemWidth(300.0f);
            ImGui::SliderFloat("Team Blast Bar", &TeamBlastBar, 0.0f, 91.0f);

            ImGui::PushItemWidth(100.0f);
            ImGui::InputInt("Ring 0 - 999", &Ring);
            ImGui::InputInt("Ring Freeze Ring Ammo 0 - 999", &RingLock);
            ImGui::Checkbox("Ring Freeze", &RingCheck);
            if (RingCheck == 1) {
                Ring = RingLock;
            }
            else {
            }
            ImGui::InputInt("Live 0 - 99", &Live);

            ImGui::PushItemWidth(80.0f);
            ImGui::SliderInt("Character Powers Speed", &SpeedCharacterPower, 0, 3);
            ImGui::SliderInt("Character Powers Fly", &FlyCharacterPower, 0, 3);
            ImGui::SliderInt("Character Powers Power", &PowerCharacterPower, 0, 3);
            ImGui::Checkbox("Infinite Fly", &InfiniteFlyCheck);
            if (InfiniteFlyCheck == 1) {
                FlyBar = 0.0f;
            }
            else {
            }
            ImGui::SliderInt("Active Role", &ActiveCharacter, 0, 2);
            ImGui::Checkbox("Time Freezee", &TimeFreezeCheck);
            if (TimeFreezeCheck == 1) {
                Time = TimeFreeze;
            }
            else {
                TimeFreeze = Time;
            }

            ImGui::Text("\nIt Works Even If the Texts Are Confused :D\nOnly Uses Initial Letters");
            ImGui::InputText("Moon Jump Hotkey (Upper Case) Default 'E'", &MoonJumpHotkey, 2);
            ImGui::InputFloat("Moon Jump Force", &MoonJumpForce);
            ImGui::InputText("Horizontal Acceleration Hotkey (Upper Case) Default 'R'", &HorizontalAccelerationHotkey, 2);
            ImGui::InputFloat("Horizontal Acceleration Force", &HortizonalAccelerationForce);
            ImGui::Checkbox("Character Size Edit (Experimental)",&SizeEditCheck);
            ImGui::Checkbox("Active Character Position Edit", &CharacterPosEdit);
            ImGui::Checkbox("Character Point Edit", &CharacterPointEdit);

            ImGui::End();

            if (SizeEditCheck == 1){
                DWORD UseSpeedSizeX = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, SpeedSizeAdress, SpeedSizeOffset);
                DWORD UseFlySizeX = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, FlySizeAdress, FlySizeOffset);
                DWORD UsePowerSizeX = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, PowerSizeAdress, PowerSizeOffset);

                ImGui::Begin("Character Size Edit (Experimental)");
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
            else{
            }

            if (CharacterPosEdit == 1){
                DWORD ActiveCharacterPosMainAddress = 0x005CE820;

                std::vector<DWORD> ActiveCharacterPosXOffset{ 0xe8 };
                std::vector<DWORD> ActiveCharacterPosYOffset{ 0xec };
                std::vector<DWORD> ActiveCharacterPosZOffset{ 0xf0 };

                DWORD UseActiveCharacterPosX = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, ActiveCharacterPosMainAddress, ActiveCharacterPosXOffset);
                DWORD UseActiveCharacterPosY = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, ActiveCharacterPosMainAddress, ActiveCharacterPosYOffset);
                DWORD UseActiveCharacterPosZ = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesBaseAdress, ActiveCharacterPosMainAddress, ActiveCharacterPosZOffset);

                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseActiveCharacterPosX, &CharacterPosX, sizeof(CharacterPosX), 0);
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseActiveCharacterPosY, &CharacterPosY, sizeof(CharacterPosY), 0);
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseActiveCharacterPosZ, &CharacterPosZ, sizeof(CharacterPosZ), 0);

                ImGui::Begin("Active Character Position");

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
                else{
                }

                ImGui::End();
            }
            else{
            }

            if (CharacterPointEdit == 1){
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD6C0, &CharacterSpeedPoint, sizeof(CharacterSpeedPoint), 0);
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD6C4, &CharacterFlyPoint, sizeof(CharacterFlyPoint), 0);
                ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD6C8, &CharacterPowerPoint, sizeof(CharacterPowerPoint), 0);

                ImGui::Begin("Character Point Edit");

                ImGui::PushItemWidth(150.0f);

                ImGui::InputInt("Speed Character Point", &CharacterSpeedPoint);
                ImGui::InputInt("Fly Character Point", &CharacterFlyPoint);
                ImGui::InputInt("Power Character Point", &CharacterPowerPoint);

                ImGui::End();

                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD6C0, &CharacterSpeedPoint, sizeof(CharacterSpeedPoint), 0);
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD6C4, &CharacterFlyPoint, sizeof(CharacterFlyPoint), 0);
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD6C8, &CharacterPowerPoint, sizeof(CharacterPowerPoint), 0);

            }
            else{
            }
            
            // Writes
            if (TeamBlastBar > 91.0f) {
            }
            else {
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD73C, &TeamBlast, sizeof(TeamBlast), 0);
            }
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD72C, &TeamBlastBar, sizeof(TeamBlastBar), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD70C, &Ring, sizeof(Ring), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD74C, &Live, sizeof(Live), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UseAllCharacterPower, &SpeedCharacterPower, sizeof(bool), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(UseAllCharacterPower + 0x1), &FlyCharacterPower, sizeof(bool), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(UseAllCharacterPower + 0x1 + 0x1), &PowerCharacterPower, sizeof(bool), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UseFlyBar, &FlyBar, sizeof(FlyBar), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UseActiveCharacter, &ActiveCharacter, sizeof(ActiveCharacter), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x009DD708, &TimeFreeze, sizeof(TimeFreeze), 0);

            if (GetAsyncKeyState(int(MoonJumpHotkey))) {
                MoonJump = MoonJumpForce;
            }
            else {
            }
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)UseMoonJump, &MoonJump, sizeof(MoonJump), 0);
            if (GetAsyncKeyState(int(HorizontalAccelerationHotkey))) {
                HortizonalAcceleration = HortizonalAccelerationForce;
            }
            else {
            }
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(UseMoonJump - 0x4), &HortizonalAcceleration, sizeof(HortizonalAcceleration), 0);

        }

        else{
            ImGui::Begin("Stage Off");

            ImGui::PushItemWidth(200.0f);

            ImGui::Text("Typing an invalid number may result in a crash");
            ImGui::Text("-1 = Dont Override\n\n");
            ImGui::InputInt("Stage Code",&OverrideStageCode);
            ImGui::InputInt("Team Code", &OverrideTeamCode);


            ImGui::Checkbox("Stage Codes Menu", &StageCodesMenu);
            ImGui::Checkbox("Team Codes Menu", &TeamCodesMenu);
            ImGui::Checkbox("Character Changer Menu", &CharacterChangerMenu);

            ImGui::End();

            if (CharacterChangerMenu == 1){

                ImGui::Begin("Character Changer");

                ImGui::PushItemWidth(200.0f);

                ImGui::InputInt("<- Sonic", &SonicC);
                ImGui::InputInt("<- Knuckles", & KnucklesC);
                ImGui::InputInt("<- Tails\n", &TailsC);

                ImGui::InputInt("<- Shadow", &ShadowC);
                ImGui::InputInt("<- Omega", &OmegaC);
                ImGui::InputInt("<- Rouge\n", &RougeC);

                ImGui::InputInt("<- Amy", &AmyC);
                ImGui::InputInt("<- Big", &BigC);
                ImGui::InputInt("<- Cream\n", &CreamC);

                ImGui::InputInt("<- Espio", & EspioC);
                ImGui::InputInt("<- Vector", &VectorC);
                ImGui::InputInt("<- Bee\n", &BeeC);

                ImGui::Checkbox("Character Codes", & CharacterChangerCodeMenu);

                if (ImGui::Button("Character Change")) {

                    WrSonic = CharacterChanger(Sonic,SonicC);
                    WrKnuckles = CharacterChanger(Knuckles, KnucklesC);
                    WrTails = CharacterChanger(Tails, TailsC);

                    WrShadow = CharacterChanger(Shadow, ShadowC);
                    WrOmega = CharacterChanger(Omega, OmegaC);
                    WrRouge = CharacterChanger(Rouge, RougeC);

                    WrAmy = CharacterChanger(Amy, AmyC);
                    WrBig = CharacterChanger(Big, BigC);
                    WrCream = CharacterChanger(Cream, CreamC);

                    WrEspio = CharacterChanger(Espio, EspioC);
                    WrVector = CharacterChanger(Vector, VectorC);
                    WrBee = CharacterChanger(Bee, BeeC);

                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Sonic + 0x1), &WrSonic, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Knuckles + 0x1), &WrKnuckles, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Tails + 0x1), &WrTails, sizeof(int), 0);

                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Shadow + 0x1), &WrShadow, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Omega + 0x1), &WrOmega, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Rouge + 0x1), &WrRouge, sizeof(int), 0);

                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Amy + 0x1), &WrAmy, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Big + 0x1), &WrBig, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Cream + 0x1), &WrCream, sizeof(int), 0);

                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Espio + 0x1), &WrEspio, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Vector + 0x1), &WrVector, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Bee + 0x1), &WrBee, sizeof(int), 0);
                }
                else{
                }

                if (ImGui::Button("Reset")){
                    SonicC = 0x005CB170;
                    KnucklesC = 0x005B6FB0;
                    TailsC = 0x005C0F20;

                    ShadowC = 0x005CB510;
                    OmegaC = 0x005B7220;
                    RougeC = 0x005C1220;

                    AmyC = 0x005CB7D0;
                    BigC = 0x005B7580;
                    CreamC = 0x005C1580;

                    EspioC = 0x005CBB40;
                    VectorC = 0x005B7940;
                    BeeC = 0x005C1890;

                    WrSonic = CharacterChanger(Sonic, SonicC);
                    WrKnuckles = CharacterChanger(Knuckles, KnucklesC);
                    WrTails = CharacterChanger(Tails, TailsC);

                    WrShadow = CharacterChanger(Shadow, ShadowC);
                    WrOmega = CharacterChanger(Omega, OmegaC);
                    WrRouge = CharacterChanger(Rouge, RougeC);

                    WrAmy = CharacterChanger(Amy, AmyC);
                    WrBig = CharacterChanger(Big, BigC);
                    WrCream = CharacterChanger(Cream, CreamC);

                    WrEspio = CharacterChanger(Espio, EspioC);
                    WrVector = CharacterChanger(Vector, VectorC);
                    WrBee = CharacterChanger(Bee, BeeC);

                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Sonic + 0x1), &WrSonic, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Knuckles + 0x1), &WrKnuckles, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Tails + 0x1), &WrTails, sizeof(int), 0);

                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Shadow + 0x1), &WrShadow, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Omega + 0x1), &WrOmega, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Rouge + 0x1), &WrRouge, sizeof(int), 0);

                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Amy + 0x1), &WrAmy, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Big + 0x1), &WrBig, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Cream + 0x1), &WrCream, sizeof(int), 0);

                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Espio + 0x1), &WrEspio, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Vector + 0x1), &WrVector, sizeof(int), 0);
                    WriteProcessMemory(HandleSonicHeroes, (PBYTE*)(Bee + 0x1), &WrBee, sizeof(int), 0);
                }
                else{
                }

                ImGui::End();

                if (CharacterChangerCodeMenu == 1){
                    ImGui::Begin("Character Codes Menu (Calls)");

                    ImGui::Text("Sonic 6074736\nKnuckles 5992368\nTails 6033184");
                    ImGui::Text("Shadow 6075664\nOmega 5992992\nRouge 6033952");
                    ImGui::Text("Amy 6076368\nBig 5993856\nCream 6034816");
                    ImGui::Text("Espio 6077248\nVector 5994816\nBee 6035600");

                    ImGui::End();
                }
                else
                {
                }
            }
            else{
            }

            if (StageCodesMenu == 1){
                ImGui::Begin("Stage Codes Menu");

                ImGui::Text("-1 = Dont Override");
                ImGui::Text("2 - Seaside Hill");
                ImGui::Text("3 - Ocean Place");
                ImGui::Text("4 - Grand Metropolis");
                ImGui::Text("5 - Power Plant");
                ImGui::Text("6 - Casino Park");
                ImGui::Text("7 - Bingo Highway");
                ImGui::Text("8 - Rail Canyon");
                ImGui::Text("9 - Bullet Station");
                ImGui::Text("10 - Frog Forest");
                ImGui::Text("11 - Lost Jungle");
                ImGui::Text("12 - Hang Castle");
                ImGui::Text("13 - Mystic Mansion");
                ImGui::Text("14 - Egg Fleet");
                ImGui::Text("15 - Final Fortress");
                ImGui::Text("16 - EGG HAWK");
                ImGui::Text("17 - TEAM ?? 1");
                ImGui::Text("18 - ROBOT CARNIVAL");
                ImGui::Text("19 - EGG ALBATROS");
                ImGui::Text("20 - TEAM ?? 2");
                ImGui::Text("21 - ROBOT STORM");
                ImGui::Text("22 - EGG EMPEROR");
                ImGui::Text("23 - METAL MADNESS");
                ImGui::Text("24 - METAL SONIC");
                ImGui::Text("29 - Bonus Stage 1");
                ImGui::Text("30 - Bonus Stage 2");
                ImGui::Text("31 - Bonus Stage 3");
                ImGui::Text("32 - Bonus Stage 4");
                ImGui::Text("33 - Bonus Stage 5");
                ImGui::Text("34 - Bonus Stage 6");
                ImGui::Text("35 - Bonus Stage 7");
                ImGui::Text("52 - Emerald Challange 1");
                ImGui::Text("53 - Emerald Challange 2");
                ImGui::Text("54 - Emerald Challange 3");
                ImGui::Text("55 - Emerald Challange 4");
                ImGui::Text("56 - Emerald Challange 5");
                ImGui::Text("57 - Emerald Challange 6");
                ImGui::Text("58 - Emerald Challange 7");

                ImGui::End();
            }
            else{
            }

            if (TeamCodesMenu == 1){
                ImGui::Begin("Team Codes Menu");

                ImGui::Text("-1 = Dont Override");
                ImGui::Text("0 - Team Sonic");
                ImGui::Text("1 - Team Dark");
                ImGui::Text("2 - Team Rose");
                ImGui::Text("3 - Team Chaotix");

                ImGui::End();
            }
            else{
            }

            if (OverrideStageCode != -1){
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x008D6720, &OverrideStageCode, sizeof(OverrideStageCode), 0);
            }
            else{
            }

            if (OverrideTeamCode != -1){
                WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x008D6920, &OverrideTeamCode, sizeof(OverrideTeamCode), 0);
            }
            else{
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