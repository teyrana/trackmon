#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace core {

class ForwardBuffer {
public:
    uint64_t timestamp;
    size_t length;
    uint8_t * buffer;

public:
    inline void reset(){ 
        timestamp = 0;
        length = 0;
        buffer = nullptr;
    }
};

class StringBuffer {
public:
    uint64_t timestamp;
    std::string str;
};

}  // namespace core
