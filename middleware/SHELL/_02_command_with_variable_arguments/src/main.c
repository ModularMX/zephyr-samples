/**
 * @file main.c
 * @brief Register a new shell command with a dynamic number of arguments
 * 
 * This sample demonstrates how to create a new shell command in Zephyr
 * that takes a dynamic number of arguments. The command, when executed, prints
 * all given arguments to the shell console.
 * 
 * To run the command, type "hello <argument1> <argument2> ... <argumentN>" 
 * in the shell console after starting the application, Examples:
 * 
 * uart:~$ hello arg1 arg2 arg3
 * 
 * You can set the CONFIG_SHELL_MINIMAL option in prj.conf to only implement 
 * the help command and our new command.
 */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

static int hello_cmd_handler( const struct shell *sh, size_t argc, char **argv );

/*register one new dummy command that prints a message*/
SHELL_CMD_REGISTER( hello, NULL, "Prints all given arguments!", hello_cmd_handler );

/*this is the action the "hello" command is going to execute when typed, just type
the command followed by any number of arguments*/
static int hello_cmd_handler( const struct shell *sh, size_t argc, char **argv )
{
    /*print the command typed plus the number of arguments separated by an space*/
    shell_print(sh, "command: %s, with %d arguments", argv[0], argc-1);

    /*print every argument separated by a new line*/
    for (int cnt = 1; cnt < argc; cnt++) {
            shell_print(sh, "  argv[%d] = %s", cnt, argv[cnt]);
    }
    return 0;
}
