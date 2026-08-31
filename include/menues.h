#pragma once

#include "memprocess.h"

#include "imgui.h"
#include "addresses.h"

namespace shmemeditor {
    
    class Menues {
    private:
        yen::memprocess::Process* editor = nullptr;

        void Extras();
        void Ring();
        void Power();
        void Time();
        void Point();
        void TeamBlast();
        void FlyBar();
        void ColorEditor();
        void Position();
        void Lives();
        void Camera();
        void Velocity();
        void Size();

        void CharacterOverwrite();
        void TeamOverwrite();
        void StageOverwrite();
    
    public:
        Menues();
        ~Menues();

        void init();
        void terminate();

        void render()
        {
            if (!editor->is_alive()) {
                editor->terminate();
                editor->init(yen::memprocess::get_pid(L"Tsonic_win.exe"));

                ImGui::Begin("Game not started");
                ImGui::End();

                return;
            }

            uint8_t GameState = 0;
            if(!editor->read(shmemeditor::addresses::GameState,GameState)) { // The Game have not been started yet
                return;
            }

            if (GameState > 3 && GameState < 11) { // In Game
                Extras();
                Ring();
                Power();
                Time();
                Point();
                TeamBlast();
                FlyBar();
                ColorEditor();
                Position();
                Lives();
                Camera();
                Velocity();
                Size();

                return;
            }
            else if (GameState < 2) { // Menu
                CharacterOverwrite();
                TeamOverwrite();
                StageOverwrite();

                return;
            }
            else { // Others
            
            }
            
        }

    };

}
