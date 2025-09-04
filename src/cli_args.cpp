#include "cli_args.hpp"
#include "software_info.hpp"

#define SERVER_MODE_OPTION_DESCRIPTION  "Mode for building sources and etc."
#define CLIENT_MODE_OPTION_DESCRIPTION  "Mode for deploying sources and etc."
#define ABOUT_OPTION_DESCRIPTION        "Displaying software information"

CmdArgs gCmdArgs = { 0 };

void prepareCmdArgs(CLI::App& app, int argc, char** argv) {
    app.description(NV_DESCRIPTION);

    argv = app.ensure_utf8(argv);

    (void)argv;

    auto group = app.add_option_group("SoftwareMode", "Select mode or request software information");
    group->add_flag("-s,--server", gCmdArgs.isServerMode, SERVER_MODE_OPTION_DESCRIPTION);
    group->add_flag("-c, --client", gCmdArgs.isClientMode, CLIENT_MODE_OPTION_DESCRIPTION);
    group->add_flag("-a, --about", gCmdArgs.isShowAbout, ABOUT_OPTION_DESCRIPTION);
    group->require_option(1); // Требуется минимум 1 из 2
}
