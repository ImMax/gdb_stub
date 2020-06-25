#pragma once

#include <gmock/gmock.h>
#include "target/MemoryIf.hpp"
#include "target/CPUIf.hpp"

namespace gdb_stub {

class MemoryMock : public MemoryIf {
public:
    MOCK_CONST_METHOD2(memoryRead, std::vector<uint8_t>(uint32_t addr, size_t len));
    MOCK_CONST_METHOD1(memoryWrite, void(const std::vector<uint8_t> &data));
    MOCK_CONST_METHOD0(readGPR, std::vector<Register>());
    MOCK_CONST_METHOD1(setBreakpoint, void(Address));
    MOCK_CONST_METHOD0(removeAllBreakpoints, void());
};

class CPUMock : public CPUIf {
public:
    MOCK_CONST_METHOD0(getStatus, CPUStatus());
    MOCK_CONST_METHOD0(start, void());
    MOCK_CONST_METHOD0(stop, void());
};

}