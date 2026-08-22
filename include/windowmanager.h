#pragma once

#include <string>
#include <array>
#include "GLFW/glfw3.h"

namespace shmemeditor {

    class WindowManager
    {
    private:
        GLFWwindow* window = nullptr;

    public:
        WindowManager(const std::string& WindowName, const std::array<float, 2> Size = {800,600});
        ~WindowManager();

        bool init(const std::string& WindowName, const std::array<float, 2> Size = {800,600});
        void imgui_init();
        void terminate();
        int render_loop();

    };

}