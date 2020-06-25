#pragma once

#include <string>

namespace gdb_stub {

struct State {
    size_t packetSize;
    bool multiprocess;
    bool isKilled;
};

}
