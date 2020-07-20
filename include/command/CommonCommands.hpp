#pragma once

#include <sstream>
#include "command/Command.hpp"
#include "target/Target.hpp"
#include "target/MemoryIf.hpp"

namespace gdb_stub {

namespace cmd {

class GetHaltReason : public Command {
public:
    std::string getName() const override { return "?"; }
    Result exec(Target &target, const std::string &args) override {
        target.cpu.stopIfNot();
        return {"S02"};
    }
};

class ReadGeneralRegisters : public Command {
public:
    std::string getName() const override { return "g"; }
    Result exec(Target &target, const std::string &args) override {
        auto registers = target.cpu.readGPR();
        if (registers.empty()) return {E01, Status::FAILED};
        std::ostringstream registersHex;
        for (const auto& r : registers) {
            registersHex << str::toHexStr(r);
        }
        return {registersHex.str()};
    }
};

class Continue : public Command {
public:
    std::string getName() const override { return "c"; }
    Result exec(Target &target, const std::string &args) override {
        target.cpu.start();
        target.status = TargetStatus::RUNNING;
        return CONTINUE_RESULT;
    }
};

class ReadRegister : public Command {
public:
    std::string getName() const override { return "p"; }
    Result exec(Target &target, const std::string &args) override { return {"10"}; }
};

enum class BreakpointType {
    SOFTWARE,
    HARDWARE,
    WRITE_WATCHPOINT,
    READ_WATCHPOINT,
    ACCESS_WATCHPOINT,
};

using BreakpointKind = std::string;

class BreakpointCmd {
public:
    BreakpointCmd(const std::string &cmd) {
        const auto cmdParts = str::split(cmd, ',');
        switch (cmdParts[0].at(1)) {
            case '0': type = BreakpointType::SOFTWARE; break;
            case '1': type = BreakpointType::HARDWARE; break;
            case '2': type = BreakpointType::WRITE_WATCHPOINT; break;
            case '3': type = BreakpointType::READ_WATCHPOINT; break;
            case '4': type = BreakpointType::ACCESS_WATCHPOINT; break;
        }
        address = str::hexToInt<Address>(cmdParts[1]);
        kind = cmdParts[2];
    }
    BreakpointType type;
    Address address;
    BreakpointKind kind;
};

class InsertBreakpoint : public Command {
public:
    std::string getName() const override { return "Z"; }
    Result exec(Target &target, const std::string &args) override {
        const auto bpCmd = BreakpointCmd(args);
        target.cpu.setBreakpoint(bpCmd.address);
        target.status = TargetStatus::BREAK;
        return {"OK"};
    }
};

class RemoveBreakpoint : public Command {
public:
    std::string getName() const override { return "z"; }
    Result exec(Target &target, const std::string &args) override {
        target.cpu.removeAllBreakpoints();
        return {"OK"};
    }
};

class ReadMemory : public Command {
public:
    std::string getName() const override { return "m"; }
    Result exec(Target &target, const std::string &args) override {
        auto sCmd = str::split(args, ',');
        std::string hexAddrStr = sCmd[0].substr(1, sCmd[0].size());
        std::string lenStr = sCmd[1];
        auto data = target.memory.memoryRead(str::hexToInt<uint32_t>(hexAddrStr), std::stoi(lenStr));
        if (data.empty()) return {E01, Status::FAILED};
        return {str::toHexStr(data)};
    }
};

class SetTreadForNextCmd : public Command {
public:
    std::string getName() const override { return "H"; }
    Result exec(Target &target, const std::string &args) override {
        const auto op = args[1];
        const auto threadId = args[2];
        if (threadId == '-')
            return {"OK"};
        else
            return {E01, Status::FAILED};
    }
};

class Interrupt : public Command {
public:
    std::string getName() const override { return cmd::INTERRUPT_NAME; }
    Result exec(Target &target, const std::string &args) override {
        target.cpu.stop();
        target.status = TargetStatus::STOPPED;
        return {"S02"};
    }
};

class Trap : public Command {
public:
    std::string getName() const override { return cmd::TRAP_NAME; }
    Result exec(Target &target, const std::string &args) override {
        return {"S05"};
    }
};

}

}