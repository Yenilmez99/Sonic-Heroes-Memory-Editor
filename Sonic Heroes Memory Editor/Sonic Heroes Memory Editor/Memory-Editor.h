#pragma once

const unsigned char UsefulNops[16] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

struct UsefulVarIntager {
    int VarIntager = NULL;
    bool CheckBoxControl = NULL;
};
struct UsefulVarFloat {
    float VarFloat = NULL;
    bool CheckBoxControl = NULL;
};

enum GameState : unsigned char // i copied in Sonic Heroes SDK
{
    // Game state is not set.
    Null = 0,

    // Browsing the main menu. This state essentially has no effect.
    Menu = 1,

    // The level is starting to load/is being initialized.
    StartLevelLoad = 2,

    // Level has loaded. Animate the titlecard fadeout.
    EndLevelLoad = 3,

    Unknown4 = 4,

    // Player
    InGame = 5,

    // Displays the main pause menu.
    InGamePaused = 6,

    // Displays the controller rebinding menu.
    InGamePausedSettings = 7,

    // Displays the Auto/Free Camera menu.
    InGamePausedSettingsCamera = 8,

    // Displays the controller rebinding menu.
    InGamePausedSettingsRebinding = 9,

    // Executes ACTION::FreezeExec.
    // Freezes all gameplay while keeping camera active.
    InGameSceneFrozen = 10,

    // Used to exit the level and save game.
    InGameExitWithSave = 11,

    // Used for exit: ???
    Unknown13Exit = 13,

    // Executed when the character wins the stage.
    InGameLevelWin = 15,

    // Used for exit: ???
    InGameExit2 = 16,

    // Used for exit: ???
    Unknown17Exit = 17,
};

class MemoryEditor {
public:
    MemoryEditor(sf::RenderWindow* WindowAddr,sf::Time* DeltaTimeAddr);
    ~MemoryEditor();

    bool Sonic_Heroes_Is_Open();
    void LoadCharacterAddress();
    void LoadPointers();
    unsigned char GetGameState();
    void SetGameState(unsigned char Game_State);
    void FreeCameraControl();

    // Stage On Functions
    void RingAmmoEdit();
    void TeamBlastEdit();
    void CharacterLevelEdit();
    void TimerEdit();
    void FlyBarEdit();
    void ExternalForceEdit();
    void PointEdit();
    void ExtraEdits();
    void SizeEdit();
    void PositionEdit();
    void CameraEdit();
    void ColorEdit();

    // Stage Off functions
    void CharacterOverride();
    void StageTeamOverride();

    void Draw();

private:
    sf::RenderWindow* WindowPtr = nullptr;  HWND SonicHeroesHWND = NULL;    HANDLE SonicHeroesHandle = NULL;
    sf::Time* myDeltaTime;  unsigned char Game_State = NULL;    bool FirstTimeOpenedStage = 0;
    UsefulVarIntager Ring;  UsefulVarFloat TeamBlast;   bool CharacterOverrideCheckBox = 0;
    int CharactersMainAddresses[3] = { NULL,NULL,NULL };    int CompareCharacterAddress[3] = { NULL,NULL,NULL };
    int CharacterLevelAddress = NULL;   UsefulVarIntager CharacterLevels[3];    int ActiveRoleAddress = NULL;   unsigned char ActiveRole = NULL;
    int Timer[3] = { NULL,NULL,NULL };
    UsefulVarFloat FlyBar; int FlyBarAddress = NULL;
    int ExternalForceAddress = NULL;    float ExternalForce[2] = { 10.0f,10.0f }; char ForceKeys[4] = { 'R','\0','E','\0' };
    int CharacterPoint[4] = { NULL,NULL,NULL,NULL };
    UsefulVarIntager Live;
    float Positions[3][3] = { {NULL,NULL,NULL},{NULL,NULL,NULL},{NULL,NULL,NULL} };
    bool StageFreezeCheckbox = 0;
    float CameraPosition[3] = { NULL,NULL,NULL };   unsigned short CameraRotation[3] = { NULL,NULL,NULL };  float CameraMovSpeed = 5.0f;
    float ColorEditVaribles[4] = { 0.0f,0.0f,0.0f,1.0f };   unsigned char WritableValue[4] = { 0,0,0,0xff };

};
