#include "igl.h"

int main(int argc, char **argv)
{
    // CLI parsing
    if (int ret = IGL::CLI(argc, argv))
        return ret;

    // Setup window
    glfwSetErrorCallback([](int error, const char *description) {
        std::cerr
            << fmt::format("Glfw Error {}: {}", error, description)
            << std::endl;
    });
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize glfw!" << std::endl;
        return 1;
    }

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    GLFWwindow *window = glfwCreateWindow(
        1280, 720, "igl -- Image OpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create glfw window!" << std::endl;
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    if (!gladLoadGL())
    {
        std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = nullptr; // don't generate the .ini file
    io.ConfigDockingWithShift = true;
    io.FontAllowUserScaling = false; // disable win scaling on ctrl+mouse wheel
    io.WantCaptureKeyboard = true;
    // io.WantCaptureMouse = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    IGL::SetupLogger();
    IGL::LoadFonts();

    double prev_time = glfwGetTime();
    double curr_time = prev_time;

    IGL::ConfigManager &cfg_man = IGL::ConfigManager::getInstance();
    IGL::Loader loader;

    // FIXME: ugly code
    if (std::filesystem::is_directory(cfg_man.m_target_path))
        loader.m_cwd = cfg_man.m_target_path;
    else
        loader.m_cwd = cfg_man.m_target_path.parent_path();

    loader.RefreshFilesList();
    loader.SetPivot(cfg_man.m_target_path);
    loader.StartWorkers();

    IGL::GuiManager gui;
    gui.m_cwd = std::filesystem::canonical(loader.m_cwd);
    gui.m_files = loader.GetFilesPaths();
    gui.m_files_state = loader.GetFilesState();
    gui.m_selected_file = loader.m_pivot;

    // main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::IsKeyReleased(GLFW_KEY_Q) ||
            ImGui::IsKeyReleased(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        curr_time = glfwGetTime();
        if (curr_time - prev_time > .01)
        {
            prev_time = curr_time;
            loader.UnloadImageFromGPU();
            loader.LoadImageToGPU();

            // FIXME: this ain't pretty...
            if (loader.m_pivot >= 0)
            {
                gui.m_texture_id = loader.m_textures[loader.m_pivot].m_texture_id;
                gui.m_width = loader.m_textures[loader.m_pivot].m_width;
                gui.m_height = loader.m_textures[loader.m_pivot].m_height;
            }
        }

        loader.SetPivot(gui.m_selected_file);
        gui.m_files_state = loader.GetFilesState(); // FIXME: ugly code

        gui.DockSpace();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    // signal threads to stop and block until they return
    loader.StopWorkers();

    spdlog::get("main")->info("Terminated igl!");
    return 0;
}

void IGL::SetupLogger()
{
    spdlog::set_pattern("[%H:%M:%S.%e %z] [t %t] %l:: %v");
    auto logger = spdlog::stdout_color_mt("main");
    // auto logger = spdlog::basic_logger_mt("main", "igl.log", true);
    logger->flush_on(spdlog::level::debug);
    logger->set_level(spdlog::level::off);
    spdlog::get("main")->info("Running igl!");
}

void IGL::LoadFonts()
{
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        Hermit_compressed_data, Hermit_compressed_size, 25.0f);
}
