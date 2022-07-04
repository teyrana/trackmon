#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace core {

enum BinaryBufferEnum {
    BUFFER_BINARY_AIS=2,
    BUFFER_BINARY_MOOS=3,
    BUFFER_BINARY_PCAP=4,
    BUFFER_BINARY_UNKNOWN=0
};

enum TextBufferEnum {
    BUFFER_TEXT_AIS=1,
    BUFFER_TEXT_NMEA=5,
    BUFFER_TEXT_MOOS=7,
    BUFFER_TEXT_UNKNOWN=0
};

class ForwardBuffer {
public:
    uint8_t * buffer = nullptr;
    size_t length = 0;
    uint64_t timestamp = 0;
    BinaryBufferEnum type = BUFFER_BINARY_UNKNOWN;

public:
    inline void reset(){ 
        timestamp = 0;
        length = 0;
        buffer = nullptr;
    }
};

class StringBuffer {
public:
    std::string str;
    uint64_t timestamp = 0;
    TextBufferEnum type = BUFFER_TEXT_UNKNOWN;
};

}  // namespace core
