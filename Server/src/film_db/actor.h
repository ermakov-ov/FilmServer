#ifndef JSONPARSER_ACTOR_H
#define JSONPARSER_ACTOR_H

#include <string>

class Actor {
public:
    explicit Actor(int id, std::string name)
        : m_id(id), m_name(std::move(name)) {}

    int id() const noexcept { return m_id; }
    const std::string& name() const noexcept { return m_name; }

private:
    int         m_id;
    std::string m_name;
};

#endif //JSONPARSER_ACTOR_H
