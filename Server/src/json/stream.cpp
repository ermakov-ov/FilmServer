#include "stream.h"

#include <cstring>

namespace parser
{

    StreamError::StreamError(const std::string& msg)
    : std::runtime_error(msg)
    {
    }
    StreamBuffer::StreamBuffer(const std::string& data_str)
    : m_data(nullptr)
    , m_length(0)
    , m_offset(0)
    {
        m_length = data_str.size();
        m_data = std::make_unique<char[]>(data_str.size() + 1);

        memcpy(m_data.get(), data_str.c_str(), data_str.size());

        m_data[data_str.size()] = '\0' ;
    }
    StreamBuffer::StreamBuffer(DataPtr data, std::size_t size)
    : m_data(std::move(data))
    , m_length(size)
    , m_offset(0)
    {
    }

    char StreamBuffer::peek() const
    {
        if (isEof())
        {
            throw StreamError("End of stream");
        }
        return m_data[m_offset];
    }

    char StreamBuffer::next()
    {
        if (isEof())
        {
            throw StreamError("Try to advance beyond end of stream");
        }
        ++m_offset;
        return m_data[m_offset];
    }

    char StreamBuffer::prev()
    {
        if (m_length <= 0)
        {
            throw StreamError("Buffer is empty");
        }
        if ( m_offset == 0)
        {
            throw StreamError("Trying to go outside the buffer");
        }

        return m_data[--m_offset] ;
    }

    bool StreamBuffer::isEof() const
    {
        return m_offset + 1 > m_length;
    }

    std::size_t StreamBuffer::position() const
    {
        return m_offset;
    }

}
