/**
 * @file main.c
 * @brief Shell command with with getopt staandard parsing
 * 
 * This sample demonstrates how to create a new shell command in Zephyr
 * that uses the standard getopt function to parse command-line options.
 * The new command "hello" accepts options -a, -b <value>, and -c <value>.
 * Each option triggers a different response when the command is executed.
 * 
 * Example usage:
 * 
 * uart:~$ hello -a -b value1 -c value2
 * Option -a was found
 * Option -b with value 'value1'
 * Option -c with value 'value2'
 * 
 * You can set the CONFIG_SHELL_MINIMAL option in prj.conf to only implement 
 * the help command and our new command.
 */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <unistd.h>

static int hello_cmd_handler(const struct shell *sh, size_t argc, char **argv);

/*register one new dummy command that accepts options -a, -b <value>, and -c <value>*/
SHELL_CMD_REGISTER(hello, NULL, "Shell command with getopt standard parsing", hello_cmd_handler);

/* this is the action the "hello" command is going to execute when typed */
static int hello_cmd_handler(const struct shell *sh, size_t argc, char **argv)
{
    int opt;
    struct getopt_state *state;
    
    /* loop throug all the possible options pased */
    while((opt = getopt(argc, argv, "ab:c:")) != -1) {
        /*get the structure with variables optarg, optind, opterr and optopt; */
        state = getopt_state_get();
        /*run actions according to each options flag passed*/
        switch(opt) {
            case 'a':
                shell_print(sh, "Option -a was found");
            break;
            case 'b':
                shell_print(sh, "Option -b with value '%s'", state->optarg);
            break;
            case 'c':
                shell_print(sh, "Option -c with value '%s'", state->optarg);
            break;
            default:
                shell_print(sh, "Unknown option: %c", opt);
            break;
        }
    }

    return 0;
}
