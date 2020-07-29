#ifndef __CLI_H__
#define __CLI_H__

#include "CLI/CLI.hpp"
#include "CLI/App.hpp"
#include <iostream>
#include <filesystem>
#include <numeric>

namespace IGL
{

    class ConfigManager
    {
    public:
        static ConfigManager &getInstance()
        {
            static ConfigManager instance;
            return instance;
        }

        ConfigManager(ConfigManager const &) = delete;
        void operator=(ConfigManager const &) = delete;

        std::filesystem::path m_target_path = ".";
        std::vector<std::string> m_supported_ext = {
            ".png", ".jpg", ".jpeg", ".bmp"};

    private:
        ConfigManager() {}
    };

    int CLI(int argc, char **argv);

} // namespace IGL
#endif // __CLI_H__
