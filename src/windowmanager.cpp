#include <string>
#include <iostream>
#include <thread>
#include <chrono>

#include "GLFW/glfw3.h"
#include "menues.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

#include "imgui_styles.h"

#include "windowmanager.h"
#include "helper.h"

shmemeditor::WindowManager::WindowManager(const std::string& WindowName, const std::array<float, 2> Size)
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    if (!init(WindowName,Size)) return;

    render_loop();
}
shmemeditor::WindowManager::~WindowManager()
{
    terminate();
}
bool shmemeditor::WindowManager::init(const std::string& WindowName, const std::array<float, 2> Size)
{
    if (window) glfwDestroyWindow(window);
    window = nullptr;
    window = glfwCreateWindow(Size[0], Size[1], WindowName.c_str(), NULL, NULL);

    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    imgui_init();

    return true;
}
void shmemeditor::WindowManager::imgui_init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    SetupImGuiCatppuccinMochaStyle();
}
void shmemeditor::WindowManager::terminate()
{
    if (window) glfwDestroyWindow(window);
    window = nullptr;
    glfwTerminate();
    window = nullptr;
    std::cout << "GLFW Terminated" << std::endl;
}
int shmemeditor::WindowManager::render_loop()
{
    Menues EditorMenu;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));
        ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
        ImGui::PopStyleVar();

        static bool first_setup = false;
        if (!first_setup) {
            first_setup = true;
            DockBuildLayout(dockspace_id);
        }

        EditorMenu.render();

        glClearColor(0.2f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}