#pragma once

#include <string>
#include <utility>
#include <boost/algorithm/string/join.hpp>
#include "CommandTable.hpp"

namespace gdb_stub {

using PacketData = std::string;
using Checksum = uint8_t;

struct Packet {
    Packet(PacketData data, Checksum checksum) : data(std::move(data)), checksum(checksum) {}
    PacketData data;
    Checksum checksum;
    std::string toStr() const {
        return static_cast<std::ostringstream&>(std::ostringstream()
                << "$" << data << "#" << str::toHexStr({checksum})).str();
    }
};

inline bool operator==(const Packet &left, const Packet &right) {
    return (left.data == right.data) && (left.checksum == right.checksum);
}

class Stub {
public:
    Stub(Target &target, std::vector<std::shared_ptr<cmd::Command>> commands,
         std::istream &inputStream, std::ostream &outputStream);
    void response(const Packet &request);
    Packet request();
    bool isDead();

private:
    uint8_t readChecksum();

    CommandTable commandTable;

    std::istream &in;
    std::ostream &out;
};

Checksum calculateChecksum(const PacketData &str);
char ackOk();
char ackFail();

}