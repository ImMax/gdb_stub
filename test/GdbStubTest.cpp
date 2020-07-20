#include <string>
#include <future>
#include <gtest/gtest.h>
#include "TargetMock.hpp"
#include "GdbStub.hpp"
#include "StrUtils.hpp"
#include "command/DefaultCommandsList.hpp"

namespace gdb_stub {

using ::testing::Return;



class GdbStubTest : public ::testing::Test {
public:
    void SetUp() {
        dummyTarget.status = TargetStatus::STOPPED;
    }
    void TearDown() {}

    std::stringstream in;
    std::stringstream out;

    const std::string TEST_NAME = "TEST";

    Target dummyTarget {
            Target::Config{{"test", TEST_NAME}},
            memMock,
            cpuMock
    };
    Stub stub {
            dummyTarget,
            DEFAULT_COMMANDS_LIST,
            in, out
    };
    MemoryMock memMock;
    CPUMock cpuMock;
};

TEST_F(GdbStubTest, ReadMemoryTest) {
    std::vector<uint8_t> bytes { 0xBE, 0xDA };
    EXPECT_CALL(cpuMock, getStatus()).WillRepeatedly(Return(CPUStatus::STOPPED));
    EXPECT_CALL(memMock, memoryRead(0x123, 10)).WillRepeatedly(Return(bytes));

    const auto INPUT_PACKET = Packet("m123,10", 0x90);

    auto OUTPUT_PACKET = stub.process(INPUT_PACKET);

    const auto EXPECTED_OUTPUT_PACKET = Packet{"beda", 0x8c};

    EXPECT_EQ(bytes.size(), size_t{2});
    EXPECT_EQ(OUTPUT_PACKET.toStr(), EXPECTED_OUTPUT_PACKET.toStr());
}

TEST_F(GdbStubTest, Test_H_packet) {
    EXPECT_CALL(cpuMock, getStatus()).WillRepeatedly(Return(CPUStatus::STOPPED));

    const auto INPUT_PACKET = Packet("Hg0", 0xdf);

    auto OUTPUT_PACKET = stub.process(INPUT_PACKET);

    const auto EXPECTED_OUTPUT_PACKET = Packet{"E01", 0xa6};

    EXPECT_EQ(OUTPUT_PACKET.toStr(), EXPECTED_OUTPUT_PACKET.toStr());
}

TEST_F(GdbStubTest, TestCheckSum) {
    auto data = PacketData("beda");
    auto checksum = calculateChecksum(data);
    EXPECT_EQ(checksum, 0x8c);
}

TEST_F(GdbStubTest, TestStringUtils) {
    std::vector<uint8_t> bytes { 0xBE, 0xDA };
    auto str = str::toHexStr(bytes);

    EXPECT_EQ("beda", str);
    EXPECT_EQ(0xBE, str::hexToInt<uint8_t>("be"));
    EXPECT_EQ("0fff", str::intToHex<uint16_t>(0xfff));
}

TEST_F(GdbStubTest, Test_Break) {
    auto OUTPUT_PACKET = stub.process(TRAP_PACKET);

    const auto EXPECTED_OUTPUT_PACKET = Packet{"S05", 0xb8};

    EXPECT_EQ(OUTPUT_PACKET.toStr(), EXPECTED_OUTPUT_PACKET.toStr());
}

TEST_F(GdbStubTest, Test_break_while_stopped) {
    dummyTarget.status = TargetStatus::STOPPED;

    auto OUTPUT_PACKET = stub.process(TRAP_PACKET);

    const auto EXPECTED_OUTPUT_PACKET = Packet{"S05", 0xb8};

    EXPECT_EQ(OUTPUT_PACKET.toStr(), EXPECTED_OUTPUT_PACKET.toStr());
}

}
