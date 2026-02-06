/**
 * @file main.c
 * @brief Event Driven State Machine with two states
 * 
 * This sample demonstrates a simple event-driven state machine implementation.
 * In this example, a timer expiration event  sets a new "event" 
 * that triggers the state machine execution.
 */
#include <zephyr/kernel.h>
#include <zephyr/smf.h>

#define DUMMY_EVENT BIT(0)

/*my states */
typedef enum {
    state1,
    state2,
} MyStates;

/* declare a user defined state machine with user data*/
struct state_machine {
    /*the first element always should be the smf structure*/
    struct smf_ctx ctx;
    /*event to post and wait for*/
    struct k_event event;
};

static enum smf_state_result state1_run(void *data);
static enum smf_state_result state2_run(void *data);

/* Populate state table  with both states and only the run state on each one*/
static const struct smf_state state_table[] = {
    [state1] = SMF_CREATE_STATE(NULL, state1_run, NULL, NULL, NULL),
    [state2] = SMF_CREATE_STATE(NULL, state2_run, NULL, NULL, NULL),
};

static void expiry_callback(struct k_timer *timer_id);

/* declare a software timer called timer_id, and expiry_callback
with no stop callback function */
K_TIMER_DEFINE( timer_id, expiry_callback, NULL );

int main(void)
{
    /* declare the state machine */
    struct state_machine sm;
    
    /* initialize the event object */
    k_event_init( &sm.event );

    /* start the timer to expire every 1 second */
    k_timer_start( &timer_id, K_MSEC(1000), K_MSEC(1000) );
    k_timer_user_data_set( &timer_id, &sm );

    /* set up the state machine to run with the first state */
    smf_set_initial( SMF_CTX(&sm), &state_table[state1] );

    while(true)
    {
        /* wait for the dummy event indefinitely */
        k_event_wait(&sm.event, DUMMY_EVENT, true, K_FOREVER);
        /* run the state machine only after the event occurs */
        smf_run_state( SMF_CTX(&sm) );
    }

    return 0;
}

/*for simplicity we are using a software timer to set an event,
but this can be anything like the press of a button or other 
external interrupt*/
static void expiry_callback(struct k_timer *timer_id)
{
    struct state_machine *sm = k_timer_user_data_get(timer_id);
    printk("Timer expired, setting event.\n");
    /* set the dummy event every time the timer expires */
    k_event_set(&sm->event, DUMMY_EVENT);
}

/* just print a message and transition to next state */
static enum smf_state_result state1_run(void *data)
{
    printk("State 1 running after the event.\n");
    /*transition to state 2*/
    smf_set_state(SMF_CTX(data), &state_table[state2]);
    /* Indicate event was handled */
    return SMF_EVENT_HANDLED;
}

/* just print a message and transition back to previous state */
static enum smf_state_result state2_run(void *data) 
{
    printk("State 2 running after the event.\n");
    /*transition to state 1*/
    smf_set_state(SMF_CTX(data), &state_table[state1]);
    /* Indicate event was handled */
    return SMF_EVENT_HANDLED;
}