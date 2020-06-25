#pragma once

#include <memory>
#include <map>
#include <utility>
#include "target/State.hpp"
#include "target/MemoryIf.hpp"
#include "target/CPUIf.hpp"

namespace gdb_stub {

class Target {
public:
    using Config = std::map<std::string, std::string>;

    Target(const Config &config, State state, const MemoryIf &memory, const CPUIf &cpu) :
            config(config), memory(memory), cpu(cpu) {
        this->state = std::move(state);
    }

    Config config;
    State state;
    const MemoryIf &memory;
    const CPUIf &cpu;
};

}