#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#include "loader.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <filesystem>

namespace IGL
{
    class GuiManager
    {
    public:
        void DockSpace();
        void ImageViewer();
        void FileExplorer();
        void Log();
        void ResetLayout();
        void HelpOverlay();
        void Select(int idx);
        void SelectNext();
        void SelectPrevious();
        void PanHorizontally(float amount);
        void PanVertically(float amount);
        void Zoom(float amount);

        // private:

        // DockSpace attributes
        int m_initialized = 1;
        bool m_render_left = true;
        bool m_render_bottom = false;

        // ImageViewer attributes
        std::string m_image_viewer_title = "Canvas";
        GLuint m_texture_id = 0;
        int m_width = 100;
        int m_height = 100;
        float m_scaling = 1.0f;
        float m_ref_x = 0.0f, m_ref_y = 0.0f;
        ImVec2 m_mouse_drag_init;
        ImVec2 m_ref_drag_init;

        // FileExplorer attributes
        std::string m_file_explorer_title = "Explorer";
        std::string m_cwd = std::filesystem::current_path().c_str();
        std::vector<std::filesystem::path> m_files;
        std::vector<std::string> m_files_state;
        int m_selected_file = 0;

        // Log attributes
        std::string m_log_title = "Logs";

        // Help Overlay attributes
        int m_corner = 3;
        bool m_show_help_overlay = false;

        // other
        int m_style = 0; // theme
    };

    void HelpOverlay();

} // namespace IGL
#endif // __WIDGETS_H__
