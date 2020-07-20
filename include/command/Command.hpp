#pragma once

#include <string>
#include <vector>
#include "target/Target.hpp"

namespace gdb_stub {

namespace cmd {

constexpr auto INTERRUPT_NAME = "int";
constexpr auto TRAP_NAME = "trap";

constexpr auto E01 = "E01";

enum class Status {
    SUCCESS,
    FAILED,
    EMPTY,
    CONTINUE,
};

struct Result {
    Result(const std::string &val, Status status = Status::SUCCESS) : val(val), status(status) {}
    std::string val;
    Status status;
};

const auto EMPTY_RESULT = Result("", Status::EMPTY);
const auto CONTINUE_RESULT = Result("", Status::CONTINUE);

class Command {
public:
    virtual std::string getName() const = 0;
    virtual Result exec(Target &target, const std::string &args) = 0;
    virtual ~Command() = default;
};

using Command_ptr = std::shared_ptr<Command>;

}

}