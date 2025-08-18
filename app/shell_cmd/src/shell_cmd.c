
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

#include "shell_cmd.h"

static int cmd_version(const struct shell *shell, size_t argc, char **argv) {
    shell_print(shell, "Hello, %s!", argc > 1 ? argv[1] : "Zephyr");
    return 0;
}

static int cmd_demo_ping(const struct shell *shell, size_t argc, char **argv) {
    shell_print(shell, "cmd_demo_ping, %s!", argc > 1 ? argv[1] : "cmd_demo_ping");
    return 0;
}

static int cmd_demo_params(const struct shell *shell, size_t argc, char **argv) {
    shell_print(shell, "cmd_demo_params, %s!", argc > 1 ? argv[1] : "cmd_demo_params");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_demo,
        SHELL_CMD(params, NULL, "Print params command.", cmd_demo_params),
        SHELL_CMD(ping,   NULL, "Ping command.", cmd_demo_ping),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(demo, &sub_demo, "Demo commands", NULL);
SHELL_CMD_REGISTER(version, NULL, "Show kernel version", cmd_version);
