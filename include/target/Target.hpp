#pragma once

#include <memory>
#include <map>
#include <utility>
#include "target/MemoryIf.hpp"
#include "target/CPUIf.hpp"

namespace gdb_stub {

enum class TargetStatus {
    RUNNING,
    STOPPED,
    BREAK,
    KILLED,
};

class Target {
public:
    using Config = std::map<std::string, std::string>;

    Target(const Config &config, const MemoryIf &memory, const CPUIf &cpu) :
            config(config), memory(memory), cpu(cpu) {
        status = TargetStatus::STOPPED;
    }

    Config config;
    TargetStatus status;
    const MemoryIf &memory;
    const CPUIf &cpu;
};

}