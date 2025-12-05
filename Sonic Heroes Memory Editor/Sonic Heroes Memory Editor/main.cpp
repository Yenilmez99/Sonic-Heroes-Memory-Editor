#define SFML_STATIC
#define _USE_MATH_DEFINES

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>

#include <chrono>
#include <thread>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <Windows.h>

#include "json.hpp"
#include "Memory-Editor.h"

int main() {
    sf::RenderWindow* myWindow = nullptr;
    myWindow = new sf::RenderWindow(sf::VideoMode({800,600}), "Sonic Heroes Memory Editor", sf::State::Windowed);

    sf::Time DeltaTime; // DeltaTime

    // Imgui Init
    ImGui::SFML::Init(*myWindow);

    sf::Clock myClock;
    DeltaTime = myClock.restart();

    sf::Color BackGroundColor(255, 0, 0, 255);

    MemoryEditor Sonic_Heroes_Memory_Editor(myWindow, &DeltaTime);

    while (myWindow->isOpen()) {
        while (const auto event = myWindow->pollEvent()) {

            ImGui::SFML::ProcessEvent(*myWindow, *event);

            if (event->is<sf::Event::Closed>()) {
                myWindow->close();
            }
        }
        myClock.restart();

        // Imgui Update
        ImGui::SFML::Update(*myWindow, DeltaTime);

        // Clear Window
        myWindow->clear(BackGroundColor);

        // You can write functions to after here

        Sonic_Heroes_Memory_Editor.Draw();

        // You can write functions to before here
        ImGui::SFML::Render(*myWindow);
        myWindow->display();

        std::this_thread::sleep_for(std::chrono::milliseconds(8));
        DeltaTime = myClock.restart();
    }

    ImGui::SFML::Shutdown();

    return 0;
}