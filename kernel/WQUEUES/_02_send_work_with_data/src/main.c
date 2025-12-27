/**
 * @file main.c
 * @brief Send work + data to system work queue
 * 
 * The sample shows how to pass data to a work queue, and also how to declare a work
 * in run time. To pass data we need to declare a structure where the first element
 * should be the "struct k_work" acting as a work handler, then any other element
 * could act as the extra data we would like to send, the system macro CONTAINER_OF
 * can be used to extract any "data" element we want inside the function acting as a
 * "work" 
 */
/* Include libraries */
#include <zephyr/kernel.h>

/*define a structure with the work queue and data to send
it is important the first element be the k_work*/
struct work_data {
    struct k_work work_to_queue;
    uint8_t data;
};

/*This is the function acting as the "work to do"*/
void work_handler( struct k_work *work );

int main( void )
{
    struct work_data my_wd;
    my_wd.data = 0;
    
    /* init our k_work which is part of the structure */
    k_work_init( &my_wd.work_to_queue, work_handler );

    while(1)
    {
        
        /*just increase the data value before send the work*/
        my_wd.data++;
        k_work_submit( &my_wd.work_to_queue );

        k_msleep( 1000 );
    }
}

/*the function will be called "in" the system work queue thread
active by default when a "work" has been submitted*/
void work_handler( struct k_work *work ) 
{
    /* get the data from the structure */
    struct work_data *data = CONTAINER_OF( work, struct work_data, work_to_queue );

    printk("Data passed to work queue: %d\r", data->data);
}