#ifndef __LOADER_H__
#define __LOADER_H__

#include "glad/glad.h"
#include "spdlog/spdlog.h"
#include "fmt/format.h"

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <filesystem>

namespace IGL
{
    class Texture
    {

    public:
        // Texture() {}
        Texture(std::filesystem::path path) : m_path(path) {}

        /**
         * Move initialization.
        */
        Texture(Texture &&other);

        /**
         * Copy initialization.
        */
        Texture(const Texture &other);

        /**
         * Move assignment.
        */
        Texture &operator=(Texture &&other);

        /**
         * Copy assignment.
        */
        Texture &operator=(const Texture &other);

        /**
         * Allows sorting textures by path name.
        */
        friend bool operator<(const Texture &l, const Texture &r)
        {
            return l.m_path < r.m_path;
        }

        // private:
        std::filesystem::path m_path;
        int m_width = 100;
        int m_height = 100;
        unsigned char *m_data = 0; // memory
        GLuint m_texture_id = 0;   // gpu
        // TextureStatus m_mem_status = TextureStatus::OnHold;
        mutable std::mutex m_mutex;
    };

    class Loader
    {
    public:
        /**
         * starts threads which run `Work` method
        */
        void StartWorkers();
        void StopWorkers(); // blocking call: waits for threads to return

        /**
         * continuously checks for textures with state
         * `TextureLoadingStatus::AwaitingWorker` and loads the corresponding
         * file to memory. It also checks
         * 
        */
        void Work(int id);

        /**
         * Refreshes the list of files that can be loaded by the workers in
         * background. It first frees all the previously allocated memory and
         * then does the refreshing.
         * Note:
         * This method must only be called by the main (OpenGL context) thread!
         * @param dirpath the path to the files directory.
        */
        void RefreshFilesList(std::filesystem::path dirpath);
        void RefreshFilesList();

        /**
         * Returns a vector with the files paths
        */
        std::vector<std::filesystem::path> GetFilesPaths();

        /**
         * Returns a vector with the files memory state
        */
        std::vector<std::string> GetFilesState();

        /**
         * 
        */
        // void SetBufItems(int max_buf_size);
        void SetBufItems();

        /**
         * 
        */
        void SetPivot(int index);
        void SetPivot(std::filesystem::path filename);

        /**
         * Tries to get lock to one of the textures and loads the one first
         * found InMem to GPU
        */
        void LoadImageToGPU();
        void LoadImageToGPU(int file_id); // internal, should not be used directly!

        void UnloadImageFromGPU();

        // private:
        unsigned char *LoadImageToMem( // internal, should not be used directly
            std::filesystem::path filename, int *out_width, int *out_height);

        std::vector<Texture> m_textures;
        std::set<int> m_buf;        // indices of files with buffer. These files can be loaded to memory/gpu
        int m_max_buf_size = 10;    // represents a max buf size of 5 images
        int m_pivot = -1;           // stores the index of the file being depicted
        bool m_keep_working = true; // threads read only
        std::vector<std::string> m_supported_ext = {
            ".png", ".jpg", ".jpeg", ".bmp"};
        std::vector<std::thread *> m_workers;
        std::filesystem::path m_cwd = std::filesystem::current_path();
        const int m_thread_sleep_ms = 10; // miliseconds
    };

} // namespace IGL

#endif // __LOADER_H__
