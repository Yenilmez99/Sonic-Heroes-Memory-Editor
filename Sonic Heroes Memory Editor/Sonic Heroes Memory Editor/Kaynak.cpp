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

#include "Funcs.h"

#define FrameRateLock 144

int main() {

    int RunStage = 0;
    float SHMEOpacityBefore = 1.0f, SHMEOpacityAfter = 1.0f;

    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(1024, 720, "Sonic Heroes Memory Editor", NULL, NULL);

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

    DWORD ProcessIDSonicHeroes;
    DWORD SonicHeroesBaseAdress;

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

        if (RunStage == 0)
            StageOn(HandleSonicHeroes, hwnd_SonicHeroesTM,SonicHeroesBaseAdress);

        else
            StageOff(HandleSonicHeroes);

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();

        Sleep(1000 / FrameRateLock); // Frame Rate Lock 1000/FPS
    }

    return 0;
}