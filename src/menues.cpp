#include "imgui.h"
#include <algorithm>
#include <cstdint>

#include "menues.h"
#include "memprocess.h"

shmemeditor::Menues::Menues()
{
    init();
}
shmemeditor::Menues::~Menues()
{
    terminate();
}
void shmemeditor::Menues::init()
{
    terminate();
    editor = new yen::memprocess::Process(yen::memprocess::get_pid(L"Tsonic_win.exe"));
}

void shmemeditor::Menues::terminate()
{
    if (editor) delete editor;
    editor = nullptr;
}

void shmemeditor::Menues::Extra()
{

}
void shmemeditor::Menues::Ring()
{
    ImGui::Begin("Ring Edit");

    int Rings = 0;
    editor->read(0x009DD70C, Rings);

    uint8_t CurrentState = 0;
    editor->read(0x00423B2A, CurrentState);
    bool RingFreeze = CurrentState == 0x0;

    if (ImGui::InputInt("##Rings", &Rings)) {
        Rings = std::clamp(Rings,0,999);
        editor->write(0x009DD70C, Rings);

        if (RingFreeze) {
            editor->write_force(0x00584A15, Rings); // Quick update
            editor->write_force(0x0061835B, Rings); // Quick update metal sonic
        }
    }

    if (ImGui::Button("Add 50 Ring")) {
        Rings += 50;
        Rings = std::clamp(Rings,0,999);
        editor->write(0x009DD70C, Rings);

        if (RingFreeze) {
            editor->write_force(0x00584A15, Rings); // Quick update
            editor->write_force(0x0061835B, Rings); // Quick update metal sonic
        }
    }

    if (ImGui::Checkbox("Freeze", &RingFreeze)) {
        if (RingFreeze){ // freeze
            editor->write_force(0x0040116E, 0x00900000); // enter stage
            editor->write_force(0x004016D1, 0x00900000); // enter stage 2
            editor->write_force(0x004019B1, 0x00900000); // enter stage 3
            editor->write_force(0x004048A8, 0x00900000); // restart
            editor->write_force(0x00404A96, 0x00900000); // after death smt
            editor->write_force(0x00404B54, 0x00900000); // after death smt 2
            editor->write_force(0x00423A8D, 0x00900000); // super sonic ring lose
            editor->write_force(0x00423B29, 0x00900000); // ring claim - lose
            editor->write_force(0x00584A15, Rings); // before death
            editor->write_force(0x0061835B, Rings); // metal sonic 50 ring
        }
        else { // restore
            editor->write_force(0x0040116E, 0x009DD70C); // enter stage
            editor->write_force(0x004016D1, 0x009DD70C); // enter stage 2
            editor->write_force(0x004019B1, 0x009DD70C); // enter stage 3
            editor->write_force(0x004048A8, 0x009DD70C); // restart
            editor->write_force(0x00404A96, 0x009DD70C); // after death smt
            editor->write_force(0x00404B54, 0x009DD70C); // after death smt 2
            editor->write_force(0x00423A8D, 0x009DD70C); // super sonic ring lose
            editor->write_force(0x00423B29, 0x009DD70C); // ring claim - lose
            editor->write_force(0x00584A15, 0x0); // before death
            editor->write_force(0x0061835B, 0x32); // metal sonic 50 ring
        }
    }

    ImGui::End();

}
void shmemeditor::Menues::Power()
{
    ImGui::Begin("Character Power");

    uintptr_t Address = editor->get_pointer_address(0x00A4C268, {0x208});
    if (!Address) {
        ImGui::Text("Couldnt find address");
        ImGui::End();
        return;
    }

    uint8_t Powers[3] = {};
    editor->read(Address,Powers);
    int IntPowers[3] = {Powers[0],Powers[1],Powers[2]};

    if (ImGui::SliderInt3("S/F/P ##Powers", IntPowers, 0, 3)) {
        Powers[0] = IntPowers[0];
        Powers[1] = IntPowers[1];
        Powers[2] = IntPowers[2];
        editor->write(Address,Powers,3);
    }

    uint8_t CurrentState = 0;
    editor->read(0x005B4C38, &CurrentState, 1);
    bool PowerFreeze = CurrentState;

    if (ImGui::Checkbox("Freeze", &PowerFreeze)) {
        if (PowerFreeze) { // freeze
            uint8_t freeze_uppwr[3] = {0xC2,0x0C,0x00}; 
            uint8_t freeze_rstlvl[3] = {0x90,0x90,0x90};
            uint8_t freeze_entlvl[6] = {0x1D, 0x00, 0x00, 0x90, 0x00, 0x90};
            editor->write_force(0x005B4C50,freeze_uppwr,3); // Tsonic_win.exe+1B4C50 - 64 A1 00000000 - mov eax,fs:[00000000] -> write C2 0C 00 (ret 000C)
            editor->write_force(0x005B4C37,freeze_rstlvl,3); // Tsonic_win.exe+1B4C37 - C6 00 00 - mov byte ptr [eax],00 Restart Level -> write 0x909090 (3x NOP)
            editor->write_force(0x005AA951,freeze_entlvl,6); // Tsonic_win.exe+1AA950 - 88 9C 30 08020000 - mov [eax+esi+00000208],bl -> write mov [Tsonic_win.exe+500004],bl
        }
        else { // restore
            uint8_t restore_uppwr[3] = {0x64,0xA1,0x00};
            uint8_t restore_rstlvl[3] = {0xC6,0x00,0x00};
            uint8_t restore_entlvl[6] = {0x9C, 0x30, 0x08, 0x02, 0x00, 0x00};
            editor->write_force(0x005B4C50,restore_uppwr,3);
            editor->write_force(0x005B4C37,restore_rstlvl,3);
            editor->write_force(0x005AA951,restore_entlvl,6);
        }

    }

    ImGui::End();
}
void shmemeditor::Menues::Time()
{
    ImGui::Begin("Time");

    uint8_t Time[3] = {};
    editor->read(0x009DD708,Time,3);
    int TimeInt[3] = {Time[2],Time[1],Time[0]};
    if (ImGui::InputInt3("##Time", TimeInt)) {
        TimeInt[0] = std::clamp(TimeInt[0],0,99);
        TimeInt[1] = std::clamp(TimeInt[1],0,59);
        TimeInt[2] = std::clamp(TimeInt[2],0,59);
        Time[0] = TimeInt[2];   Time[1] = TimeInt[1];   Time[2] = TimeInt[0];
        editor->write(0x009DD708,Time,3);
    }

    if (ImGui::Button("Clear Time")) {
        uint8_t ClearTime[3] = {0,0,0};
        editor->write(0x009DD708,ClearTime,3);
    }

    uint8_t CurrentState = 0;
    editor->read(0x00423F6A,CurrentState);
    bool TimeFreeze = !CurrentState;
    if (ImGui::Checkbox("Freeze", &TimeFreeze)) {
        if (TimeFreeze) {
            editor->write_force(0x00423F69,0x00900000); // Split second
            editor->write_force(0x00423E91,0x00900000); // Second
            editor->write_force(0x00423F0B,0x00900000); // Minute
            editor->write_force<unsigned char>(0x004270D7,0xC3); // Enter Stage
            editor->write_force<unsigned char>(0x00423A0D,0xEB); // Enter Stage 2
        }
        else {
            editor->write_force(0x00423F69,0x009DD708);
            editor->write_force(0x00423E91,0x009DD709);
            editor->write_force(0x00423F0B,0x009DD70A);
            editor->write_force<unsigned char>(0x004270D7,0xC6);
            editor->write_force<unsigned char>(0x00423A0D,0x75);
        }
    }

    ImGui::End();
}
void shmemeditor::Menues::Point()
{
    ImGui::Begin("Point");

    int Points[3] = {};
    editor->read(0x009DD6C0,Points,12);
    int TotalPoint = Points[0] + Points[1] + Points[2];
    ImGui::Text("Total Point: %d",TotalPoint);
    if (ImGui::InputInt3("S/F/P", Points)) {
        editor->write(0x009DD6C0,Points,12);
    }

    ImGui::End();
}
void shmemeditor::Menues::TeamBlast()
{
    ImGui::Begin("Team Blast");

    uint8_t CurrentState = 0;
    editor->read(0x004019CC,CurrentState);
    bool TeamBlastFreeze = CurrentState == 0xFE;
    if (ImGui::Checkbox("Team Blast Always On", &TeamBlastFreeze)) {
        if (TeamBlastFreeze) {
            editor->write<unsigned char>(0x009DD73C,1);

            short new_command = 0x05FE;
            editor->write_force(0x004019CC,new_command); // start stage: inc byte ptr [009DD73C]
            editor->write_force(0x004048C6,0x00900000); // restart stage: mov [00900000],ebx
            editor->write_force(0x00404AB4,0x00900000); // die: mov [00900000],ebx
            editor->write_force<unsigned char>(0x00420378,1); // rest 2: mov [ecx+009DD73C],00000001
            editor->write_force<unsigned char>(0x0060A475,1); // Metal Sonic Team Change: mov [009DD73C],00000001
        }
        else {
            short orginal = 0x1D89;
            editor->write_force(0x004019CC,orginal); // start stage: mov [009DD73C],ebx
            editor->write_force(0x004048C6,0x009DD73C); // restart stage: mov [009DD73C],ebx
            editor->write_force(0x00404AB4,0x009DD73C); // die: mov [009DD73C],ebx
            editor->write_force<unsigned char>(0x00420378,0); // rest 2: mov [ecx+009DD73C],00000000
            editor->write_force<unsigned char>(0x0060A475,0); // Metal Sonic Team Change: mov [009DD73C],00000000
        
        }
    }

    ImGui::End();
}
