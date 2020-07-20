#pragma once

#include <string>
#include <utility>
#include <queue>
#include <mutex>
#include <boost/algorithm/string/join.hpp>
#include "CommandTable.hpp"

namespace gdb_stub {

using PacketData = std::string;
using Checksum = uint8_t;

Checksum calculateChecksum(const PacketData &str);
char ackOk();
char ackFail();

struct Packet {
    Packet(PacketData data, Checksum checksum) : data(std::move(data)), checksum(checksum) {}
    PacketData data;
    Checksum checksum;
    bool isValid() const {
        return checksum == calculateChecksum(data);
    }
    std::string toStr() const {
        return static_cast<std::ostringstream&>(std::ostringstream()
                << "$" << data << "#" << str::toHexStr({checksum})).str();
    }
};

const auto INTERRUPT_PACKET = Packet(cmd::INTERRUPT_NAME, 0);
const auto TRAP_PACKET = Packet(cmd::TRAP_NAME, 0);
const auto EMPTY_PACKET = Packet("", 0);

const std::map<std::string, Packet> signalPackets = {
        { cmd::INTERRUPT_NAME, INTERRUPT_PACKET },
        { cmd::TRAP_NAME, TRAP_PACKET },
};

inline bool operator==(const Packet &left, const Packet &right) {
    return (left.data == right.data) && (left.checksum == right.checksum);
}

inline bool operator!=(const Packet &left, const Packet &right) {
    return (left.data != right.data) || (left.checksum != right.checksum);
}

enum class Signal {
    INT,
    TRAP,
};

class Stub {
public:
    static std::map<uint8_t, Signal> signalMap;

    Stub(Target &target, std::vector<cmd::Command_ptr> commands,
         std::istream &inputStream, std::ostream &outputStream);
    void run();
    void send(const Packet &packet);
    Packet receive();
    Packet process(const Packet &packet);
    void sendAckOk();
    void sendAckFail();
    Packet waitForSignal();
    bool isDead();

private:
    static bool isPacketStart(char data);
    static bool isPacketEnd(char data);
    static bool isSignal(char data);
    static bool isSignal(const Packet &packet);
    static Packet signalToPacket(Signal sig);

    uint8_t readChecksum();
    Packet readPacket();
    char readChar();

    Target &target;
    CommandTable commandTable;

    bool isKilled;

    std::mutex queueMtx;
    std::queue<Packet> packetQueue;

    std::istream &in;
    std::ostream &out;
};

}