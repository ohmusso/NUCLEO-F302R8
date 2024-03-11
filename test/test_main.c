#include "test_main.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT        ( 0xffffff )

extern void Clock_ComEsp32Config(void);

/* The task functions. */
static void vTaskSample(void* pvParameters);

void vTestMain(void)
{
    printf("start: vTestMain\r\n");

    /* Create one of the two tasks. */
    xTaskCreate(vTaskSample, "Task 1", 100, NULL, 1, NULL); /* We are not using the task handle. */

    /* Start the scheduler to start the tasks executing. */
    vTaskStartScheduler();

    for (;;){
        /* never reache */
    }
}

static void vTaskSample(void* pvParameters)
{
    const char* pcTaskName = "Task 1 is running\r\n";
    volatile uint32_t ul;

    Clock_ComEsp32Config();

    /* As per most tasks, this task is implemented in an infinite loop. */
    for (;;)
    {
        /* Print out the name of this task. */
        printf(pcTaskName);

        /* Delay for a period. */
        for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
        {
            /* This loop is just a very crude delay implementation.  There is
            nothing to do in here.  Later exercises will replace this crude
            loop with a proper delay/sleep function. */
        }
    }
}

/* called by FreeRTOS */
void vApplicationTickHook(void) {}
