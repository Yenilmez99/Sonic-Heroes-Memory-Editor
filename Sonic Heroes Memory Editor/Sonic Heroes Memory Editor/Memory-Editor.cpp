#define SFML_STATIC
#define _USE_MATH_DEFINES
#define SONIC_HEROES_BASE_ADDRESS 0x400000
#define myImGuiFlagCombo1 (ImGuiWindowFlags_)(0x2 | 0x4 | 0x20) // No Move, No Resiazble, No Collapse
#define myImGuiFlagCombo2 (ImGuiWindowFlags_)(0x1 | 0x2 | 0x4 | 0x20) // No TitleBar No Move, No Resiazble, No Collapse

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>

#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <Windows.h>
#include "Memory-Editor.h"

HWND GetHWNDByWindowName(std::wstring WindowName) {
	// find hwnd by window name
	LPCWSTR LPCWindowName = WindowName.c_str();
	HWND WindowNameToFindWindow = FindWindowW(NULL, LPCWindowName);
	if (WindowNameToFindWindow == NULL) {
		std::cerr << "Can't found window" << std::endl;
		return NULL;
	}
	return WindowNameToFindWindow;
}
HANDLE GetHandleByHWND(HWND WindowNameToFindWindow) {
	// find handle by hwnd

	DWORD WindowProcessID;
	GetWindowThreadProcessId(WindowNameToFindWindow, &WindowProcessID);

	HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, false, WindowProcessID);
	if (ProcessHandle == NULL) {
		std::cerr << "Cant Find Process Handle" << std::endl;
		return NULL;
	}
	return ProcessHandle;
}
DWORD GetPointerAddress(const HANDLE ProcessHandle,
	DWORD GameBaseAddress,
	DWORD PointerMainAddress,
	const std::vector<int> Offsets) {
	int Address = NULL;
	ReadProcessMemory(ProcessHandle, (PBYTE*)(GameBaseAddress + PointerMainAddress), &Address, sizeof(int), 0);
	for (int i = 0; i < Offsets.size(); i++) {
		PointerMainAddress = Address + Offsets.at(i);
		if (i + 1 < Offsets.size())
			ReadProcessMemory(ProcessHandle, (PBYTE*)PointerMainAddress, &Address, sizeof(int), 0);
	}
	return PointerMainAddress;
}
void CharacterChanger(HANDLE SonicHeroesHandle,const short Character, const short toCharacter) {
	static const int CharacterAddress[12] = {
		0x005AAC87,0x005AAC9C,0x005AACB1,
		0x005AACC6,0x005AACD8,0x005AACED,
		0x005AAD02,0x005AAD14,0x005AAD26,
		0x005AAD38,0x005AAD60,0x005AAD88 };
	static const int CharacterCalls[12] = { 
		0x005CB170,0x005B6FB0,0x005C0F20,
		0x005CB510,0x005B7220,0x005C1220,
		0x005CB7D0,0x005B7580,0x005C1580,
		0x005CBB40,0x005B7940,0x005C1890 };

	int WriteCode = CharacterCalls[toCharacter] - (CharacterAddress[Character] + 0x5);
	WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(CharacterAddress[Character] + 0x1), &WriteCode, sizeof(int), 0);
}

MemoryEditor::MemoryEditor(sf::RenderWindow* WindowAddr,sf::Time* DeltaTimeAddr) {
	this->WindowPtr = WindowAddr;
	this->myDeltaTime = DeltaTimeAddr;
}
MemoryEditor::~MemoryEditor() {
}
bool MemoryEditor::Sonic_Heroes_Is_Open() {
	this->SonicHeroesHWND = GetHWNDByWindowName(L"SONIC HEROES(TM)");

	if (this->SonicHeroesHWND == NULL) return false;
	else;

	this->SonicHeroesHandle = GetHandleByHWND(SonicHeroesHWND);

	if (this->SonicHeroesHandle == NULL) return false;
	else return true;

}
void MemoryEditor::LoadCharacterAddress() { // if have a problem 

	CompareCharacterAddress[0] = GetPointerAddress(SonicHeroesHandle, SONIC_HEROES_BASE_ADDRESS, (DWORD)(0x005CE820), { 0x00 });
	CompareCharacterAddress[1] = GetPointerAddress(SonicHeroesHandle, SONIC_HEROES_BASE_ADDRESS, (DWORD)(0x005CE824), { 0x00 });
	CompareCharacterAddress[2] = GetPointerAddress(SonicHeroesHandle, SONIC_HEROES_BASE_ADDRESS, (DWORD)(0x005CE828), { 0x00 });

	CharactersMainAddresses[0] = *std::min_element(CompareCharacterAddress, CompareCharacterAddress + 3); // Speed
	CharactersMainAddresses[2] = *std::max_element(CompareCharacterAddress, CompareCharacterAddress + 3); // Power

	CharactersMainAddresses[1] = 0;
	for (int i = 0; i < 3; i++)
		CharactersMainAddresses[1] += CompareCharacterAddress[i];
	CharactersMainAddresses[1] -= CharactersMainAddresses[0] + CharactersMainAddresses[2]; // Fly

}
void MemoryEditor::LoadPointers() {

	ActiveRoleAddress = GetPointerAddress(SonicHeroesHandle, SONIC_HEROES_BASE_ADDRESS, (DWORD)0x006778AC, { 0x18, 0x148 });
	CharacterLevelAddress = GetPointerAddress(SonicHeroesHandle, SONIC_HEROES_BASE_ADDRESS, (DWORD)0x006778B0, { 0x818 });
	FlyBarAddress = GetPointerAddress(SonicHeroesHandle, SONIC_HEROES_BASE_ADDRESS, (DWORD)0x00675390, { 0x6C });
	LoadCharacterAddress();

}
unsigned char MemoryEditor::GetGameState() {
	static unsigned char Game_State = NULL;
	ReadProcessMemory(SonicHeroesHandle,(PBYTE*)0x008D66F0, &Game_State,(SIZE_T)1, 0);
	return Game_State;
}
void MemoryEditor::SetGameState(unsigned char Game_State) {
	WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x008D66F0, &Game_State, (SIZE_T)1, 0);
}
void MemoryEditor::FreeCameraControl() {

	float Yaw = CameraRotation[1] * (2.0f * M_PI / 65536.0f);;

	float forwardX = sinf(Yaw);
	float forwardZ = cosf(Yaw);

	float rightX = cosf(Yaw);
	float rightZ = -sinf(Yaw);

	// Forward
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
		CameraPosition[0] -= forwardX * CameraMovSpeed;
		CameraPosition[2] -= forwardZ * CameraMovSpeed;
	}

	// Backward
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
		CameraPosition[0] += forwardX * CameraMovSpeed;
		CameraPosition[2] += forwardZ * CameraMovSpeed;
	}

	// Left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
		CameraPosition[0] -= rightX * CameraMovSpeed;
		CameraPosition[2] -= rightZ * CameraMovSpeed;
	}

	// Right
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
		CameraPosition[0] += rightX * CameraMovSpeed;
		CameraPosition[2] += rightZ * CameraMovSpeed;
	}

	// Up
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
		CameraPosition[1] += CameraMovSpeed;
	}

	// Down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V)) {
		CameraPosition[1] -= CameraMovSpeed;
	}

	// AxisX
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M))
		CameraRotation[0] += 50.0f * CameraMovSpeed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N))
		CameraRotation[0] -= 50.0f * CameraMovSpeed;

	// AxisY
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift))
		CameraRotation[1] += 50.0f * CameraMovSpeed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl))
		CameraRotation[1] -= 50.0f * CameraMovSpeed;

	// AxisZ
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift))
		CameraRotation[2] += 50.0f * CameraMovSpeed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
		CameraRotation[2] -= 50.0f * CameraMovSpeed;

	for (unsigned char i = 0; i < 3; i++) {
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x00A60C30 + 0x4 * i), &CameraPosition[i], sizeof(float), 0);
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x00A60C3C + 0x4 * i), &CameraRotation[i], sizeof(short), 0);
	}

}

void MemoryEditor::Draw() {
	if (Sonic_Heroes_Is_Open()) {

		Game_State = GetGameState();

		if (Game_State > GameState::EndLevelLoad && Game_State < GameState::InGameExitWithSave) {

			if (!FirstTimeOpenedStage) {
				LoadPointers();
				FirstTimeOpenedStage = true;
			}

			CharacterLevelEdit();
			TeamBlastEdit();
			RingAmmoEdit();
			TimerEdit();
			FlyBarEdit();
			ExternalForceEdit();
			PointEdit();
			ExtraEdits();
			SizeEdit();
			PositionEdit();
			CameraEdit();
			ColorEdit();

		}

		else if (Game_State == GameState::InGameExitWithSave || Game_State == GameState::InGameExit2 || Game_State == GameState::Unknown13Exit || Game_State == GameState::Unknown17Exit) {
			ImGui::Begin("Quiting Stage", nullptr);
			ImGui::SetWindowPos(ImVec2(WindowPtr->getSize().x / 4, 0.0f));
			ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 2, WindowPtr->getSize().y));
			ImGui::End();

			static DWORD oldProtect;
			if (VirtualProtectEx(SonicHeroesHandle, (LPVOID)0x006AA46F, sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect)) {

				static unsigned const char OrginalCode[4] = { 0xD9, 0x44, 0x24, 0x28 };
				WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x006AA46F, OrginalCode, (SIZE_T)4, 0);

				VirtualProtectEx(SonicHeroesHandle, (LPVOID)0x006AA46F, sizeof(float), oldProtect, &oldProtect);
			}

		}

		else if (Game_State == GameState::Null || Game_State == GameState::Menu) {
			StageFreezeCheckbox = 0;
			CharacterOverride();
			StageTeamOverride();
		}

		else {
		}

	}

	else {
		ImGui::Begin("Sonic Heroes Not Running", nullptr);
		ImGui::SetWindowPos(ImVec2(WindowPtr->getSize().x / 4, 0.0f));
		ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 2, WindowPtr->getSize().y));
		ImGui::End();

		FirstTimeOpenedStage = false;

	}

}

// In Stage Functions
void MemoryEditor::CharacterLevelEdit() {

	for (short i = 0; i < 3; i++) {
		ReadProcessMemory(SonicHeroesHandle, (PBYTE*)(CharacterLevelAddress + 0x1 * i), &CharacterLevels[i].VarIntager, sizeof(char), 0);
	}
	ImGui::Begin("Characters Level Edit", nullptr, myImGuiFlagCombo1);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 4, 2 * WindowPtr->getSize().y / 10));
	// ImGui::PushItemWidth(Window->getSize().x / 4 - 15);

	ImGui::Text("S/F/P Level");

	static int CharacterLeveltoInt[3] = { NULL,NULL,NULL };
	for (short i = 0; i < 3; i++) {
		CharacterLeveltoInt[i] = CharacterLevels[i].VarIntager;
	}

	if (ImGui::SliderInt3("##CharacterLevel", CharacterLeveltoInt, 0, 3)) {
		for (short i = 0; i < 3; i++)
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(CharacterLevelAddress + 0x1 * i), &CharacterLeveltoInt[i], sizeof(char), 0);
	}

	if (ImGui::Checkbox("Freeze Level", &CharacterLevels[0].CheckBoxControl)) {
		static const unsigned char OrginalCodeGiveLVL[7] = { 0x88, 0x84, 0x29, 0x08, 0x02, 0x00, 0x00 };
		static const unsigned char OrginalCodeLoseLVL[3] = { 0xC6, 0x00, 0x00 };
		if (CharacterLevels[0].CheckBoxControl) {
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x005B4C81), UsefulNops, (SIZE_T)7, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x005B4C37), UsefulNops, (SIZE_T)3, 0);
		}
		else {
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x005B4C81), OrginalCodeGiveLVL, (SIZE_T)7, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x005B4C37), OrginalCodeLoseLVL, (SIZE_T)3, 0);

		}
	}

	ImGui::End();
}
void MemoryEditor::TeamBlastEdit() {
	ReadProcessMemory(SonicHeroesHandle, (PBYTE*)0x009DD72C, &TeamBlast.VarFloat, sizeof(float), 0);

	ImGui::Begin("Team Blast Edit", nullptr, myImGuiFlagCombo1);
	ImGui::SetWindowPos(ImVec2(WindowPtr->getSize().x / 4, 0.0f));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 4, 2 * WindowPtr->getSize().y / 10));

	if (ImGui::SliderFloat("##TeamBlastBar", &TeamBlast.VarFloat, 0.0f, 91.0f, "%.0f")) {
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x009DD72C, &TeamBlast.VarFloat, sizeof(float), 0);
	}

	if (ImGui::Checkbox("Team Blast Always On", &TeamBlast.CheckBoxControl)) {
		if (TeamBlast.CheckBoxControl)
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x009DD73C, &TeamBlast.CheckBoxControl, sizeof(bool), 0);
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00420378, &TeamBlast.CheckBoxControl, sizeof(bool), 0);
	}

	ImGui::End();
}
void MemoryEditor::RingAmmoEdit() {
	ReadProcessMemory(SonicHeroesHandle, (PBYTE*)0x009DD70C, &Ring.VarIntager, sizeof(int), 0);

	ImGui::Begin("Ring Ammount", nullptr, myImGuiFlagCombo1);
	ImGui::SetWindowPos(ImVec2(WindowPtr->getSize().x / 2, 0.0f));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 4, 2.3f * WindowPtr->getSize().y / 10));

	if (ImGui::InputInt("##Ring", &Ring.VarIntager, 1)) {

		Ring.VarIntager = std::clamp(Ring.VarIntager, 0, 999);

		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x009DD70C, &Ring.VarIntager, sizeof(int), 0);

		if (Ring.CheckBoxControl)
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00423B2D, &Ring.VarIntager, sizeof(int), 0);
	}

	static int HowManyAddRing = 10;
	static std::string Label = "Add Ring";
	Label = std::format("Add {} Ring", HowManyAddRing);

	ImGui::Text("How Many Add Ring");
	ImGui::InputInt("##HowManyRing", &HowManyAddRing);
	if (ImGui::Button(Label.c_str())) {
		Ring.VarIntager += HowManyAddRing;
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x009DD70C, &Ring.VarIntager, sizeof(int), 0);
		Ring.VarIntager -= HowManyAddRing;
	}

	if (ImGui::Checkbox("Ring Freeze", &Ring.CheckBoxControl)) {
		static const unsigned char OrginalCodeClaimAndHit[7] = { 0x89, 0x04, 0xAD, 0x0C, 0xD7, 0x9D, 0x00 };  // Orginal Value for Claim and hit change
		static const unsigned char OrginalCodeDeath[17] = {
		0xC7, 0x04, 0x85, 0x0C, 0xD7, 0x9D, 0x00, 0x00, 0x00, 0x00, 0x00,  // Should nop address 1
		0x89, 0x1D, 0x0C, 0xD7, 0x9D, 0x00 }; // Should nop address 2
		static const unsigned char MetalSonicSubRing[7] = { 0x89, 0x04, 0x8D, 0x0C, 0xD7, 0x9D, 0x00 };

		if (Ring.CheckBoxControl) {
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00423B26, UsefulNops, (SIZE_T)7, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00584A0E, UsefulNops, (SIZE_T)11, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00404A94, UsefulNops, (SIZE_T)6, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00404B52, UsefulNops, (SIZE_T)6, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00423A8A, UsefulNops, (SIZE_T)7, 0);
		}
		else {
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00423B26, OrginalCodeClaimAndHit, (SIZE_T)7, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00584A0E, OrginalCodeDeath, (SIZE_T)11, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00404A94, &OrginalCodeDeath[12], (SIZE_T)6, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00404B52, &OrginalCodeDeath[12], (SIZE_T)6, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00423A8A, MetalSonicSubRing, (SIZE_T)7, 0);
		}
	}

	ImGui::End();
}
void MemoryEditor::TimerEdit() {
	for (short i = 0; i < 3; i++)
		ReadProcessMemory(SonicHeroesHandle, (PBYTE*)(0x009DD708 + i), &Timer[2 - i], sizeof(bool), 0);

	ImGui::Begin("Timer Edit", nullptr, myImGuiFlagCombo2);
	ImGui::SetWindowPos(ImVec2(3 * WindowPtr->getSize().x / 4, 0.0f));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 4, WindowPtr->getSize().y / 10));

	if (ImGui::InputInt3("##Timer", Timer)) {
		Timer[0] = std::clamp(Timer[0], 0, 99);
		Timer[1] = std::clamp(Timer[1], 0, 59);
		Timer[2] = std::clamp(Timer[2], 0, 99);
		for (short i = 0; i < 3; i++)
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x009DD708 + i), &Timer[2 - i], sizeof(bool), 0);

	}
	static bool TimeFreezeCheckBox = 0;
	if (ImGui::Checkbox("Time Freeze", &TimeFreezeCheckBox)) {

		static const unsigned char SplitSecondOrginalCode[6] = { 0x88,0x1d,0x08,0xd7,0x9d,0x00 };
		static const unsigned char SecondOrginalCode[6] = { 0x88,0x15,0x09,0xd7,0x9d,0x00 };
		static const unsigned char MinuteOrginalCode[5] = { 0xa2,0x0a,0xd7,0x9d,0x00 };

		if (TimeFreezeCheckBox) {

			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00423F67, UsefulNops, (SIZE_T)6, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00423E8F, UsefulNops, (SIZE_T)6, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00423F0A, UsefulNops, (SIZE_T)5, 0);

		}

		else {
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00423F67, SplitSecondOrginalCode, (SIZE_T)6, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00423E8F, SecondOrginalCode, (SIZE_T)6, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00423F0A, MinuteOrginalCode, (SIZE_T)5, 0);
		}
	}

	ImGui::End();
}
void MemoryEditor::FlyBarEdit() {

	ImGui::Begin("Fly Bar Edit", nullptr, myImGuiFlagCombo2);
	ImGui::SetWindowPos(ImVec2(0.0f, 2 * WindowPtr->getSize().y / 10));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 4, 2 * WindowPtr->getSize().y / 10));

	static float MaxFlyUnit = NULL;
	ReadProcessMemory(SonicHeroesHandle, (PBYTE*)0x00789FE4, &MaxFlyUnit, sizeof(float), 0);

	ReadProcessMemory(SonicHeroesHandle, (PBYTE*)FlyBarAddress, &FlyBar.VarFloat, sizeof(float), 0);

	if (ImGui::SliderFloat("##FlyBar", &FlyBar.VarFloat, 0, MaxFlyUnit, "%.1f"))
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)FlyBarAddress, &FlyBar.VarFloat, sizeof(float), 0);

	if (ImGui::Checkbox("Infinite Fly", &FlyBar.CheckBoxControl)) {
		static const unsigned char OrginalCode[6] = { 0xD9, 0x9D, 0x94, 0x09, 0x00, 0x00 };
		if (FlyBar.CheckBoxControl)
			WriteProcessMemory(SonicHeroesHandle, (LPVOID)0x005C571D, UsefulNops, (SIZE_T)6, nullptr);

		else
			WriteProcessMemory(SonicHeroesHandle, (LPVOID)0x005C571D, OrginalCode, (SIZE_T)6, nullptr);
	}

	ImGui::Text("How Many Unit Fly\n(Default 180)");

	if (ImGui::InputFloat("##HowManyFlyUnit", &MaxFlyUnit, 0.0f, 0.0f, "%.2f")) {
		static DWORD oldProtect;
		if (VirtualProtectEx(SonicHeroesHandle, (LPVOID)0x00789FE4, sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect)) {
			WriteProcessMemory(SonicHeroesHandle, (LPVOID)0x00789FE4, &MaxFlyUnit, sizeof(float), nullptr);
			VirtualProtectEx(SonicHeroesHandle, (LPVOID)0x00789FE4, sizeof(float), oldProtect, &oldProtect);
		}
	}

	ImGui::End();
}
void MemoryEditor::ExternalForceEdit() {

	ExternalForceAddress = GetPointerAddress(SonicHeroesHandle, SONIC_HEROES_BASE_ADDRESS, (DWORD)(0x005CE820), { 0xDC });;

	ImGui::Begin("Super Speed - Moon Jump", nullptr, myImGuiFlagCombo2);
	ImGui::SetWindowPos(ImVec2(WindowPtr->getSize().x / 4, 2 * WindowPtr->getSize().y / 10));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 4, 2 * WindowPtr->getSize().y / 10));

	ImGui::Text("Super Speed Force\nMoon Jump Force");
	ImGui::InputFloat2("##ExternalForce", ExternalForce, "%.2f");
	ImGui::InputText("SS Key", ForceKeys, sizeof(short), ImGuiInputTextFlags_CharsUppercase);
	ImGui::InputText("MJ Key", &ForceKeys[2], sizeof(short), ImGuiInputTextFlags_CharsUppercase);

	if (SonicHeroesHWND == GetForegroundWindow()) {
		for (short i = 0; i < 2; i++) {
			if (GetAsyncKeyState(ForceKeys[i*2]) & 0x8000)
				WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(ExternalForceAddress + i * 4), &ExternalForce[i], sizeof(float), 0);
		}
	}

	ForceKeys[0] = std::clamp(ForceKeys[0], 'A', 'Z');
	ForceKeys[2] = std::clamp(ForceKeys[2], 'A', 'Z');

	ImGui::End();
}
void MemoryEditor::PointEdit() {
	for (short i = 0; i < 3; i++)
		ReadProcessMemory(SonicHeroesHandle, (PBYTE*)(0x009DD6C0 + i * 4), &CharacterPoint[i], sizeof(int), 0);

	CharacterPoint[3] = CharacterPoint[0] + CharacterPoint[1] + CharacterPoint[2]; // Total Point

	ImGui::Begin("Point Edit", nullptr, myImGuiFlagCombo2);
	ImGui::SetWindowPos(ImVec2(WindowPtr->getSize().x / 2, 2.3f * WindowPtr->getSize().y / 10));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 4, 1.7 * WindowPtr->getSize().y / 10));

	ImGui::Text("Total Point: %d", CharacterPoint[3]);
	if (ImGui::InputInt("Speed", &CharacterPoint[0]))WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x009DD6C0, &CharacterPoint[0], sizeof(int), 0);
	if (ImGui::InputInt("Fly", &CharacterPoint[1]))WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x009DD6C4, &CharacterPoint[1], sizeof(int), 0);
	if (ImGui::InputInt("Power", &CharacterPoint[2]))WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x009DD6C8, &CharacterPoint[2], sizeof(int), 0);

	ImGui::End();
}
void MemoryEditor::ExtraEdits() {
	ReadProcessMemory(SonicHeroesHandle, (PBYTE*)(ActiveRoleAddress), &ActiveRole, sizeof(char), 0);
	ReadProcessMemory(SonicHeroesHandle, (PBYTE*)0x009DD74C, &Live.VarIntager, sizeof(int), 0);

	ImGui::Begin("Extras", nullptr, myImGuiFlagCombo2);
	ImGui::SetWindowPos(ImVec2(3 * WindowPtr->getSize().x / 4, WindowPtr->getSize().y / 10));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 4, 3 * WindowPtr->getSize().y / 10));

	ImGui::Text("Active Role S/P/F");
	if (ImGui::SliderInt("##ActiveRole", (int*)(&ActiveRole), 0, 2)) {
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(ActiveRoleAddress), &ActiveRole, sizeof(char), 0);
	}

	if (ImGui::InputInt("Live", &Live.VarIntager))
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x009DD74C, &Live, sizeof(int), 0);
	if (ImGui::Checkbox("Freeze Live", &Live.CheckBoxControl)) {

		static unsigned const char OrginalCode[7] = { 0x89, 0x04 ,0x8D ,0x4C,0xD7 ,0x9D ,0x00 };

		if (Live.CheckBoxControl) {
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x0040462E, UsefulNops, (SIZE_T)7, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00423B97, UsefulNops, (SIZE_T)7, 0);
		}

		else {
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x0040462E, OrginalCode, (SIZE_T)7, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00423B97, OrginalCode, (SIZE_T)7, 0);
		}
	}

	if (ImGui::Button("Quick Exit")) SetGameState(GameState::InGameExitWithSave);

	static bool DisableGUICheckBox = 0;
	static unsigned const char OrginalCode[4] = { 0xD9, 0x44, 0x24, 0x28 };
	if (ImGui::Checkbox("Disable GUI", &DisableGUICheckBox)) {

		static DWORD oldProtect;
		if (VirtualProtectEx(SonicHeroesHandle, (LPVOID)0x006AA46F, sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect)) {

			if (DisableGUICheckBox &&
				(Game_State == GameState::Unknown4 ||
					Game_State == GameState::InGame ||
					Game_State == GameState::InGameSceneFrozen)) {
				WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x006AA46F, UsefulNops, (SIZE_T)4, 0);
			}

			else {
				WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x006AA46F, OrginalCode, (SIZE_T)4, 0);
			}

			VirtualProtectEx(SonicHeroesHandle, (LPVOID)0x006AA46F, sizeof(float), oldProtect, &oldProtect);
		}


	}
	if (DisableGUICheckBox) {

		static DWORD oldProtect;
		if (VirtualProtectEx(SonicHeroesHandle, (LPVOID)0x006AA46F, sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect)) {

			if (Game_State != GameState::Unknown4 && Game_State != GameState::InGame && Game_State != GameState::InGameSceneFrozen) {
				WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x006AA46F, OrginalCode, (SIZE_T)4, 0);
			}

			else {
				WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x006AA46F, UsefulNops, (SIZE_T)4, 0);
			}

			VirtualProtectEx(SonicHeroesHandle, (LPVOID)0x006AA46F, sizeof(float), oldProtect, &oldProtect);
		}

	}

	if ((GetAsyncKeyState('Q') & 0x1) && SonicHeroesHWND == GetForegroundWindow()) {
		StageFreezeCheckbox = !StageFreezeCheckbox;

		if (!StageFreezeCheckbox && Game_State == GameState::InGameSceneFrozen) {
			SetGameState(GameState::InGame);
		}
	}

	if (ImGui::Checkbox("Freeze Stage", &StageFreezeCheckbox)) {

		if (StageFreezeCheckbox && (Game_State == GameState::InGame || Game_State == GameState::Unknown4)) {
			SetGameState(GameState::InGameSceneFrozen);
		}

		else if (!StageFreezeCheckbox && Game_State == GameState::InGameSceneFrozen) {
			SetGameState(GameState::InGame);
		}

	}

	ImGui::Text("Shortcut Key: Q");

	if (StageFreezeCheckbox && (Game_State == GameState::InGame || Game_State == GameState::Unknown4)) {

		SetGameState(GameState::InGameSceneFrozen);
	}

	ImGui::End();

}
void MemoryEditor::SizeEdit() {

	LoadCharacterAddress();

	int SizeAddress[3] = { 
		CharactersMainAddresses[0] + 0x100,
		CharactersMainAddresses[1] + 0x100,
		CharactersMainAddresses[2] + 0x100 };

	static float Sizes[3][3] = {
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	for (short i = 0; i < 3; i++) for (short j = 0; j < 3; j++)
		ReadProcessMemory(SonicHeroesHandle, (PBYTE*)(SizeAddress[i] + j * 4), &Sizes[i][j], sizeof(float), 0);

	ImGui::Begin("Size Edit", nullptr, myImGuiFlagCombo2);
	ImGui::SetWindowPos(ImVec2(0.0f, 4 * WindowPtr->getSize().y / 10));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 2, 3 * WindowPtr->getSize().y / 10));

	ImGui::Text("Sizes: X/Y/Z");

	if (ImGui::InputFloat3("Speed ##SizeSpeed", &Sizes[0][0])) {
		for (short i = 0; i < 3; i++)
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(SizeAddress[0] + i * 4), &Sizes[0][i], sizeof(float), 0);
	}

	if (ImGui::InputFloat3("Fly ##SizeFly", &Sizes[1][0])) {
		for (short i = 0; i < 3; i++)
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(SizeAddress[1] + i * 4), &Sizes[1][i], sizeof(float), 0);
	}

	if (ImGui::InputFloat3("Power ##SizePower", &Sizes[2][0])) {
		for (short i = 0; i < 3; i++)
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(SizeAddress[2] + i * 4), &Sizes[2][i], sizeof(float), 0);
	}

	static bool SizeEditCheckBox = 0;
	if (ImGui::Checkbox("Edit Size (Freeze)", &SizeEditCheckBox)) {

		static unsigned const char OrginalCode[12] = {
			0xD9, 0x1E, 0xD8, 0x46,
			0xD9, 0x5E, 0x04, 0xD9,
			0xD9, 0x5E, 0x08, 0x83
		};
		static unsigned const char FreezeCode[12] = {
			0xDD,0xD8,0xD8,0x46,
			0xDD,0xD8,0x90,0xD9,
			0xDD,0xD8,0x90,0x83
		};

		if (SizeEditCheckBox) {
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00429DAD, FreezeCode, sizeof(int), 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00429DB2, &FreezeCode[4], sizeof(int), 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00429DBC, &FreezeCode[8], sizeof(int), 0);
		}

		else {
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00429DAD, OrginalCode, sizeof(int), 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00429DB2, &OrginalCode[4], sizeof(int), 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x00429DBC, &OrginalCode[8], sizeof(int), 0);
		}
	}

	ImGui::End();
}
void MemoryEditor::PositionEdit() {
	static float TeleportMeToThisCoordinate[3] = { NULL,100.0f,NULL };
	static unsigned const char PositionOffset[3] = {
		0xE8, // X
		0xEC, // Y
		0xF0 }; // Z

	for (short i = 0; i < 3; i++) for (short j = 0; j < 3; j++)
		ReadProcessMemory(SonicHeroesHandle, (PBYTE*)(CharactersMainAddresses[i] + PositionOffset[j]), &Positions[i][j], sizeof(float), 0);

	ImGui::Begin("Active Character Position",nullptr,myImGuiFlagCombo2);

	ImGui::SetWindowPos(ImVec2(WindowPtr->getSize().x / 2, 4 * WindowPtr->getSize().y / 10));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 2, 2.5f * WindowPtr->getSize().y / 10));

	ImGui::Text("X/Y/Z Coordinate");
	ImGui::InputFloat3("##XYZCoordinate",TeleportMeToThisCoordinate);

	if (ImGui::Button("Teleport Me")) {
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(CharactersMainAddresses[(3 - ActiveRole) % 3] + PositionOffset[0]), TeleportMeToThisCoordinate, (SIZE_T)12, 0);
	}

	if (ImGui::InputFloat3("Speed Character Position", Positions[0],"%0.2f")) {
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(CharactersMainAddresses[0] + PositionOffset[0]), Positions[0], (SIZE_T)12, 0);
	}
	if (ImGui::InputFloat3("Fly Character Position", Positions[1], "%0.2f")) {
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(CharactersMainAddresses[1] + PositionOffset[0]), Positions[1], (SIZE_T)12, 0);
	}
	if (ImGui::InputFloat3("Power Character Position", Positions[2], "%0.2f")) {
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(CharactersMainAddresses[2] + PositionOffset[0]), Positions[2], (SIZE_T)12, 0);
	}

	ImGui::End();

}
void MemoryEditor::CameraEdit() {

	for (short i = 0; i < 3; i++) {
		// Camera Position
		ReadProcessMemory(SonicHeroesHandle, (PBYTE*)(0x00A60C30 + 0x4 * i), &CameraPosition[i], sizeof(float), 0);
		// Camera Rotation
		ReadProcessMemory(SonicHeroesHandle, (PBYTE*)(0x00A60C3C + 0x4 * i), &CameraRotation[i], sizeof(short), 0);
	}

	ImGui::Begin("Camera Edit",nullptr,myImGuiFlagCombo2);

	ImGui::SetWindowPos(ImVec2(0.0f, 7 * WindowPtr->getSize().y / 10));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 2, 3 * WindowPtr->getSize().y / 10));

	// ImGui::PushItemWidth(240.0f);

	ImGui::Text("Camera Position X/Y/Z");
	if (ImGui::InputFloat3("##CameraPositions", CameraPosition, "%.2f")) {
		for (short i = 0; i < 3; i++) {
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x00A60C30 + 0x4 * i), &CameraPosition[i], sizeof(float), 0);
		}
	}

	static int TempCameraRot[3] = { NULL,NULL,NULL };
	for (short i = 0; i < 3; i++) TempCameraRot[i] = CameraRotation[i];

	static bool UseSliderCheckBox = 0;
	ImGui::Checkbox("Use Slider",&UseSliderCheckBox);

	ImGui::Text("Camera Rotation Axis Min: 0 Max: 65535");

	static bool BeingUsedFunction = 0;

	if (UseSliderCheckBox) {
		BeingUsedFunction = ImGui::SliderInt3("##CameraRotationsSlider", TempCameraRot, 0, 65536);
	}

	else {
		BeingUsedFunction = ImGui::InputInt3("##CameraRotationsInput", TempCameraRot);
	}

	if (BeingUsedFunction) {
		for (short i = 0; i < 3; i++) {
			TempCameraRot[i] = std::clamp(TempCameraRot[i], 0, 65535);
			CameraRotation[i] = TempCameraRot[i];
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x00A60C3C + 0x4 * i), &CameraRotation[i], sizeof(short), 0);
		}
	}

	static bool FreeCameraCheckbox = 0;
	if (ImGui::Checkbox("Free Camera", &FreeCameraCheckbox)) {
		static unsigned const char XOrginalCode[2] = { 0x89,0x11 };
		static unsigned const char YOrginalCode[3] = { 0x89, 0x51, 0x04 };
		static unsigned const char ZOrginalCode[3] = { 0x89, 0x51, 0x08 };

		if (FreeCameraCheckbox) {
			// Positions
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x006207DB), UsefulNops, (SIZE_T)2, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x006207E3), UsefulNops, (SIZE_T)3, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x006207EC), UsefulNops, (SIZE_T)3, 0);

			// Rotations
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x006207F8), UsefulNops, (SIZE_T)2, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x00620800), UsefulNops, (SIZE_T)3, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x00620809), UsefulNops, (SIZE_T)3, 0);
		}
		else {
			// Positions
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x006207DB), XOrginalCode, (SIZE_T)2, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x006207E3), YOrginalCode, (SIZE_T)3, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x006207EC), ZOrginalCode, (SIZE_T)3, 0);

			// Rotations
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x006207F8), XOrginalCode, (SIZE_T)2, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x00620800), YOrginalCode, (SIZE_T)3, 0);
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(0x00620809), ZOrginalCode, (SIZE_T)3, 0);
		}
	}

	if (FreeCameraCheckbox) {
		static bool FreeCameraControlCheckBox = 1;
		ImGui::Checkbox("Free Camera Control", &FreeCameraControlCheckBox);

		if (SonicHeroesHWND == GetForegroundWindow() &&
			FreeCameraControlCheckBox &&
			(Game_State == GameState::InGame ||
				Game_State == GameState::Unknown4 ||
				Game_State == GameState::InGameSceneFrozen)) FreeCameraControl();

		ImGui::Text("If Free Camera Control is enabled,\nit is recommended to Freeze Stage.");

	}

	if (ImGui::Button("Teleport Me to Camera Position")) {
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)(CharactersMainAddresses[(3 - ActiveRole) % 3] + 0xE8), CameraPosition, (SIZE_T)12, 0);
	}

	ImGui::End();
}
void MemoryEditor::ColorEdit() {
	ImGui::Begin("Color Edit", nullptr, myImGuiFlagCombo2);

	ImGui::SetWindowPos(ImVec2(WindowPtr->getSize().x / 2, 6.5f * WindowPtr->getSize().y / 10));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 3, 3.5f * WindowPtr->getSize().y / 10));

	ImGui::SetNextItemWidth(1.7f * WindowPtr->getSize().y / 7);
	ImGui::ColorPicker4("Select Color",ColorEditVaribles);

	for (short i = 0; i < 4; i++) WritableValue[i] = ColorEditVaribles[i] * 0xff;

	ImGui::End();

	ImGui::Begin("Color Assign", nullptr, myImGuiFlagCombo2);

	ImGui::SetWindowPos(ImVec2(WindowPtr->getSize().x / 1.2f, 6.5f * WindowPtr->getSize().y / 10));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 6 , 3.5f * WindowPtr->getSize().y / 10));

	ImGui::Text("Color Address");
	static char ColorAddressInput[7] = "8C729C";
	static unsigned int ColorAddress = 0x8C729C;
	static char* ErrorControl = new char[1];
	ImGui::InputText("##AddressInput",ColorAddressInput,(size_t)7, ImGuiInputTextFlags_CharsUppercase);

	ColorAddress = std::strtol(ColorAddressInput,&ErrorControl,0x10);

	if (*ErrorControl=='\0') {
		ImGui::Text("If you write incorrect address\nGame may crash.");
	}
	else {
		ImGui::Text("Address isn't Hexadecimal.\nAre you sure the Address is true?");
	}

	if (ImGui::Button("Assign Color")) {
		if (*ErrorControl == '\0') {
			WriteProcessMemory(SonicHeroesHandle, (PBYTE*)ColorAddress, WritableValue, sizeof(int), 0);
		}
	}

	ImGui::End();
}

// On Menu Functions
void MemoryEditor::CharacterOverride() {
	ImGui::Begin("Character Overrides", nullptr);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 2, WindowPtr->getSize().y));

	ImGui::Text("All overrides may cause\nthe game to crash!");

	static const char* CharacterOverrideListbox[12] = {
	"Sonic", "Knuckles", "Tails",
	"Shadow", "Omega", "Rouge",
	"Amy", "Big", "Cream",
	"Espio", "Vector", "Charmy"
	};

	static int SelectedCharacterOverride[12] = { 0,1,2,3,4,5,6,7,8,9,10,11 };

	for (short i = 0; i < 12; i++) {
		if (ImGui::Combo(CharacterOverrideListbox[i], &SelectedCharacterOverride[i], CharacterOverrideListbox, IM_ARRAYSIZE(CharacterOverrideListbox)) && CharacterOverrideCheckBox) {
			CharacterChanger(SonicHeroesHandle, i, SelectedCharacterOverride[i]);
		}

	}
	ImGui::Text("Character Override Enable?");
	if (ImGui::Checkbox("##CharacterOverride", &CharacterOverrideCheckBox)) {

		if (CharacterOverrideCheckBox) {
			for (short i = 0; i < 12; i++)
				CharacterChanger(SonicHeroesHandle, i, SelectedCharacterOverride[i]);
		}

		else {
			for (short i = 0; i < 12; i++)
				CharacterChanger(SonicHeroesHandle, i, i);
		}
	}

	ImGui::End();
}
void MemoryEditor::StageTeamOverride() {
	static const char* StageCodesListbox[38] = {
	"Dont Override" ,"Seaside Hill" ,"Ocean Place" ,
	"Grand Metropolis" ,"Power Plant" ,"Casino Park" ,
	"Bingo Highway" ,"Rail Canyon" ,"Bullet Station" ,
	"Frog Forest" ,"Lost Jungle" ,"Hang Castle" ,
	"Mystic Mansion" ,"Egg Fleet" ,"Final Fortress" ,
	"EGG HAWK" ,"TEAM ?? 1" ,"ROBOT CARNIVAL" ,
	"EGG ALBATROS" ,"TEAM ?? 2" ,"ROBOT STORM" ,
	"EGG EMPEROR" ,"METAL MADNESS" ,"METAL SONIC" ,
	"Bonus Stage 1" ,"Bonus Stage 2" ,"Bonus Stage 3" ,
	"Bonus Stage 4" ,"Bonus Stage 5" ,"Bonus Stage 6" ,
	"Bonus Stage 7" ,"Emerald Challange 1" ,"Emerald Challange 2" ,
	"Emerald Challange 3" ,"Emerald Challange 4" ,"Emerald Challange 5" ,
	"Emerald Challange 6" ,"Emerald Challange 7" };

	static const char* TeamCodesListbox[5] = { "Dont Override", "Team Sonic","Team Dark", "Team Rose", "Team Chaotix" };
	static int TeamCodesListboxSelect = 0;

	static int SelectedStageOverride = 0;
	static unsigned char OverrideStageCode = 0;

	ImGui::Begin("Stage and Team Override");
	ImGui::SetWindowPos(ImVec2(WindowPtr->getSize().x / 2, 0.0f));
	ImGui::SetWindowSize(ImVec2(WindowPtr->getSize().x / 2, WindowPtr->getSize().y));

	ImGui::Combo("Stage Override", &SelectedStageOverride, StageCodesListbox, IM_ARRAYSIZE(StageCodesListbox));

	if (SelectedStageOverride > 0 && SelectedStageOverride <= 23)
		OverrideStageCode = SelectedStageOverride + 1;
	else if (SelectedStageOverride > 23 && SelectedStageOverride <= 30)
		OverrideStageCode = SelectedStageOverride + 5;
	else if (SelectedStageOverride > 30 && SelectedStageOverride <= 37)
		OverrideStageCode = SelectedStageOverride + 21;
	else
		OverrideStageCode = 1;

	ImGui::Combo("Team Override", &TeamCodesListboxSelect, TeamCodesListbox, IM_ARRAYSIZE(TeamCodesListbox));
	TeamCodesListboxSelect -= 1;
	if (TeamCodesListboxSelect < -1 || TeamCodesListboxSelect > 3) TeamCodesListboxSelect = -1;

	ImGui::End();

	if (OverrideStageCode != 1)
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x008D6720, &OverrideStageCode, (SIZE_T)1, 0);
	if (TeamCodesListboxSelect != -1) {
		WriteProcessMemory(SonicHeroesHandle, (PBYTE*)0x008D6920, &TeamCodesListboxSelect, (SIZE_T)1, 0);
	}

	TeamCodesListboxSelect += 1;

}

// Sonic Heroes Closed Functions