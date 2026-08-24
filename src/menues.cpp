#include "imgui.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "menues.h"
#include "memprocess.h"

constexpr int EmptyArea = 0x00900000;

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
            editor->write_force(0x0040116E, EmptyArea); // enter stage
            editor->write_force(0x004016D1, EmptyArea); // enter stage 2
            editor->write_force(0x004019B1, EmptyArea); // enter stage 3
            editor->write_force(0x004048A8, EmptyArea); // restart
            editor->write_force(0x00404A96, EmptyArea); // after death smt
            editor->write_force(0x00404B54, EmptyArea); // after death smt 2
            editor->write_force(0x00423A8D, EmptyArea); // super sonic ring lose
            editor->write_force(0x00423B29, EmptyArea); // ring claim - lose
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
            editor->write_force(0x00423F69,EmptyArea); // Split second
            editor->write_force(0x00423E91,EmptyArea); // Second
            editor->write_force(0x00423F0B,EmptyArea); // Minute
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

    float TeamBlastBar = 0;
    editor->read(0x009DD72C,TeamBlastBar);
    if (ImGui::SliderFloat("##TeamBlastBar", &TeamBlastBar, 0.0f, 91.0f)) {
        editor->write(0x009DD72C,TeamBlastBar);
    }

    uint8_t CurrentState = 0;
    editor->read(0x004019CC,CurrentState);
    bool TeamBlastFreeze = CurrentState == 0xFE;
    if (ImGui::Checkbox("Team Blast Always On", &TeamBlastFreeze)) {
        if (TeamBlastFreeze) {
            editor->write<unsigned char>(0x009DD73C,1); // instantly get TeamBlast

            short new_command = 0x05FE;
            editor->write_force(0x004019CC,new_command); // start stage: inc byte ptr [009DD73C]
            editor->write_force(0x004048C6,EmptyArea); // restart stage: mov [00900000],ebx
            editor->write_force(0x00404AB4,EmptyArea); // die: mov [00900000],ebx
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

    if (!TeamBlastFreeze) {
        uint8_t TeamBlastState = 0;
        editor->read<unsigned char>(0x009DD73C,TeamBlastState);
        const std::string TeamBlastButtonName = (TeamBlastState == 1) ? "Deactivate Team Blast" : "Activate Team Blast";
        if (ImGui::Button(TeamBlastButtonName.c_str())) {
            editor->write<unsigned char>(0x009DD73C,(TeamBlastState != 1));
            if (TeamBlastBar >= 92) editor->write<float>(0x009DD72C,0.0f);
        }
    }

    ImGui::End();
}
void shmemeditor::Menues::FlyBar()
{
    ImGui::Begin("FlyBar");

    ImGui::Text("How Many Unit Fly\n(Default 180)");
    float Unit = 0.0f;
    editor->read(0x00789FE4,Unit);
    if (ImGui::InputFloat("##HowManyUnitFly", &Unit)) {
        editor->write_force(0x00789FE4,Unit);
    }


    uint8_t CurrentState = 0;
    editor->read(0x005C56FA,CurrentState);
    bool Freeze = CurrentState == 0x28;
    if (ImGui::Checkbox("Infinite Fly", &Freeze)) {
        editor->write_force(0x005C56FA,Freeze ? 0x28 : 0x22);
    }

    ImGui::End();
}
void shmemeditor::Menues::ColorEditor()
{
    ImGui::Begin("Color Editor");

    struct CharacterData {
        const char* Name = nullptr;
        const char* EffectNames[6];
        std::vector<int> Address;
    };
    static const CharacterData AllCHData[12] {
        {"Sonic",
            {"Jump Balls", "Trails", "Tornado",     "Dash Effect", "Jump Dash Rings", "Sonic Overdrive Ball"},
            {0x8C729C,      0x8CF694, 0x8DE4F8,      0x8CF094,      0x8CEF40,          0x8C72AC}
        },
        {"Tails",
            {"Jump Balls", "Trails", "Fly Effect",  "Dash Effect"},
            {0x782584,      0x8CF69C, 0x8CEF48,      0x8CF09C}
        },
        {"Knuckles",
            {"Jump Balls", "Trails", "Combo Effect","Dash Effect"},
            {0x781F6C,      0x8CF698, 0x8CEF44,      0x8CF098}
        },
        {"Shadow",
            {"Jump Balls", "Trails", "Tornado",     "Dash Effect", "Jump Dash Rings"},
            {0x8C72A0,      0x8CF6A0, 0x8DE4FC,      0x8CF0A0,      0x8CEF4C}
        },
        {"Rouge",
            {"Jump Balls", "Trails", "Fly Effect",  "Dash Effect"},
            {0x782588,      0x8CF6A8, 0x8CEF54,      0x8CF0A8}
        },
        {"Omega",
            {"Jump Balls", "Trails", "Combo Effect","Dash Effect"},
            {0x781F70,      0x8CF6A4, 0x8CEF50,      0x8CF0A4}
        },
        {"Amy",
            {"Jump Balls", "Trails", "Tornado",     "Dash Effect", "Jump Dash Rings"},
            {0x8C72A4,      0x8CF6AC, 0x8DE504,      0x8CF0AC,      0x8CEF58}
        },
        {"Cream",
            {"Jump Balls", "Trails", "Fly Effect",  "Dash Effect"},
            {0x78258C,      0x8CF6B4, 0x8CEF60,      0x8CF0B4}
        },
        {"Big",
            {"Jump Balls", "Trails", "Combo Effect","Dash Effect"},
            {0x781F74,      0x8CF6B0, 0x8CEF5C,      0x8CF0B0}
        },
        {"Espio",
            {"Jump Balls", "Trails", "Tornado",     "Dash Effect", "Jump Dash Rings"},
            {0x8C72A8,      0x8CF6B8, 0x8DE500,      0x8CF0B8,      0x8CEF64}
        },
        {"Charmy",
            {"Jump Balls", "Trails", "Fly Effect",  "Dash Effect"},
            {0x782590,      0x8CF6C0, 0x8CEF6C,      0x8CF0C0}
        },
        {"Vector",
            {"Jump Balls", "Trails", "Combo Effect","Dash Effect"},
            {0x781F78,      0x8CF6BC, 0x8CEF68,      0x8CF0BC}
        }
    };
    static const char* CLCHItemLabel[12] = {
	"Sonic", "Tails", "Knuckles",
	"Shadow", "Rouge", "Omega",
	"Amy", "Cream", "Big",
	"Espio", "Charmy", "Vector"
	};
    static int selectedCH = 0;
    static int selectedEF = 0;

    if (ImGui::Combo("Character", &selectedCH,CLCHItemLabel,IM_ARRAYSIZE(CLCHItemLabel))) {
        selectedEF = 0;
    }
    int arrsize = (selectedCH % 3) ? 4 : 5;
    if (selectedCH == 0) arrsize = 6;
    ImGui::Combo("Effect", &selectedEF,AllCHData[selectedCH].EffectNames,arrsize);

    ImU32 U32Color = 0;
    editor->read(AllCHData[selectedCH].Address.at(selectedEF), U32Color);
    ImVec4 readColor = ImGui::ColorConvertU32ToFloat4(U32Color);
    float fColor[4] = {readColor.x,readColor.y,readColor.z,readColor.w};
    if (ImGui::ColorEdit4("Set Color", fColor)) {
        U32Color = ImGui::ColorConvertFloat4ToU32({fColor[0],fColor[1],fColor[2],fColor[3]});
        editor->write_force(AllCHData[selectedCH].Address.at(selectedEF), U32Color);
    }

    ImGui::End();
}

// on Menu
void shmemeditor::Menues::CharacterOverride()
{
    ImGui::Begin("Character Override");

    struct CharacterCodeArea {
        int Sonic = 0, Tails = 2, Knuckles = 1, Unknown1 = 3;
        int Shadow = 3, Rouge = 5, Omega = 4, Unknown2 = 3;
        int Amy = 6, Cream = 8, Big = 7, Unknown3 = 3;
        int Espio = 9, Charmy = 11, Vector = 10, Unknown4 = 3;
    };

    // 008BEB84 Sonic/Tails/Knuckles/03UNK-S/R/O/03UNK-A/C/B/03UNK-E/C/V/03UNK
    CharacterCodeArea Clear = {};
    if (ImGui::Button("Clear")) editor->write(0x008BEB84,Clear);
    
    static const char* CHComboBoxLabel[12] = {
	"Sonic", "Tails", "Knuckles",
	"Shadow", "Rouge", "Omega",
	"Amy", "Cream", "Big",
	"Espio", "Charmy", "Vector"
	};
    static const char* CHItemLabel[12] = {
	"Sonic", "Knuckles", "Tails",
	"Shadow", "Omega", "Rouge",
	"Amy", "Big", "Cream",
	"Espio", "Vector", "Charmy"
	};

    CharacterCodeArea CurrentState;;
    editor->read(0x008BEB84,CurrentState);
    int StateToInt[16] = {};
    memcpy(StateToInt, &CurrentState, sizeof(int) * 16);

    for (int i = 0, j= 0; i<16; ++i) {
        if ((i+1) % 4 == 0) continue;
        if (ImGui::Combo(CHComboBoxLabel[j],&StateToInt[i],CHItemLabel, IM_ARRAYSIZE(CHComboBoxLabel))) {
            editor->write(0x008BEB84 + 0x4 * i,StateToInt[i]);
        }
        ++j;
    }

    ImGui::Text("WARNING");
    ImGui::Text("If you place two or more characters of the same role\non the same team, the game will crash when exiting the stage.");


    ImGui::End();
}
void shmemeditor::Menues::TeamOverride()
{
    ImGui::Begin("Team Override");

    static const char* TMItemLabel[4] = {
        "Team Sonic",
        "Team Dark",
        "Team Rose",
        "Team Chaotix"
    };
    int EmptyAreaTMO = EmptyArea + 0xC;
    int Select = 0;
    editor->read(EmptyAreaTMO,Select);
    if (ImGui::Combo("Stage Override",&Select,TMItemLabel,IM_ARRAYSIZE(TMItemLabel))) {
        editor->write(EmptyAreaTMO,Select);
    }

    int ReadCode = 0x00022887;
    editor->read(0x0044B66E,ReadCode);
    bool Override = ReadCode == 0x90000C05;
    if (ImGui::Checkbox("Override Enable", &Override)) {
        editor->write_force(0x0044B66E, Override ? 0x90000C05 : 0x00022887);
    }

    ImGui::End();
}
void shmemeditor::Menues::StageOverride()
{
    ImGui::Begin("Stage Override");

    static const char* STGComboBoxLabel[37] = {
	"Seaside Hill" ,"Ocean Place" ,"Grand Metropolis",
    "Power Plant" ,"Casino Park" ,"Bingo Highway" ,
	"Rail Canyon" ,"Bullet Station" ,"Frog Forest",
	"Lost Jungle" ,"Hang Castle" ,"Mystic Mansion" ,
	"Egg Fleet" ,"Final Fortress" ,"EGG HAWK" ,
	"TEAM ?? 1" ,"ROBOT CARNIVAL" ,"EGG ALBATROS" ,
	"TEAM ?? 2" ,"ROBOT STORM" ,"EGG EMPEROR" ,
	"METAL MADNESS" ,"METAL SONIC" ,"Bonus Stage 1" ,
	"Bonus Stage 2" ,"Bonus Stage 3" ,"Bonus Stage 4" ,
	"Bonus Stage 5" ,"Bonus Stage 6" ,"Bonus Stage 7" ,
	"Emerald Challange 1" ,"Emerald Challange 2" ,"Emerald Challange 3" ,
	"Emerald Challange 4" ,"Emerald Challange 5" ,"Emerald Challange 6" ,
	"Emerald Challange 7" };
    
    // Welcome to code hell
    int EmptyAreaSTGO = EmptyArea + 0x8;
    int StageState = 0;
    editor->read(EmptyArea + 0x8,StageState);
    int StageCode = 0;
    if (StageState > 1 && StageState <= 24) StageCode = StageState - 2;
    else if (StageState > 28 && StageState <= 35) StageCode = StageState - 6;
    else if (StageState > 51 && StageState <= 58) StageCode = StageState - 22;
    if (ImGui::Combo("Stage Override", &StageCode, STGComboBoxLabel, IM_ARRAYSIZE(STGComboBoxLabel))) {
        int OverrideCode = 2;
        if (StageCode <= 22) OverrideCode = StageCode + 2;
        else if (StageCode > 22 && StageCode <= 29) OverrideCode = StageCode + 6;
        else if (StageCode > 29 && StageCode <= 36) OverrideCode = StageCode + 22;
        else OverrideCode = 2;

        editor->write(EmptyAreaSTGO,OverrideCode);
    }

    uint8_t CurrentState = 0;
    editor->read(0x0044B6A9,CurrentState);
    bool Override = CurrentState == 0x5;
    if (ImGui::Checkbox("Override Enable", &Override)) {
        // it reads new stage data from 0x900008
        unsigned char edited_code[6] = {0x05,0x08,0x00,0x90,0x00,0x90};
        unsigned char orginal_code[6] = {0x04,0x85,0x98,0x38,0x74,0x00};
        editor->write_force(0x0044B6A9,Override ? edited_code : orginal_code,6);

        if (Override) {
            int OverrideCode = 2;
            if (StageCode <= 22) OverrideCode = StageCode + 2;
            else if (StageCode > 22 && StageCode <= 29) OverrideCode = StageCode + 6;
            else if (StageCode > 29 && StageCode <= 36) OverrideCode = StageCode + 22;
            else OverrideCode = 2;
            editor->write(EmptyAreaSTGO,OverrideCode);
        }
    }

    ImGui::End();
    
}
