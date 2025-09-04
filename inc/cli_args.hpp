#ifndef CLI_ARGS_HPP
#define CLI_ARGS_HPP

// Third-party imports (LICENSES are included)
#include "CLI11.hpp"

struct CmdArgs {
    bool isServerMode;
    bool isClientMode;
    bool isShowAbout;
};

extern CmdArgs gCmdArgs;

extern CLI::Option* gRemoveExtraOption;

void prepareCmdArgs(CLI::App& app, int argc, char** argv);

#endif // CLI_ARGS_HPP
