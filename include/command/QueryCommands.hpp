#pragma once

#include <sstream>
#include "command/Command.hpp"
#include "target/Target.hpp"
#include "StrUtils.hpp"

namespace gdb_stub {

namespace cmd {

class GetSupportedFeatures : public Command {
public:
    std::string getName() const override { return "qSupported"; }
    Result exec(Target &target, const std::string &args) override {
        std::stringstream ss;
        ss << "PacketSize=" << target.config["PacketSize"] << ";"
           << "vContSupported" << target.config["vContSupported"] << ";"
           << "swbreak" << target.config["swbreak"] << ";"
           << "multiprocess" << target.config["multiprocess"];

        return {ss.str()};
    }
};

class StartNoAckMode : public Command {
public:
    std::string getName() const override { return "QStartNoAckMode"; }
    Result exec(Target &target, const std::string &args) override { return {"OK"}; }
};

class GetFirstTracepointData : public Command {
public:
    std::string getName() const override { return "qTfP"; }
    Result exec(Target &target, const std::string &args) override { return {"l"}; }
};

class SymbolCmd : public Command {
public:
    std::string getName() const override { return "qSymbol"; }
    Result exec(Target &target, const std::string &args) override {
        // GDB client prepared to serve symbol lookup
        if (args == "qSymbol::") {
            return {"OK"};
        }

        auto tokens = str::split(args, ':');

        // set value of symbol
        if (tokens.size() == 2) {
            auto symValue = tokens.at(1);
            auto symName = tokens.at(2);
            return {"OK"};
        }

        return {""};
    }
};

class GetTraceStatus : public Command {
public:
    std::string getName() const override { return "qTStatus"; }
    Result exec(Target &target, const std::string &args) override { return {""}; }
};

class IsAttachedToExistingProcess : public Command {
public:
    std::string getName() const override { return "qAttached"; }
    Result exec(Target &target, const std::string &args) override { return {"0"}; }
};

class GetCurrentThreadId : public Command {
public:
    std::string getName() const override { return "qC"; }
    Result exec(Target &target, const std::string &args) override { return {""}; }
};

class GetSectionOffsets : public Command {
public:
    std::string getName() const override { return "qOffsets"; }
    Result exec(Target &target, const std::string &args) override { return {"Text=0;Data=0;Bss=0"}; }
};

}

}