/**
 * @file main.c
 * @brief Register a new shell command with one mandatory argument
 * 
 * This sample demonstrates how to create a new shell command in Zephyr
 * that takes one mandatory argument. The command, when executed, prints
 * a simple message to the shell console.
 * 
 * To run the command, type "hello <parameter>" in the shell console after starting
 * the application, examples:
 * 
 * uart:~$ hello arg1
 * 
 * You can set the CONFIG_SHELL_MINIMAL option in prj.conf to only implement 
 * the help command and our new command.
 */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

static int hello_cmd_handler(const struct shell *sh, size_t argc, char **argv);

/*register one new dummy command that accepts only one argument*/
SHELL_CMD_ARG_REGISTER(hello, NULL, "Print a message when one argument is provided!", hello_cmd_handler, 2, 0);

/*this is the action the "hello <parameter>" command is going to execute when typed*/
static int hello_cmd_handler(const struct shell *sh, size_t argc, char **argv)
{
    /*the parameter string is stored in argv[1] while the command itself its in argv[0]*/
    shell_print(sh, "Dummy command with one argument: %s\n", argv[1]);
    return 0;
}
