/**
 * @file main.c
 * @brief Register a new shell command with no arguments
 * 
 * This sample demonstrates how to create a new shell command in Zephyr
 * that does not take any arguments. The command, when executed, prints
 * a simple message to the shell console.
 * 
 * To run the command, type "hello" in the shell console after starting
 * the application., also type help to display available commands.
 * 
 * You can set the CONFIG_SHELL_MINIMAL option in prj.conf to only implement 
 * the help command and our new command.
 */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

static int hello_cmd_handler( const struct shell *sh );

/*register one new dummy command that prints a message*/
SHELL_CMD_REGISTER( hello, NULL, "Prints a dummy message", hello_cmd_handler );

/*this is the action the "hello" command is going to execute when typed, in our case
just print the message "This is just a dummy command"*/
static int hello_cmd_handler( const struct shell *sh )
{
    shell_print(sh, "This is just a dummy command\n");

    return 0;
}
