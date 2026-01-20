/**
 * @file main.c
 * @brief Register a new shell command with two subcommands
 * 
 * This sample demonstrates how to create a new shell command in Zephyr
 * that includes two subcommands. The main command is "hello", and it has
 * two subcommands: "sub1" and "sub2". Each subcommand executes a
 * different handler function that prints a message to the shell.
 * 
 * To run the command, type "hello" in the shell console and then
 * type "sub1" or "sub2" to execute the respective subcommand.
 * 
 * uart:~$ hello sub1
 * This is just a dummy sub command 1
 * uart:~$ hello sub2
 * This is just a dummy sub command 2
 * 
 * You can set the CONFIG_SHELL_MINIMAL option in prj.conf to only implement 
 * the help command and our new command.
 */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

static int hello_sub_cmd1_handler(const struct shell *sh);
static int hello_sub_cmd2_handler(const struct shell *sh);

/*register at least two sub commands we'll use with hello command*/
SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_hello_cmds,
    SHELL_CMD(sub1, NULL, "Sub 1 command.", hello_sub_cmd1_handler),
    SHELL_CMD(sub2, NULL, "Sub 2 command.", hello_sub_cmd2_handler),
    SHELL_SUBCMD_SET_END
);

/* Register the "hello" command with two subcommands: "sub1" and "sub2" */
SHELL_CMD_REGISTER(hello, &sub_hello_cmds, "Run command with two subcommands", NULL);

/*this is the action the "hello sub1" command is going to execute when typed*/
static int hello_sub_cmd1_handler(const struct shell *sh)
{
    shell_print(sh, "This is just a dummy sub command 1");
    return 0;
}

/*this is the action the "hello sub2" command is going to execute when typed*/
static int hello_sub_cmd2_handler(const struct shell *sh)
{
    shell_print(sh, "This is just a dummy sub command 2");
    return 0;
}
