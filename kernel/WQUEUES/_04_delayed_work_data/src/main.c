/**
 * @file main.c
 * @brief Single delayable work with data
 * 
 * The sample shows how to send data to a delayable "work", it is pretty much the same
 * as the previous example, but this time as part of the structure we use the element of
 * type "k_work_delayable". inside the the "work" we use first the k_work_delayable_from_work
 * to extract a pointer to k_work_delayable structure before use the CONTAINER_OF macro
 * to extract the data we passed
 */
/* Include libraries */
#include <zephyr/kernel.h>

/*define a structure with the work queue and data to send
it is important the first element be the k_work_delayable*/
struct work_data {
    struct k_work_delayable work_to_queue;
    uint8_t data;
};

/*This is the function acting as the "work to do"*/
void work_handler( struct k_work *work );

int main( void )
{
    struct work_data my_dwd;
    my_dwd.data = 0;

    /* init our k_work_delayable which is part of the structure */
    k_work_init_delayable( &my_dwd.work_to_queue , work_handler );

    while(1)
    {
        printk("About to send a new work\r");
        /*just increase the data value before send the work*/
        my_dwd.data++;
        k_work_schedule( &my_dwd.work_to_queue, K_MSEC(500) );

        k_msleep( 2000 );
    }
}

/*the function will be called "in" the system work queue thread
active by default when a "work" has been submitted*/
void work_handler( struct k_work *work ) 
{
    /* get the k_work_delayable from the structure k_work */
    struct k_work_delayable *dwork = k_work_delayable_from_work( work );
    /* get the data from the structure */
    struct work_data *data = CONTAINER_OF( dwork, struct work_data, work_to_queue );

    printk("Data passed to work queue: %d\r", data->data);
}