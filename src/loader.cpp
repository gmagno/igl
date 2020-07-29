
#include "loader.h"
#include "stb_image.h"

IGL::Texture::Texture(Texture &&other)
{
    std::lock_guard<std::mutex> lock(other.m_mutex);
    m_path = std::move(other.m_path);
    m_width = std::move(other.m_width);
    m_height = std::move(other.m_height);
    m_data = std::move(other.m_data);
    m_texture_id = std::move(other.m_texture_id);
    other.m_path.clear();
    other.m_width = 100;
    other.m_height = 100;
    other.m_data = 0;
}

IGL::Texture::Texture(const Texture &other)
{
    std::lock_guard<std::mutex> lock(other.m_mutex);
    m_path = other.m_path;
    m_width = other.m_width;
    m_height = other.m_height;
    m_data = other.m_data;
    m_texture_id = other.m_texture_id;
}

IGL::Texture &IGL::Texture::operator=(Texture &&other)
{
    std::lock(m_mutex, other.m_mutex);
    std::lock_guard<std::mutex> self_lock(m_mutex, std::adopt_lock);
    std::lock_guard<std::mutex> other_lock(other.m_mutex, std::adopt_lock);
    m_path = std::move(other.m_path);
    m_width = std::move(other.m_width);
    m_height = std::move(other.m_height);
    m_data = std::move(other.m_data);
    m_texture_id = std::move(other.m_texture_id);
    other.m_path.clear();
    other.m_width = 100;
    other.m_height = 100;
    other.m_data = 0;
    return *this;
}

IGL::Texture &IGL::Texture::operator=(const Texture &other)
{
    std::lock(m_mutex, other.m_mutex);
    std::lock_guard<std::mutex> self_lock(m_mutex, std::adopt_lock);
    std::lock_guard<std::mutex> other_lock(other.m_mutex, std::adopt_lock);
    m_path = other.m_path;
    m_width = other.m_width;
    m_height = other.m_height;
    m_data = other.m_data;
    m_texture_id = other.m_texture_id;
    return *this;
}

void IGL::Loader::StartWorkers()
{
    unsigned int nproc = std::thread::hardware_concurrency(); // may return 0
    spdlog::get("main")->info(fmt::format("nproc: {}", nproc));
    nproc = nproc <= 1 ? 1 : nproc - 1;
    // nproc = 1;
    for (int k = 0; k < nproc; k++)
    {
        spdlog::get("main")->info(fmt::format("spawning thread: {}", k));
        std::thread *t = new std::thread(&IGL::Loader::Work, this, k);
        m_workers.push_back(t);
    }
}

void IGL::Loader::StopWorkers()
{
    m_keep_working = false;
    for (const auto &w : m_workers)
    {
        spdlog::get("main")->info(fmt::format("waiting on worker..."));
        w->join();
    }
    spdlog::get("main")->info(fmt::format("done waiting for workers!"));
}

void IGL::Loader::Work(int id)
{
    std::vector<Texture>::iterator it;
    while (m_keep_working)
    {
        spdlog::get("main")->info(fmt::format(
            "\nworker: [{}] ==========================================", id));

        spdlog::get("main")->info(fmt::format(
            "worker: [{}] pivot: {}", id, m_pivot));

        // try to load pivot first, this improves user experience
        if (m_pivot >= 0)
        {
            std::unique_lock<std::mutex> lock(
                m_textures[m_pivot].m_mutex, std::try_to_lock);
            if (!lock.owns_lock())
            {
                spdlog::get("main")->info(fmt::format(
                    "worker: [{}] unable to lock pivot texture", id));
            }
            else if (!m_textures[m_pivot].m_data && !m_textures[m_pivot].m_texture_id)
            {
                spdlog::get("main")->info(fmt::format(
                    "worker: [{}] load pivot image to sys memory...", id));
                int width = 0, height = 0;
                unsigned char *image_data = LoadImageToMem(
                    m_textures[m_pivot].m_path, &width, &height);
                m_textures[m_pivot].m_data = image_data;
                m_textures[m_pivot].m_width = width;
                m_textures[m_pivot].m_height = height;
                spdlog::get("main")->info(fmt::format(
                    "loading pivot image {} returned with '{}'",
                    m_textures[m_pivot].m_path.string(),
                    image_data != 0 ? "success" : "failure"));
            }
        }

        for (auto it = std::begin(m_textures); it != std::end(m_textures); ++it)
        {
            spdlog::get("main")->info(fmt::format(
                "worker: [{}] on index: {}", id, it - m_textures.begin()));
            std::unique_lock<std::mutex> lock(it->m_mutex, std::try_to_lock);
            // std::scoped_lock<std::mutex> lock(it->m_mutex, std::try_to_lock;
            if (!lock.owns_lock())
            {
                spdlog::get("main")->info(fmt::format(
                    "worker: [{}] unable to lock texture", id));
                continue; // unable to lock the mutex
            }

            // try to deallocate memory if this is a non buffered texture
            if (m_buf.find(it - m_textures.begin()) == m_buf.end())
            {
                // this is an unbuffered element, let's deallocate the image
                if (it->m_data)
                {
                    spdlog::get("main")->info(fmt::format(
                        "worker: [{}] deallocating sys memory...", id));
                    stbi_image_free(it->m_data);
                    it->m_data = 0;
                }
                else
                {
                    spdlog::get("main")->info(fmt::format(
                        "worker: [{}] this texture has already been "
                        "deallocated, nothing to do here...",
                        id));
                }
            }
            else if (!it->m_data && !it->m_texture_id) // load image
            {
                spdlog::get("main")->info(fmt::format(
                    "worker: [{}] load image to sys memory...", id));
                int width = 0, height = 0;
                unsigned char *image_data = LoadImageToMem(
                    it->m_path, &width, &height);
                it->m_data = image_data;
                it->m_width = width;
                it->m_height = height;
                spdlog::get("main")->info(fmt::format(
                    "loading image {} returned with '{}'",
                    it->m_path.string(),
                    image_data != 0 ? "success" : "failure"));
            }
            else
            {
                spdlog::get("main")->info(fmt::format(
                    "worker: [{}] there's nothing to do regarding this texture",
                    id));
            }
        }
        spdlog::get("main")->info(fmt::format(
            "worker: [{}] going to sleep... zzZZzzz...", id));
        std::this_thread::sleep_for(
            std::chrono::milliseconds(m_thread_sleep_ms));
    }
    spdlog::get("main")->info(fmt::format("worker: [{}] retiring...", id));
}

void IGL::Loader::RefreshFilesList()
{
    RefreshFilesList(m_cwd);
}

void IGL::Loader::RefreshFilesList(std::filesystem::path dirpath)
{
    m_cwd = dirpath;
    for (auto &tex : m_textures)
        tex.m_mutex.lock();

    for (auto &tex : m_textures)
    {
        if (tex.m_data)
        {
            stbi_image_free(tex.m_data);
            tex.m_data = 0;
        }
        if (tex.m_texture_id)
        {
            glDeleteTextures(1, &tex.m_texture_id);
            tex.m_texture_id = 0;
        }
    }
    m_textures.clear();
    for (auto &p : std::filesystem::directory_iterator(dirpath))
    {
        std::string ext = p.path().extension().string();
        // extension string to lower case
        std::transform(ext.begin(), ext.end(), ext.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        auto match = std::find(
            std::begin(m_supported_ext), std::end(m_supported_ext), ext);
        if (std::filesystem::is_regular_file(p) &&
            match != std::end(m_supported_ext))
            m_textures.push_back(Texture(p));
    }
    std::sort(m_textures.begin(), m_textures.end());

    // reset pivot index to 0 if there are valid files in the working dir and
    // pivot index was not already set
    if (m_pivot < 0 && m_textures.size())
        m_pivot = 0;

    // set indices of images that can be buffered
    SetBufItems();

    for (auto &tex : m_textures)
        tex.m_mutex.unlock();
}

std::vector<std::filesystem::path> IGL::Loader::GetFilesPaths()
{
    std::vector<std::filesystem::path> files;
    for (auto &tex : m_textures)
    {
        files.push_back(tex.m_path);
    }
    return files;
}

std::vector<std::string> IGL::Loader::GetFilesState()
{
    std::vector<std::string> files_state(m_textures.size());
    for (auto it = std::begin(m_textures); it != std::end(m_textures); ++it)
    {
        std::string state = "   ";
        if (m_buf.find(it - m_textures.begin()) != m_buf.end())
        {
            state[0] = '-';
        }
        if (it->m_data)
        {
            state[0] = ' ';
            state[1] = '-';
        }
        if (it->m_texture_id)
        {
            state[0] = ' ';
            state[1] = ' ';
            state[2] = '>';
        }
        files_state[it - m_textures.begin()] = state;
    }
    return files_state;
}

void IGL::Loader::SetBufItems()
{
    m_buf.clear();
    if (m_pivot < 0)
        return;
    for (int i = m_pivot - m_max_buf_size; i <= m_pivot + m_max_buf_size; ++i)
    {
        int size = m_textures.size();
        int valid_index = (i % size + size) % size;
        m_buf.insert(valid_index);
    }
}

void IGL::Loader::SetPivot(int index)
{
    int size = m_textures.size();
    if (size)
    {
        m_pivot = (index % size + size) % size;
    }
    SetBufItems();
}

void IGL::Loader::SetPivot(std::filesystem::path filename)
{
    auto it = std::find_if(
        m_textures.begin(), m_textures.end(),
        [filename](Texture val) {
            return val.m_path.string() == filename.string() ? true : false;
        });
    if (it != m_textures.end())
    {
        m_pivot = it - m_textures.begin();
        SetBufItems();
    }
}

unsigned char *IGL::Loader::LoadImageToMem(
    std::filesystem::path filename, int *out_width, int *out_height)
{
    unsigned char *image_data = stbi_load(
        filename.c_str(), out_width, out_height, 0, 4);
    return image_data;
}

void IGL::Loader::LoadImageToGPU()
{
    if (m_pivot >= 0)
    {
        std::unique_lock<std::mutex> lock(
            m_textures[m_pivot].m_mutex, std::try_to_lock);
        if (lock.owns_lock())
        {
            if (m_textures[m_pivot].m_data && !m_textures[m_pivot].m_texture_id)
            {
                spdlog::get("main")->info(fmt::format(
                    "loading pivot image to GPU: {}",
                    m_textures[m_pivot].m_path.string()));
                LoadImageToGPU(m_pivot);
                spdlog::get("main")->info(fmt::format(
                    "deallocating sys memory"));
                stbi_image_free(m_textures[m_pivot].m_data);
                m_textures[m_pivot].m_data = 0;
            }
        }
    }
    for (auto &index : m_buf)
    {
        std::unique_lock<std::mutex> lock(
            m_textures[index].m_mutex, std::try_to_lock);
        if (!lock.owns_lock())
            continue; // unable to lock the mutex, try again later...
        if (m_textures[index].m_data && !m_textures[index].m_texture_id)
        {
            spdlog::get("main")->info(fmt::format(
                "loading image to GPU: {}", m_textures[index].m_path.string()));
            LoadImageToGPU(index);
            spdlog::get("main")->info(fmt::format("deallocating sys memory"));
            stbi_image_free(m_textures[index].m_data);
            m_textures[index].m_data = 0;
            break; // do not load all images at once
        }
    }
}

void IGL::Loader::UnloadImageFromGPU()
{
    // for (auto &tex : m_textures)
    for (auto it = std::begin(m_textures); it != std::end(m_textures); ++it)
    {
        std::unique_lock<std::mutex> lock(it->m_mutex, std::try_to_lock);
        if (!lock.owns_lock())
            continue; // unable to lock the mutex, try again later...
        if (m_buf.find(it - m_textures.begin()) != m_buf.end())
            continue; // this is a buffered texture, leave it and try the next one
        if (it->m_texture_id)
        {
            spdlog::get("main")->info(fmt::format(
                "unloading image from GPU: {}", it->m_path.string()));
            glDeleteTextures(1, &it->m_texture_id);
            it->m_texture_id = 0;
            break; // do not unload all images at once
        }
    }
}

void IGL::Loader::LoadImageToGPU(int tex_id)
{
    // Create a OpenGL texture identifier
    glGenTextures(1, &m_textures[tex_id].m_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_textures[tex_id].m_texture_id);

    // Setup filtering parameters for display
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 m_textures[tex_id].m_width, m_textures[tex_id].m_height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, m_textures[tex_id].m_data);
    // m_textures[tex_id].m_mem_status = TextureStatus::InGPU;
}
