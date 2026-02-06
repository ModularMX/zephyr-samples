/**
 * @file main.c
 * @brief State Machine with entry and exit actions
 * 
 * This sample demonstrates a simple state machine with two states. Each state has
 * entry, run, and exit actions defined. The state machine transitions between the two states,
 * executing the corresponding entry and exit actions during the transitions.
 */
#include <zephyr/kernel.h>
#include <zephyr/smf.h>

/*my states */
typedef enum {
    state1,
    state2,
} MyStates;

static void state1_entry(void *data);
static enum smf_state_result state1_run(void *data);
static void state1_exit(void *data);

static void state2_entry(void *data);
static enum smf_state_result state2_run(void *data);
static void state2_exit(void *data);

/* Populate state table  with both state and entry, run & exit transitions on each one*/
static const struct smf_state state_table[] = {
        [state1] = SMF_CREATE_STATE(state1_entry, state1_run, state1_exit, NULL, NULL),
        [state2] = SMF_CREATE_STATE(state2_entry, state2_run, state2_exit, NULL, NULL),
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
        /* run the state machine every minute */
        smf_run_state( &sm );
        k_msleep(1000);
    }

    return 0;
}

/* just display a message when enter the state */
static void state1_entry(void *data) {
    printk("State 1 entry.\n");
}

/* just print a message and transition to next state */
static enum smf_state_result state1_run(void *data) {
    printk("State 1 run.\n");
    smf_set_state(SMF_CTX(data), &state_table[state2]);
    /* Indicate event was handled */
    return SMF_EVENT_HANDLED;
}

/* just display a message when exit the state */
static void state1_exit(void *data) {
    printk("State 1 exit.\n");
}

/* just display a message when enter the state */
static void state2_entry(void *data) {
    printk("State 2 entry.\n");
}

/* just print a message and transition back to previous state */
static enum smf_state_result state2_run(void *data) {
    printk("State 2 run.\n");
    smf_set_state(SMF_CTX(data), &state_table[state1]);
    /* Indicate event was handled */
    return SMF_EVENT_HANDLED;
}

/* just display a message when exit the state */
static void state2_exit(void *data) {
    printk("State 2 exit.\n");
}