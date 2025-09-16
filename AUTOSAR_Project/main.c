/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

/* MCAL includes. */
#include "gpio.h"

/* Definitions for the event bits in the event group. */
#define mainGREEN_LED_BIT ( 1UL << 0UL )  /* Event bit 0 */
#define mainRed_LED_BIT   ( 1UL << 1UL )  /* Event bit 1 */

EventGroupHandle_t xEventGroup;

/*
 * The HW setup function
 */
static void prvSetupHardware(void);


void vEventBitSettingTask(void *pvParameters);
void vEventBitReadingTask(void *pvParameters);

int main()
{
    prvSetupHardware();
    /* Before an event group can be used it must first be created. */
    xEventGroup = xEventGroupCreate();

    /* Create the task that sets event bits in the event group. */
    xTaskCreate(vEventBitSettingTask, "Bit Setter", 256, NULL, 1, NULL);

    /* Create the task that waits for event bits to get set in the event group. */
    xTaskCreate(vEventBitReadingTask, "Bit Reader", 256, NULL, 2, NULL);

    /* Start the scheduler so the created tasks start executing. */
    vTaskStartScheduler();
    /* The following line should never be reached. */
    for (;;);

}

static void prvSetupHardware(void)
{
    /* Place here any needed HW initialization such as GPIO, UART, etc.  */
    GPIO_BuiltinButtonsLedsInit();

}

void vEventBitSettingTask(void *pvParameters)
{
    const TickType_t xDelay1000ms = pdMS_TO_TICKS(1000UL);
    for (;;)
    {
        /* Delay for a short while before starting the next loop. */
        vTaskDelay(xDelay1000ms);
        xEventGroupSetBits(xEventGroup, mainGREEN_LED_BIT);

        /* Delay for a short while before setting the other bit. */
        vTaskDelay(xDelay1000ms);
        xEventGroupSetBits(xEventGroup, mainRed_LED_BIT);
    }
}

void vEventBitReadingTask(void *pvParameters)
{
    EventBits_t xEventGroupValue;
    const EventBits_t xBitsToWaitFor = ( mainGREEN_LED_BIT | mainRed_LED_BIT);

    for (;;)
    {
        /* Block to wait for event bits to become set within the event group. */
        xEventGroupValue = xEventGroupWaitBits( xEventGroup,    /* The event group to read. */
                                                xBitsToWaitFor, /* Bits to test. */
                                                pdTRUE,         /* Clear bits on exit if the unblock condition is met. */
                                                pdTRUE,        /* Wait for all bits. */
                                                portMAX_DELAY); /* Don't time out. */
        /* Toggle a certain LED for each bit that was set. */
        if ((xEventGroupValue & mainGREEN_LED_BIT) != 0)
        {
           GPIO_GreenLedToggle();
        }
        if ((xEventGroupValue & mainRed_LED_BIT) != 0)
        {
            GPIO_RedLedToggle();
        }
    }
}

/*-----------------------------------------------------------*/
