#pragma once

#include <memory>
#include <map>
#include <utility>
#include "target/State.hpp"

namespace gdb_stub {

using Register = std::vector<uint8_t>;
using Address = uint32_t;

class MemoryIf {
public:
    virtual std::vector<uint8_t> memoryRead(uint32_t addr, size_t len) const = 0;
    virtual void memoryWrite(const std::vector<uint8_t> &data) const = 0;
    virtual std::vector<Register> readGPR() const = 0;
    virtual void setBreakpoint(Address addr) const = 0;
    virtual void removeAllBreakpoints() const = 0;
    virtual ~MemoryIf() = default;
};

}
