#pragma once

#include "memprocess.h"

#include "imgui.h"

namespace shmemeditor {
    
    class Menues {
    private:
        yen::memprocess::Process* editor = nullptr;
        void Extra();
        void Ring();
        void Power();
        void Time();
        void Point();
        void TeamBlast();
    
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
            if(!editor->read(0x008D66F0,GameState)) { // The Game have not been started yet
                return;
            }

            if (GameState > 3 && GameState < 11) { // In Game
                Extra();
                Ring();
                Power();
                Time();
                Point();
                TeamBlast();

                return;
            }
            else if (GameState < 2) { // Menu
                return;
            }
            else { // Others
            
            }
            
        }

    };

}
