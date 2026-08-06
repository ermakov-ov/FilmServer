#ifndef JSONPARSER_DIRECTOR_H
#define JSONPARSER_DIRECTOR_H

#include <string>

class Director {
public:
    explicit Director(int id, std::string name)
        : m_id(id), m_name(std::move(name)) {}

    int id() const noexcept { return m_id; }
    const std::string& name() const noexcept { return m_name; }
private:
    int m_id;
    std::string m_name;
};

#endif //JSONPARSER_DIRECTOR_H
