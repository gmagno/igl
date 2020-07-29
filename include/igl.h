#ifndef __IGL_H__
#define __IGL_H__

#include "cli.h"
#include "loader.h"
#include "hermit.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>  // Initialize with gladLoadGL()
#include <GLFW/glfw3.h> // Include glfw3.h after OpenGL definitions

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include "widgets.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "fmt/format.h"
#include <chrono>
#include <iostream>

namespace IGL
{
    void SetupLogger();
    void LoadFonts();
    void Render();

} // namespace IGL

#endif // __IGL_H__
