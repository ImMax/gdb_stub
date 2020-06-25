#pragma once

#include <vector>
#include <memory>
#include "command/Command.hpp"
#include "command/CommonCommands.hpp"
#include "command/QueryCommands.hpp"
#include "command/MultiLetterCommands.hpp"

namespace gdb_stub {

std::vector<std::shared_ptr<cmd::Command>> DEFAULT_COMMANDS_LIST {
        std::make_shared<cmd::GetHaltReason>(),
        std::make_shared<cmd::ReadGeneralRegisters>(),
        std::make_shared<cmd::Continue>(),
        std::make_shared<cmd::ReadRegister>(),
        std::make_shared<cmd::InsertBreakpoint>(),
        std::make_shared<cmd::RemoveBreakpoint>(),
        std::make_shared<cmd::ReadMemory>(),
        std::make_shared<cmd::SetTreadForNextCmd>(),

        std::make_shared<cmd::GetSupportedFeatures>(),
        std::make_shared<cmd::GetCurrentThreadId>(),
        std::make_shared<cmd::GetFirstTracepointData>(),
        std::make_shared<cmd::IsAttachedToExistingProcess>(),
        std::make_shared<cmd::GetSectionOffsets>(),
        std::make_shared<cmd::SymbolCmd>(),
        std::make_shared<cmd::GetTraceStatus>(),
        std::make_shared<cmd::StartNoAckMode>(),

        std::make_shared<cmd::KillProcess>(),
};

}