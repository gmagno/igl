
#include "widgets.h"

void IGL::GuiManager::DockSpace()
{
    bool opt_fullscreen = true;
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode;
    dockspace_flags |= ImGuiDockNodeFlags_AutoHideTabBar;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetWorkPos());
    ImGui::SetNextWindowSize(viewport->GetWorkSize());
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    window_flags |= ImGuiWindowFlags_NoNavFocus;

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);

    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO &io = ImGui::GetIO();
    ImGuiID dockspace_id = 0;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        dockspace_id = ImGui::GetID("MyDockSpace");

        if (m_initialized == 1)
        {
            m_initialized = 2;
            ImGuiContext *ctx = ImGui::GetCurrentContext();
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(
                dockspace_id, ImGuiDockNodeFlags_DockSpace);

            // create layout
            ImGuiID dock_main_id = dockspace_id;
            ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(
                dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
            ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(
                dock_main_id, ImGuiDir_Down, 1.20f, NULL, &dock_main_id);
            ImGui::DockBuilderDockWindow(
                m_image_viewer_title.c_str(), dock_main_id);
            ImGui::DockBuilderDockWindow(
                m_file_explorer_title.c_str(), dock_id_left);
            ImGui::DockBuilderDockWindow(
                m_log_title.c_str(), dock_id_bottom);

            ImGui::DockBuilderFinish(dockspace_id);
        }

        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        if (m_initialized == 2)
        {
            ImageViewer();
            if (m_render_left)
                FileExplorer();
            if (m_render_bottom)
                Log();

            if (ImGui::IsKeyReleased(GLFW_KEY_R))
                ResetLayout();
            if (ImGui::IsKeyReleased(GLFW_KEY_E))
                m_render_left = !m_render_left;
            // if (ImGui::IsKeyReleased(GLFW_KEY_B))
            //     m_render_bottom = !m_render_bottom;

            // show help overlay
            if (ImGui::IsKeyReleased(GLFW_KEY_H))
                m_show_help_overlay = !m_show_help_overlay;
            if (m_show_help_overlay)
                HelpOverlay();

            // change theme
            if (ImGui::IsKeyReleased(GLFW_KEY_T))
            {
                m_style = (m_style + 1) % 3;
                if (m_style == 2)
                    ImGui::StyleColorsClassic();
                else if (m_style == 1)
                    ImGui::StyleColorsLight();
                else
                    ImGui::StyleColorsDark();
            }

            // Change selected image
            if (io.KeyAlt)
            {
                if (io.MouseWheel < 0.0f)
                    SelectNext();
                else if (io.MouseWheel > 0.0f)
                    SelectPrevious();
            }
            if (ImGui::IsKeyPressed(GLFW_KEY_K))
                SelectPrevious();
            if (ImGui::IsKeyPressed(GLFW_KEY_J))
                SelectNext();

            // center image
            if (io.KeysDown[GLFW_KEY_C])
            {
                m_ref_x = 0.0f;
                m_ref_y = 0.0f;
            }

            if (io.KeyCtrl) // zoom image
            {
                if (ImGui::IsKeyPressed(GLFW_KEY_EQUAL))
                    Zoom(10.0f);
                if (ImGui::IsKeyPressed(GLFW_KEY_MINUS))
                    Zoom(-10.0f);
            }
            else // pan image
            {
                if (ImGui::IsKeyPressed(GLFW_KEY_W))
                    PanVertically(2.0f);
                if (ImGui::IsKeyPressed(GLFW_KEY_S))
                    PanVertically(-2.0f);
                if (ImGui::IsKeyPressed(GLFW_KEY_A))
                    PanHorizontally(2.0f);
                if (ImGui::IsKeyPressed(GLFW_KEY_D))
                    PanHorizontally(-2.0f);
            }
        }
    }
    else
    {
        ImGui::Text("ERROR: Docking is not enabled!");
    }

    ImGui::End();
    return;
}

void IGL::GuiManager::ResetLayout()
{
    m_initialized = 1;
    m_render_left = true;
    m_render_bottom = false;
}

void IGL::GuiManager::PanHorizontally(float amount)
{
    m_ref_x += amount * 20.0f / m_scaling;
}

void IGL::GuiManager::PanVertically(float amount)
{
    m_ref_y += amount * 20.0f / m_scaling;
}

void IGL::GuiManager::Zoom(float amount)
{
    m_scaling += (amount > 0.0f ? 1.0f : -1.0f) * 0.1f * m_scaling;
    m_scaling = m_scaling < 0.01 ? 0.01 : m_scaling;
}

void IGL::GuiManager::ImageViewer()
{
    ImGuiIO &io = ImGui::GetIO();

    // image window
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar;
    flags |= ImGuiWindowFlags_NoScrollWithMouse;
    // flags |= ImGuiWindowFlags_NoTitleBar;
    ImGui::Begin(m_image_viewer_title.c_str(), 0, flags);

    ImVec2 win_size = ImGui::GetWindowSize();
    ImVec2 cursor_pos = ImGui::GetCursorPos();
    ImVec2 mouse_pos = ImGui::GetMousePos();
    ImVec2 win_pos = ImGui::GetWindowPos();

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None))
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
        {
            m_mouse_drag_init = ImGui::GetMousePos();
            m_ref_drag_init.x = m_ref_x;
            m_ref_drag_init.y = m_ref_y;
        }
        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        {
            ImVec2 mouse_pos = ImGui::GetMousePos();
            m_ref_x = m_ref_drag_init.x +
                      (mouse_pos.x - m_mouse_drag_init.x) / m_scaling;
            m_ref_y = m_ref_drag_init.y +
                      (mouse_pos.y - m_mouse_drag_init.y) / m_scaling;
        }
        if (io.MouseWheel != 0.0f && !io.KeyAlt)
        {
            if (!io.KeyCtrl)
                if (io.KeyShift)
                    PanHorizontally(io.MouseWheel);
                else
                    PanVertically(io.MouseWheel);
            else
                Zoom(io.MouseWheel);
        }
    }
    // texture
    float tex_w = m_width * m_scaling, tex_h = m_height * m_scaling;
    float new_cursor_x = 0.0f, new_cursor_y = 0.0f;

    // set cursor position to the middle of the window
    new_cursor_x = (win_size.x - tex_w) * 0.5f + m_ref_x * m_scaling;
    new_cursor_y = (win_size.y - tex_h) * 0.5f + m_ref_y * m_scaling;

    ImGui::SetCursorPos(ImVec2(new_cursor_x, new_cursor_y));
    // request draw texture
    ImGui::Image((void *)(intptr_t)m_texture_id, ImVec2(tex_w, tex_h));
    ImGui::End();
    return;
}

void IGL::GuiManager::SelectPrevious()
{
    Select(m_selected_file - 1);
}

void IGL::GuiManager::SelectNext()
{
    Select(m_selected_file + 1);
}

void IGL::GuiManager::Select(int idx)
{
    int size = m_files.size();
    m_selected_file = (idx % size + size) % size;
}

void IGL::GuiManager::FileExplorer()
{
    ImGuiIO &io = ImGui::GetIO();
    ImGuiWindowFlags flags = ImGuiWindowFlags_None;
    // flags |= ImGuiWindowFlags_NoTitleBar;
    ImGui::Begin(m_file_explorer_title.c_str(), 0, flags);

    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 20.0f);
    if (ImGui::TreeNodeEx(m_cwd.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (std::size_t i = 0; i < m_files.size(); ++i)
        {
            std::string prefix = m_files_state[i];
            std::string filename = m_files[i].filename().string();
            std::string selec_text = prefix + "  " + filename;
            if (ImGui::Selectable(selec_text.c_str(), i == m_selected_file))
                m_selected_file = i;
        }
        ImGui::TreePop();
    }
    ImGui::PopStyleVar();
    ImGui::End();
    return;
}

void IGL::GuiManager::Log()
{
    ImGuiIO &io = ImGui::GetIO();
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar;
    ImGui::Begin(m_log_title.c_str(), 0, flags);

    ImGui::End();
    return;
}

void IGL::GuiManager::HelpOverlay()
{
    const float DISTANCE = 50.0f;
    ImGuiIO &io = ImGui::GetIO();
    if (m_corner != -1)
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImVec2 work_area_pos = viewport->GetWorkPos();
        ImVec2 work_area_size = viewport->GetWorkSize();
        ImVec2 window_pos = ImVec2(
            (m_corner & 1) ? (work_area_pos.x + work_area_size.x - DISTANCE)
                           : (work_area_pos.x + DISTANCE),
            (m_corner & 2) ? (work_area_pos.y + work_area_size.y - DISTANCE)
                           : (work_area_pos.y + DISTANCE));
        ImVec2 window_pos_pivot = ImVec2(
            (m_corner & 1) ? 1.0f : 0.0f,
            (m_corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(
            window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
    }
    ImGui::SetNextWindowBgAlpha(0.75f); // Transparent background
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration;
    window_flags |= ImGuiWindowFlags_NoSavedSettings;
    window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    window_flags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    if (m_corner != -1)
        window_flags |= ImGuiWindowFlags_NoMove;
    if (ImGui::Begin("HelpOverlay", 0, window_flags))
    {
        const char *header =
            "  Help\n";

        const char *help_msg =
            "  Show this help message                 : 'H'  \n"
            "\n"
            "  Image vertical/horizontal panning      : "
            "drag with middle mouse button  \n"
            "                                         : 'W', 'A', 'S', 'D'  \n"
            "                                         : "
            "[SHIFT] + mouse wheel  \n"
            "\n"
            "  Image zoom in/out                      : "
            "CTRL + mouse wheel  \n"
            "                                         : CTRL + '=', '-'  \n"
            "\n"
            "  Image recenter                         : 'C'  \n"
            "\n"
            "  Select next/previous image             : ALT + mouse wheel  \n"
            "                                         : 'J', 'K'  \n"
            "\n"
            "  Show/hide files window                 : 'E'  \n"
            "\n"
            "  Reset windows layout                   : 'R'  \n"
            "\n"
            "  Toggle theme: {dark, light, classic}   : 'T'  \n"
            "\n"
            "  Quit                                   : 'Q'  \n"
            "                                         : ESC  ";

        const char *copyright_msg = "  Copyright © 2020 Goncalo Magno "
                                    "https://gmagno.dev  \n";

        ImGui::Text(header);
        ImGui::Separator();
        ImGui::Text(help_msg);
        ImGui::Separator();
        ImGui::Text(copyright_msg);
    }
    ImGui::End();
}
