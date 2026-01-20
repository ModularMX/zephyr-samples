/**
 * @file main.c
 * @brief New shell command with two subcommands and one argument each
 * 
 * This sample demonstrates how to create a new shell command in Zephyr
 * with two subcommands, each accepting one argument. Examples of usage:
 * 
 * uart:~$ hello sub1 arg1
 * Sub command 1 with one argument: arg1
 * uart:~$ hello sub2 arg2
 * Sub command 2 with one argument: arg2
 * 
 * You can set the CONFIG_SHELL_MINIMAL option in prj.conf to only implement 
 * the help command and our new command.
 */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

static int hello_sub_cmd1_handler(const struct shell *sh, size_t argc, char **argv);
static int hello_sub_cmd2_handler(const struct shell *sh, size_t argc, char **argv);

/*register at least two sub commands we'll use with hello command*/
SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_hello_cmds,
    SHELL_CMD_ARG(sub1, NULL, "Sub 1 command.", hello_sub_cmd1_handler, 2, 0),
    SHELL_CMD_ARG(sub2, NULL, "Sub 2 command.", hello_sub_cmd2_handler, 2, 0),
    SHELL_SUBCMD_SET_END
);

/* Register the "hello" command with two subcommands: "sub1" and "sub2" */
SHELL_CMD_REGISTER(hello, &sub_hello_cmds, "Run command with two subcommands each accepting one argument", NULL);

/*this is the action the "hello sub1 <parameter>" command is going to execute when typed*/
static int hello_sub_cmd1_handler(const struct shell *sh, size_t argc, char **argv)
{
    /*the parameter string is stored in argv[1] while the command itself its in argv[0]*/
    shell_print(sh, "Sub command 1 with one argument: %s\n", argv[1]);
    return 0;
}

/*this is the action the "hello sub2 <parameter>" command is going to execute when typed*/
static int hello_sub_cmd2_handler(const struct shell *sh, size_t argc, char **argv)
{
    /*the parameter string is stored in argv[1] while the command itself its in argv[0]*/
    shell_print(sh, "Sub command 2 with one argument: %s\n", argv[1]);
    return 0;
}
