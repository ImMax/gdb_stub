#pragma once

#include <cinttypes>
#include <vector>

namespace gdb_stub {

using Register = std::vector<uint8_t>;
using Address = uint32_t;

}