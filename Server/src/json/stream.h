//
// Created by eow on 03/07/2026.
//

#ifndef SERVER_STREAM_H
#define SERVER_STREAM_H

#include <memory>
#include <stdexcept>
#include <cstddef>
#include <string>

namespace parser
{
    class StreamError : public std::runtime_error
    {
    public:
        explicit StreamError(const std::string& msg);
    };

    class StreamBuffer
    {
    public:
        using DataPtr = std::unique_ptr<char[]>;

        explicit StreamBuffer(DataPtr data, std::size_t size);
        explicit StreamBuffer(const std::string& data);

        char peek() const;
        char next();
        char prev();

        bool isEof() const ;
        std::size_t position() const;

    private:
        DataPtr     m_data;

        std::size_t m_length;
        std::size_t m_offset;
    };
} // namespace parser

#endif //SERVER_STREAM_H
