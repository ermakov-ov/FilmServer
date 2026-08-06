#ifndef HORSELINE_FILE_STREAMER_H
#define HORSELINE_FILE_STREAMER_H

#include <fstream>
#include <string>
#include <cstddef>

namespace film_server {

    class FileStreamer {
    public:
        explicit FileStreamer(const std::string& path) ;

        bool isValid() const ;
        std::size_t readChunk(char* buffer, std::size_t size) ;

    private:
        std::ifstream m_file;
    };
}

#endif //HORSELINE_FILE_STREAMER_H
