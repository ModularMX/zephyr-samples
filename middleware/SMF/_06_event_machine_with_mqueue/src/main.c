/**
 * @file main.c
 * @brief Event-driven machine using message queue
 * 
 * This sample demonstrates an event-driven state machine implementation using a message queue.
 * A timer periodically posts messages to the queue, triggering state transitions based on the
 * received messages.
 */
#include <zephyr/kernel.h>
#include <zephyr/smf.h>

/*my states */
typedef enum {
    state1,
    state2,
} MyStates;

/*declare a message queue to hold 5 messages*/
struct k_msgq my_msgq;
uint8_t my_msgq_buffer[ 5 * sizeof(uint32_t) ];

/* decalre a use defined state machnine with user data*/
struct state_machine {
    /*the first element always should be the smf structure*/
    struct smf_ctx ctx;
    /*data to pass to state machine*/
    uint32_t data;
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
    k_msgq_init( &my_msgq, my_msgq_buffer, sizeof(uint32_t), 5 );
    /*initialize the timer */
    k_timer_start( &timer_id, K_MSEC(1000), K_MSEC(1000) );
    k_timer_user_data_set( &timer_id, &my_msgq );

    /* set up the state machine to run with the first state */
    smf_set_initial( SMF_CTX(&sm), &state_table[state1] );

    while(true)
    {
        /* wait for a new data to arrive */
        k_msgq_get(&my_msgq, &sm.data, K_FOREVER);
        /* run the state machine only when the event occurs */
        smf_run_state( SMF_CTX(&sm) );
    }

    return 0;
}

static void expiry_callback(struct k_timer *timer_id)
{
    static uint32_t dummy_msg = 0;
    struct k_msgq *msgq = k_timer_user_data_get(timer_id);
    
    printk("Timer expired, posting message: %d.\n", dummy_msg);
    /*publish a new message*/
    k_msgq_put(msgq, &dummy_msg, K_NO_WAIT);
    dummy_msg ^= 1;
}

/* just print a message and transition to next state only
when data is equal to 1 */
static enum smf_state_result state1_run(void *data)
{
    struct state_machine *obj = data;

    if(obj->data == 1) {
        printk("State 1 running after: %d.\n", obj->data);
        smf_set_state(SMF_CTX(data), &state_table[state2]);
    }
    /* Indicate event was handled */
    return SMF_EVENT_HANDLED;
}

/* just print a message and transition back to previous state only
when data is equal to 1 */
static enum smf_state_result state2_run(void *data) 
{
    struct state_machine *obj = data;

    if(obj->data == 1) {
        printk("State 2 running after: %d.\n", obj->data);
        smf_set_state(SMF_CTX(data), &state_table[state1]);
    }
    /* Indicate event was handled */
    return SMF_EVENT_HANDLED;
}