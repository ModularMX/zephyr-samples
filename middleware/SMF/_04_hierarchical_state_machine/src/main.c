/**
 * @file main.c
 * @brief Hierarchical State Machine with two sub-states
 * 
 * This sample demonstrates a hierarchical state machine, the state machine
 * has a parent state with two child states. The parent state has entry and exit
 * functions that are executed when entering and exiting the child states.
 * Each child state has a run function that prints a message and transitions
 * to the next state.
 */
#include <zephyr/kernel.h>
#include <zephyr/smf.h>

/*my states */
typedef enum {
    parent,
    state1,
    state2,
    state3
} MyStates;

static void parent_entry(void *data);
static void parent_exit(void *data);
static enum smf_state_result state1_run(void *data);
static enum smf_state_result state2_run(void *data);
static enum smf_state_result state3_run(void *data);

/* Populate state table  with both states and only the run state on each one*/
static const struct smf_state state_table[] = {
    [parent] = SMF_CREATE_STATE(parent_entry, NULL, parent_exit, NULL, NULL),
    [state1] = SMF_CREATE_STATE(NULL, state1_run, NULL, &state_table[parent], NULL),
    [state2] = SMF_CREATE_STATE(NULL, state2_run, NULL, &state_table[parent], NULL),
    [state3] = SMF_CREATE_STATE(NULL, state3_run, NULL, NULL, NULL),
};

int main(void)
{
    /* declare the state machine */
    struct smf_ctx sm;
    /* set up the state machine to run with the first state */
    smf_set_initial( &sm, &state_table[state1] );

    while(true)
    {
        printk("----- Machine cycle -----\n");
        /* run the state machine every second */
        smf_run_state( &sm );
        k_msleep(1000);
    }

    return 0;
}

/* Parent state entry/exit functions */
static void parent_entry(void *data)
{
    printk("Entering Parent State.\n");
}

static void parent_exit(void *data)
{
    printk("Exiting Parent State.\n");
}

/* just print a message and transition to next state */
static enum smf_state_result state1_run(void *data)
{
    printk("State 1 running.\n");
    /*transition to state 2*/
    smf_set_state(SMF_CTX(data), &state_table[state2]);
    /* Indicate event was handled */
    return SMF_EVENT_HANDLED;
}

/* just print a message and transition back to previous state */
static enum smf_state_result state2_run(void *data) 
{
    printk("State 2 running.\n");
    /*transition to state 3*/
    smf_set_state(SMF_CTX(data), &state_table[state3]);
    /* Indicate event was handled */
    return SMF_EVENT_HANDLED;
}

static enum smf_state_result state3_run(void *data) 
{
    printk("State 3 running.\n");
    /*transition to state 1*/
    smf_set_state(SMF_CTX(data), &state_table[state1]);
    /* Indicate event was handled */
    return SMF_EVENT_HANDLED;
}