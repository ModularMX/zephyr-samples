/**
 * @file main.c
 * @brief State machine with Data Passing
 * 
 * This sample demonstrates a simple state machine implementation with two states
 * that pass data between them using a user-defined structure.
 * The state machine alternates between State 1 and State 2 based on an event variable.
 */
#include <zephyr/kernel.h>
#include <zephyr/smf.h>

/*my states */
typedef enum {
    state1,
    state2,
} MyStates;

/* decalre a use defined state machnine with user data*/
struct state_machine {
    /*the first elemnt always should be the smf structure*/
    struct smf_ctx ctx;
    /*from this point you can add as many elements you need*/
    uint32_t event;
};

static enum smf_state_result state1_run(void *data);
static enum smf_state_result state2_run(void *data);

/* Populate state table  with both state and only the run state on each one*/
static const struct smf_state state_table[] = {
        [state1] = SMF_CREATE_STATE(NULL, state1_run, NULL, NULL, NULL),
        [state2] = SMF_CREATE_STATE(NULL, state2_run, NULL, NULL, NULL),
};

int main(void)
{
    /* declare the state machine */
    struct state_machine sm;
    /*init the dummy event to zero*/
    sm.event = 0;

    /* set up the state machine to run with the first state */
    smf_set_initial( SMF_CTX(&sm), &state_table[state1] );

    while(true)
    {
        /* run the state machine every second */
        smf_run_state( SMF_CTX(&sm) );
        k_msleep(1000);
    }

    return 0;
}

/* just print a message and transition to next state only if
event is equal to 0*/
static enum smf_state_result state1_run(void *data) 
{
    struct state_machine *obj = data;

    printk("State 1 run, event = %d.\n", obj->event);
    
    if (obj->event == 0) {
        obj->event = 1;
        smf_set_state(SMF_CTX(data), &state_table[state2]);
    }
    return SMF_EVENT_HANDLED;
}

/* just print a message and transition to next state only if
event is equal to 1*/
static enum smf_state_result state2_run(void *data) 
{
    struct state_machine *obj = data;

    printk("State 2 run, event = %d.\n", obj->event);
    
    if (obj->event == 1) {
        obj->event = 0;
        smf_set_state(SMF_CTX(data), &state_table[state1]);
    }
    return SMF_EVENT_HANDLED;
}