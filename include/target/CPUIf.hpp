#pragma once

#include "target/Types.hpp"

namespace gdb_stub {

enum class CPUStatus {
    STOPPED,
    RUNNING,
    UNDEFINED,
};

class CPUIf {
public:
    virtual CPUStatus getStatus() const = 0;
    virtual void start() const = 0;
    virtual void stop() const = 0;
    virtual void stopIfNot() const = 0;
    virtual void setBreakpoint(Address addr) const = 0;
    virtual void removeAllBreakpoints() const = 0;
    virtual std::vector<Register> readGPR() const = 0;
    virtual ~CPUIf() = default;
};

}
