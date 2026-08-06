#include "file_streamer.h"

namespace film_server {
    FileStreamer::FileStreamer(const std::string& path)
    : m_file(path, std::ios::binary)
    {
    }

    bool FileStreamer::isValid() const
    {
        return m_file.is_open();
    }

    std::size_t FileStreamer::readChunk(char* buffer, std::size_t size)
    {
        m_file.read(buffer, static_cast<std::streamsize>(size));
        return static_cast<std::size_t>(m_file.gcount());
    }
}