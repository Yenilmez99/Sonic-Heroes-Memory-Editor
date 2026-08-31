#pragma once
#include "imgui.h"
#include "imgui_internal.h"

inline float BamsToDegrees(const int& bamsValue) {
    return static_cast<float>(bamsValue) * 0.0054931640625f; // ~360/65536
}
inline int DegreesToBams(const float& Degrees) {
    return static_cast<int>(Degrees * 182.0444444f); // ~65536/360
}

inline void DockBuildLayout(ImGuiID& dockspace_id) {
    ImGui::DockBuilderRemoveNode(dockspace_id); 
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

    // Layout Pieces
    ImGuiID dock_main_id = dockspace_id;

    // main bars
    ImGuiID dock_id_point = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.44f, NULL, &dock_main_id); // first bar
    ImGuiID dock_id_teamblast = ImGui::DockBuilderSplitNode(dock_id_point, ImGuiDir_Down, 0.53f, NULL, &dock_id_point); // second bar
    ImGuiID dock_id_position = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.56f, NULL, &dock_main_id); // third bar
    ImGuiID dock_id_size = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.99f, NULL, &dock_main_id); // fourth bar

    // first bar
    ImGuiID dock_id_power = ImGui::DockBuilderSplitNode(dock_id_point, ImGuiDir_Right, 0.60f, NULL, &dock_id_point);
    ImGuiID dock_id_time = ImGui::DockBuilderSplitNode(dock_id_power, ImGuiDir_Right, 0.40f, NULL, &dock_id_power);
    // second bar
    ImGuiID dock_id_velocity = ImGui::DockBuilderSplitNode(dock_id_teamblast, ImGuiDir_Right, 0.60f, NULL, &dock_id_teamblast);
    ImGuiID dock_id_ring = ImGui::DockBuilderSplitNode(dock_id_velocity, ImGuiDir_Right, 0.30f, NULL, &dock_id_velocity);
    ImGuiID dock_id_lives = ImGui::DockBuilderSplitNode(dock_id_velocity, ImGuiDir_Right, 0.30f, NULL, &dock_id_velocity);
    // third bar
    ImGuiID dock_id_camera = ImGui::DockBuilderSplitNode(dock_id_position, ImGuiDir_Right, 0.60f, NULL, &dock_id_position);
    ImGuiID dock_id_extras = ImGui::DockBuilderSplitNode(dock_id_camera, ImGuiDir_Right, 0.40f, NULL, &dock_id_camera);
    // fourth bar
    ImGuiID dock_id_color = ImGui::DockBuilderSplitNode(dock_id_size, ImGuiDir_Right, 0.60f, NULL, &dock_id_size);
    ImGuiID dock_id_flybar = ImGui::DockBuilderSplitNode(dock_id_color, ImGuiDir_Right, 0.40f, NULL, &dock_id_color);

    // Memory Editors Title
    ImGui::DockBuilderDockWindow("Point", dock_id_point); // first bar
    ImGui::DockBuilderDockWindow("Team Blast", dock_id_teamblast); // second bar
    ImGui::DockBuilderDockWindow("Size", dock_id_size); // fourth bar
    ImGui::DockBuilderDockWindow("Position", dock_id_position); // third bar
    // first bar
    ImGui::DockBuilderDockWindow("Character Power", dock_id_power);
    ImGui::DockBuilderDockWindow("Time", dock_id_time);
    // second bar
    ImGui::DockBuilderDockWindow("Velocity", dock_id_velocity);
    ImGui::DockBuilderDockWindow("Ring Edit", dock_id_ring);
    ImGui::DockBuilderDockWindow("Lives", dock_id_lives);
    // third bar
    ImGui::DockBuilderDockWindow("Camera", dock_id_camera);
    ImGui::DockBuilderDockWindow("Extras", dock_id_extras);
    // fourth bar
    ImGui::DockBuilderDockWindow("Color Editor", dock_id_color);
    ImGui::DockBuilderDockWindow("Fly Bar", dock_id_flybar);

    //on menu
    ImGuiID dock_id_characteroverwrite = dock_main_id;
    ImGuiID dock_id_stageoverwrite = ImGui::DockBuilderSplitNode(dock_id_characteroverwrite, ImGuiDir_Right, 0.45f, NULL, &dock_id_characteroverwrite);
    ImGuiID dock_id_teamoverwrite = ImGui::DockBuilderSplitNode(dock_id_stageoverwrite, ImGuiDir_Down, 0.50f, NULL, &dock_id_stageoverwrite);

    ImGui::DockBuilderDockWindow("Character Overwrite", dock_id_characteroverwrite);
    ImGui::DockBuilderDockWindow("Stage Overwrite", dock_id_stageoverwrite);
    ImGui::DockBuilderDockWindow("Team Overwrite", dock_id_teamoverwrite);

    ImGui::DockBuilderFinish(dockspace_id);
}