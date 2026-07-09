//
// Created by eow on 07/07/2026.
//

#ifndef JSONPARSER_DIRECTOR_H
#define JSONPARSER_DIRECTOR_H
#include <string>

class Director {
private:
    int id_;
    std::string name_;

public:
    explicit Director(int id, std::string name)
        : id_(id), name_(std::move(name)) {}

    int id() const noexcept { return id_; }
    const std::string& name() const noexcept { return name_; }
};

#endif //JSONPARSER_DIRECTOR_H
