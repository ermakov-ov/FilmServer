#ifndef HORSELINE_VERSION_H
#define HORSELINE_VERSION_H

#include <string>

namespace server_version {

    constexpr inline std::string_view name_app = "film-server";
    constexpr inline int kServerMajor = 2;
    constexpr inline int kServerMinor = 1;
    constexpr inline int kServerPatch = 0;

    inline std::string makeServerVersionString()
    {
        return std::to_string(kServerMajor) + "." +
               std::to_string(kServerMinor) + "." +
               std::to_string(kServerPatch);
    }
}

#endif // HORSELINE_VERSION_H
