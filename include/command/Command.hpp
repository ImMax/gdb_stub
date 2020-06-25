#pragma once

#include <string>
#include <vector>
#include "target/Target.hpp"

namespace gdb_stub {

namespace cmd {

constexpr auto E01 = "E01";

enum class Status {
    SUCCESS,
    FAILED
};

struct Result {
    Result(const std::string &val, Status status = Status::SUCCESS) : val(val), status(status) {}
    std::string val;
    Status status;
};

class Command {
public:
    virtual std::string getName() const = 0;
    virtual Result exec(Target &target, const std::string &args) = 0;
    virtual ~Command() = default;
};

}

}