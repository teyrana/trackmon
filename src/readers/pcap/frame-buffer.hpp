#pragma once

namespace readers {
namespace pcap {

class FrameBuffer {
public:

    uint64_t timestamp;

    size_t length;

    uint8_t * buffer;

};

}
}
