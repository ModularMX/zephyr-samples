/**
 * @file main.c
 * @brief New shell command with a dictionary argument
 * 
 * This sample demonstrates how to create a new shell command in Zephyr
 * that accepts a dictionary of key-value pairs as arguments.
 * The command "hello" is registered with four dictionary entries:
 * "dic1", "dic2", "dic3", and "dic4", each associated with a unique integer value.
 * Example usage:
 * 
 * uart:~$ hello dic1
 * Hello dic: dic1, with value: 1
 * 
 * You can set the CONFIG_SHELL_MINIMAL option in prj.conf to only implement 
 * the help command and our new command.
 */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

static int hello_dic_cmd_handler(const struct shell *sh, size_t argc, char **argv, void *data);

/* Register up to 4 key-value pairs as dictionary subcommands */
SHELL_SUBCMD_DICT_SET_CREATE(
    sub_hello_cmds,
    hello_dic_cmd_handler,
    (dic1, 1, "dictionary 1"), (dic2, 2, "dictionary 2"),
    (dic3, 3, "dictionary 3"), (dic4, 4, "dictionary 4")
);

/* Register the "hello" command with 4 subcommands: "dic1", "dic2", "dic3" and "dic4" */
SHELL_CMD_REGISTER(hello, &sub_hello_cmds, "Prints a message with a dictionary argument value", NULL);

/* Handler for the "hello" command with dictionary subcommands. */
static int hello_dic_cmd_handler(const struct shell *sh, size_t argc, char **argv, void *data)
{
    /* Print the dictionary name passed as an argument, plus its value */
    shell_print(sh, "Hello dic: %s, with value: %d", argv[0], (int)data);

    return 0;
}
