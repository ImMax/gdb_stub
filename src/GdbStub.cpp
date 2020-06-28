#include <iostream>
#include <vector>
#include <numeric>
#include <boost/algorithm/string/join.hpp>
#include "GdbStub.hpp"
#include "StrUtils.hpp"

namespace gdb_stub {

std::map<uint8_t, Signal> Stub::signalMap = {
        { 0x02, Signal::INT },
        { 0x03, Signal::INT },
};

Stub::Stub(Target &target, std::vector<cmd::Command_ptr> commands,
           std::istream &inputStream, std::ostream &outputStream) :
        target(target), commandTable(std::move(commands)), in(inputStream), out(outputStream) {}

void Stub::response(const Packet &request) {
    if (isSignal(request)) {
        const auto payload = commandTable.exec(target, request.data).val;
        out << ackOk() << Packet(payload, calculateChecksum(payload)).toStr();
    } else  {
        if (request.isValid()) {
            std::vector<std::string> commands = str::split(request.data, ';');

            std::vector<std::string> results{};
            for (auto &command : commands) {
                const auto result = commandTable.exec(target, command);

                if (result.status == cmd::Status::CONTINUE) {
                    return;
                }

                if (result.status != cmd::Status::EMPTY)
                    results.push_back(result.val);
            }

            auto payload = boost::algorithm::join(results, ";");
            out << ackOk() << Packet(payload, calculateChecksum(payload)).toStr();
        } else {
            out << ackFail();
        }
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

bool Stub::isPacketStart(char data) {
    return data == '$';
}

bool Stub::isPacketEnd(char data) {
    return data == '#';
}

bool Stub::isSignal(char data) {
    return data == 0x02;
}

bool Stub::isSignal(const Packet &packet) {
    return packet == INTERRUPT_PACKET;
}

Packet Stub::readPacket() {
    std::stringstream buffer;
    while(true) {
        char data;
        in >> data;
        if (isPacketEnd(data)) {
            const uint8_t checksum = readChecksum();
            return Packet(buffer.str(), checksum);
        } else {
            buffer << data;
        }
    }
}

Packet Stub::signalToPacket(Signal sig) {
    switch (sig) {
        case Signal::INT: return INTERRUPT_PACKET;
        default: return EMPTY_PACKET;
    }
}

Packet Stub::request() {
    while (true) {
        char data;
        in >> data;
        if (isPacketStart(data)) {
            return readPacket();
        }
        if (isSignal(data)) {
            return signalToPacket(signalMap[data]);
        }
    }
}

bool Stub::isDead() { return target.status == TargetStatus::KILLED; }

Checksum calculateChecksum(const PacketData &str) {
    return static_cast<uint32_t>(std::accumulate(std::begin(str), std::end(str), 0)) % 256;
}

char ackOk() { return '+'; }
char ackFail() { return '-'; }

}