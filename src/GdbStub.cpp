#include <iostream>
#include <vector>
#include <numeric>
#include <future>
#include <chrono>
#include <boost/algorithm/string/join.hpp>
#include "GdbStub.hpp"
#include "StrUtils.hpp"

namespace gdb_stub {

std::map<uint8_t, Signal> Stub::signalMap = {
        { '\2', Signal::INT },
        { '\3', Signal::INT },
        { '\5', Signal::TRAP },
};

Stub::Stub(Target &target, std::vector<cmd::Command_ptr> commands,
           std::istream &inputStream, std::ostream &outputStream) :
        target(target), commandTable(std::move(commands)), in(inputStream), out(outputStream) {}

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
    try {
        return signalMap.find(data) != signalMap.end();
    } catch (const std::out_of_range &e) {
        return false;
    }
}

bool Stub::isSignal(const Packet &packet) {
    return packet == INTERRUPT_PACKET || packet == TRAP_PACKET;
}

Packet Stub::readPacket() {
    std::stringstream buffer;
    while(true) {
        char data;
        in >> data;
        if (isPacketEnd(data)) {
            const uint8_t checksum = readChecksum();
            return {buffer.str(), checksum};
        } else {
            buffer << data;
        }
    }
}

Packet Stub::signalToPacket(Signal sig) {
    switch (sig) {
        case Signal::INT: return INTERRUPT_PACKET;
        case Signal::TRAP: return TRAP_PACKET;
        default: return EMPTY_PACKET;
    }
}

void Stub::run() {
    auto future = std::async(std::launch::async, [&]() {
        while (!isDead()) {
            char firstChar;
            in >> firstChar;
            std::cout << "read = " << firstChar << "\n";
            Packet packet = EMPTY_PACKET;
            if (isPacketStart(firstChar)) {
                packet = readPacket();
            }
            if (isSignal(firstChar)) {
                packet = signalToPacket(signalMap[firstChar]);
            }
            if (packet != EMPTY_PACKET) {
                std::lock_guard<std::mutex> lock{queueMtx};
                packetQueue.push(packet);
            }
        }
    });

    while (!isDead()) {
        const auto rxPacket = receive();
        if (rxPacket.isValid()) {
            sendAckOk();
            const auto txPacket = process(rxPacket);
            send(txPacket);
        } else {
            sendAckFail();
        }
    }
    while (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        sendAckOk();
    }
}

Packet Stub::receive() {
    while (true) {
        queueMtx.lock();
        if (!packetQueue.empty()) {
            const auto packet = Packet{packetQueue.front().data, packetQueue.front().checksum};
            packetQueue.pop();
            queueMtx.unlock();
            return packet;
        }
        queueMtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds{200});
    }
}

void Stub::send(const Packet &packet) {
    out << packet.toStr();
    out.flush();
}

Packet Stub::process(const Packet &packet) {
    const std::vector<std::string> commands = str::split(packet.data, ';');

    std::vector<std::string> results{};
    for (auto &command : commands) {
        const auto result = commandTable.exec(target, command);

        if (result.status == cmd::Status::CONTINUE) {
            const auto signalPacket = waitForSignal();
            return signalPacket;
        }

        if (result.status != cmd::Status::EMPTY)
            results.push_back(result.val);
    }

    auto payload = boost::algorithm::join(results, ";");

    return {payload, calculateChecksum(payload)};
}

Packet Stub::waitForSignal() {
    while (true) {
        queueMtx.lock();
        if (!packetQueue.empty()) {
            const auto packet = Packet{packetQueue.front().data, packetQueue.front().checksum};

            if (isSignal(packet)) {
                packetQueue.pop();
                queueMtx.unlock();
                return process(packet);
            }
        }
        queueMtx.unlock();
        if (target.cpu.getStatus() == CPUStatus::STOPPED) {
            return process(TRAP_PACKET);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{200});
    }
}

void Stub::sendAckOk() {
    out << ackOk();
    out.flush();
}

void Stub::sendAckFail() {
    out << ackFail();
    out.flush();
}

bool Stub::isDead() { return target.status == TargetStatus::KILLED; }

Checksum calculateChecksum(const PacketData &str) {
    return static_cast<uint32_t>(std::accumulate(std::begin(str), std::end(str), 0)) % 256;
}

char ackOk() { return '+'; }
char ackFail() { return '-'; }

}