#pragma once

#include "target/State.hpp"

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
    virtual ~CPUIf() = default;
};

}
