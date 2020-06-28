#pragma once

#include <sstream>
#include "command/Command.hpp"
#include "target/Target.hpp"

namespace gdb_stub {

namespace cmd {

class KillProcess : public Command {
public:
    std::string getName() const override { return "vKill"; }
    Result exec(Target &target, const std::string &args) override {
        target.status = TargetStatus::KILLED;
        return {"OK"};
    }
};


}

}