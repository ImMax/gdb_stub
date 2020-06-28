#pragma once

#include <memory>
#include <map>
#include <utility>

namespace gdb_stub {

class MemoryIf {
public:
    virtual std::vector<uint8_t> memoryRead(uint32_t addr, size_t len) const = 0;
    virtual void memoryWrite(const std::vector<uint8_t> &data) const = 0;
    virtual ~MemoryIf() = default;
};

}
