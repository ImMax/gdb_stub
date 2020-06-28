#pragma once

#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <algorithm>
#include "command/Command.hpp"
#include "command/QueryCommands.hpp"
#include "command/CommonCommands.hpp"
#include "command/QueryCommands.hpp"
#include "command/MultiLetterCommands.hpp"

namespace gdb_stub {

using namespace boost::adaptors;

class CommandTable {
public:
    CommandTable(std::vector<cmd::Command_ptr> commands) {
        for (const auto &c : commands) {
            commandTable.insert({c->getName(), c});
        }
        boost::copy(commandTable
                    | map_keys
                    | filtered([](const std::string &name) { return name.size() == 1; })
                    | transformed([](const std::string &oneLetterName) { return oneLetterName.front(); }),
                    back_inserter(oneLetterNames));
        boost::sort(oneLetterNames);
    }

    cmd::Result exec(Target &target, const std::string &cmd) {
        auto cmdName = getCmdName(cmd);

        try {
            return commandTable.at(cmdName)->exec(target, {cmd});
        } catch (const std::out_of_range &ex) {
            // if command not found empty response should be returned
            return cmd::EMPTY_RESULT;
        }
    }

    std::string getCmdName(const std::string &cmd) {
        if (std::binary_search(oneLetterNames.begin(), oneLetterNames.end(), cmd.front())) {
            return {cmd.front()};
        }
        auto end = cmd.find_first_of("=:+0123456789");
        return end != std::string::npos ? cmd.substr(0, end) : cmd;
    }

private:
    std::map<std::string, std::shared_ptr<cmd::Command>> commandTable;
    std::vector<char> oneLetterNames;
};

}