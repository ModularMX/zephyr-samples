/**
 * @file main.c
 * @brief Simple State Machine with two states
 * 
 * This sample demonstrates a simple state machine implementation with two states.
 * Each state prints a message when it is running and transitions to the other state.
 */
#include <zephyr/kernel.h>
#include <zephyr/smf.h>

/*my states */
typedef enum {
    state1,
    state2,
} MyStates;

static enum smf_state_result state1_run(void *data);
static enum smf_state_result state2_run(void *data);

/* Populate state table  with both states and only the run state on each one*/
static const struct smf_state state_table[] = {
    [state1] = SMF_CREATE_STATE(NULL, state1_run, NULL, NULL, NULL),
    [state2] = SMF_CREATE_STATE(NULL, state2_run, NULL, NULL, NULL),
};

int main(void)
{
    /* declare the state machine */
    struct smf_ctx sm;
    /* set up the state machine to run with the first state */
    smf_set_initial( &sm, &state_table[state1] );

    while(true)
    {
        /* run the state machine every second */
        smf_run_state( &sm );
        k_msleep(1000);
    }

    return 0;
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
    /*transition to state 1*/
    smf_set_state(SMF_CTX(data), &state_table[state1]);
    /* Indicate event was handled */
    return SMF_EVENT_HANDLED;
}