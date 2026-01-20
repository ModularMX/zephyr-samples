/**
 * @file main.c
 * @brief New shell command with two sub-levels
 * 
 * This sample demonstrates how to create a new shell command in Zephyr
 * that has two levels of subcommands. The main command is "hello", which has
 * two subcommands: "sub1" and "sub2". Each of these subcommands further contains
 * two additional subcommands. Examples of usage:
 * 
 * uart:~$ hello sub1 sub11
 * This is just a dummy sub command 11
 * 
 * You can set the CONFIG_SHELL_MINIMAL option in prj.conf to only implement 
 * the help command and our new command.
 */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

static int hello_sub1_cmd1_handler(const struct shell *sh);
static int hello_sub1_cmd2_handler(const struct shell *sh);
static int hello_sub2_cmd1_handler(const struct shell *sh);
static int hello_sub2_cmd2_handler(const struct shell *sh);

/*register the third level of command for the sub1 sub command*/
SHELL_STATIC_SUBCMD_SET_CREATE(
    sub1_hello_cmds,
    SHELL_CMD(sub11, NULL, "Sub 11 command.", hello_sub1_cmd1_handler),
    SHELL_CMD(sub12, NULL, "Sub 12 command.", hello_sub1_cmd2_handler),
    SHELL_SUBCMD_SET_END
);

/*register the third level of command for the sub2 sub command*/
SHELL_STATIC_SUBCMD_SET_CREATE(
    sub2_hello_cmds,
    SHELL_CMD(sub21, NULL, "Sub 21 command.", hello_sub2_cmd1_handler),
    SHELL_CMD(sub22, NULL, "Sub 22 command.", hello_sub2_cmd2_handler),
    SHELL_SUBCMD_SET_END
);

/*register two sub commands we'll use with hello command and also each of them will 
have a second level of subcomands*/
SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_hello_cmds,
    SHELL_CMD(sub1, &sub1_hello_cmds, "Sub 1 command.", NULL),
    SHELL_CMD(sub2, &sub2_hello_cmds, "Sub 2 command.", NULL),
    SHELL_SUBCMD_SET_END
);

/* Register the "hello" command with two subcommands: "sub1" and "sub2" */
SHELL_CMD_REGISTER(hello, &sub_hello_cmds, "Command with two subcommands each having two subcommands", NULL);

/*this is the action the "hello sub1 sub11" command is going to execute when typed*/
static int hello_sub1_cmd1_handler(const struct shell *sh)
{
    shell_print(sh, "This is just a dummy sub command 11");
    return 0;
}

/*this is the action the "hello sub1 sub12" command is going to execute when typed*/
static int hello_sub1_cmd2_handler(const struct shell *sh)
{
    shell_print(sh, "This is just a dummy sub command 12");
    return 0;
}

/*this is the action the "hello sub2 sub21" command is going to execute when typed*/
static int hello_sub2_cmd1_handler(const struct shell *sh)
{
    shell_print(sh, "This is just a dummy sub command 21");
    return 0;
}

/*this is the action the "hello sub2 sub22" command is going to execute when typed*/
static int hello_sub2_cmd2_handler(const struct shell *sh)
{
    shell_print(sh, "This is just a dummy sub command 22");
    return 0;
}