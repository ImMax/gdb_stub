#include <iostream>
#include <vector>
#include <numeric>
#include <boost/algorithm/string/join.hpp>
#include "GdbStub.hpp"
#include "StrUtils.hpp"

namespace gdb_stub {

Stub::Stub(Target &target, std::vector<std::shared_ptr<cmd::Command>> commands,
           std::istream &inputStream, std::ostream &outputStream) :
        commandTable(target, std::move(commands)), in(inputStream), out(outputStream) {}

void Stub::response(const Packet &request) {
    if (request.checksum == calculateChecksum(request.data)) {
        std::vector<std::string> commands = str::split(request.data, ';');

        std::vector<std::string> results{};
        for (auto &command : commands) {
            auto result = commandTable.exec(command);
            if (!result.empty())
                results.push_back(result);
        }

        auto payload = boost::algorithm::join(results, ";");
        out << ackOk() << Packet(payload, calculateChecksum(payload)).toStr();
    } else {
        out << ackFail();
    }
}

uint8_t Stub::readChecksum() {
    char checksumOne;
    char checksumTwo;
    in >> checksumOne;
    in >> checksumTwo;
    std::ostringstream buffer;
    buffer << checksumOne << checksumTwo;
    return str::hexToInt<uint8_t>(buffer.str());
}

Packet Stub::request() {
    std::stringstream buffer;
    uint8_t checksum = 0;
    bool isReading = false;
    bool isPacketReceived = false;
    while(!isPacketReceived) {
        char gdbChar;
        in >> gdbChar;
        switch(gdbChar) {
            case '$': {
                isReading = true;
            } break;
            case '#': {
                checksum = readChecksum();
                isReading = false;
                isPacketReceived = true;
            } break;
            default: {
                if (isReading) {
                    buffer << gdbChar;
                }
            } break;
        }
    }
    return Packet(buffer.str(), checksum);
}

bool Stub::isDead() { return commandTable.isKillPacketReceived(); }

Checksum calculateChecksum(const PacketData &str) {
    return static_cast<uint32_t>(std::accumulate(std::begin(str), std::end(str), 0)) % 256;
}

char ackOk() { return '+'; }
char ackFail() { return '-'; }

}
