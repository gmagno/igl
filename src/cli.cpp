#include "cli.h"

int IGL::CLI(int argc, char **argv)
{
    ConfigManager &cfg_man = ConfigManager::getInstance();

    CLI::App app{"IGL - Image OpenGL"};
    app.set_help_flag();
    auto help = app.add_flag("-h,--help", "Request help");
    // app.set_version_flag("--version", std::string("v0.1.0"));
    app.add_option(
           "path", cfg_man.m_target_path,
           "Path to image or directory")
        ->check([&cfg_man](const std::filesystem::path &path) {
            // directories are okay
            if (std::filesystem::is_directory(path))
                return std::string();
            else if (!std::filesystem::is_regular_file(path))
                return std::string(
                    "Invalid path, please provide a path to a supported image "
                    "file or a directory.");

            // regular files with a supported format are okay
            // extension string to lower case
            std::string ext = path.extension().string();
            std::transform(
                ext.begin(), ext.end(), ext.begin(),
                [](unsigned char c) { return std::tolower(c); });
            auto match = std::find(
                std::begin(cfg_man.m_supported_ext),
                std::end(cfg_man.m_supported_ext),
                ext);
            if (match == std::end(cfg_man.m_supported_ext))
                return std::string(
                    "Non supported format. Currently supported formats: " +
                    std::accumulate(
                        cfg_man.m_supported_ext.begin(),
                        cfg_man.m_supported_ext.end(),
                        std::string(""),
                        [](std::string a, std::string b) {
                            return a.empty() ? b : a + ", " + b;
                        }));
            return std::string();
        });

    // CLI11_PARSE(app, argc, argv);
    int ret = 0;
    try
    {
        app.parse(argc, argv);
        if (*help)
            throw CLI::CallForHelp();
    }
    catch (const CLI::Error &e)
    {
        ret = app.exit(e);
        if (*help)
            return -1;
    }

    cfg_man.m_target_path = std::filesystem::canonical(cfg_man.m_target_path);
    return ret;
}
