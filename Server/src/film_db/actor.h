//
// Created by eow on 07/07/2026.
//

#ifndef JSONPARSER_ACTOR_H
#define JSONPARSER_ACTOR_H
#include <string>

class Actor {
private:
    int id_;
    std::string name_;

public:
    explicit Actor(int id, std::string name)
        : id_(id), name_(std::move(name)) {}

    int id() const noexcept { return id_; }
    const std::string& name() const noexcept { return name_; }
};
#endif //JSONPARSER_ACTOR_H
