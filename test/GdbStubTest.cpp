#include <string>
#include <gtest/gtest.h>
#include "TargetMock.hpp"
#include "GdbStub.hpp"
#include "StrUtils.hpp"
#include "command/DefaultCommandsList.hpp"

namespace gdb_stub {

using ::testing::Return;

class GdbStubTest : public ::testing::Test {
public:
    void SetUp() {}
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
    EXPECT_CALL(memMock, memoryRead(0x123, 10)).WillRepeatedly(Return(bytes));

    const auto INPUT_PACKET = Packet("m123,10", 0x90);
    in << INPUT_PACKET.toStr();

    auto packet = stub.request();
    stub.response(packet);

    std::string output;
    out >> output;

    std::string EXPECTED_OUTPUT = "+$beda#8c";

    EXPECT_EQ(packet.toStr(), INPUT_PACKET.toStr());
    EXPECT_EQ(bytes.size(), size_t{2});
    EXPECT_EQ(output, EXPECTED_OUTPUT);
}

TEST_F(GdbStubTest, Test_H_packet) {
    const auto INPUT_PACKET = Packet("Hg0", 0xdf);
    in << INPUT_PACKET.toStr();

    auto packet = stub.request();
    stub.response(packet);

    std::string output;
    out >> output;

    std::string EXPECTED_OUTPUT = "+$E01#a6";

    EXPECT_EQ(packet.toStr(), INPUT_PACKET.toStr());
    EXPECT_EQ(output, EXPECTED_OUTPUT);
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
    EXPECT_CALL(cpuMock, stop());

    in << (char)(0x02);

    auto packet = stub.request();
    stub.response(packet);

    std::string output;
    out >> output;

    EXPECT_EQ(output, "+$S02#b5");
}

TEST_F(GdbStubTest, Test_break_while_stopped) {
    EXPECT_CALL(cpuMock, stop());

    dummyTarget.status = TargetStatus::STOPPED;

    in << (char)(0x02);

    auto packet = stub.request();
    stub.response(packet);

    std::string output;
    out >> output;

    EXPECT_EQ(output, "+$S02#b5");
}

}