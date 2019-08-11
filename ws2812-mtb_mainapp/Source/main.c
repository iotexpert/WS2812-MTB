/** @source __main.c__
**
** __Code for the WS2812 Example Application_
**
** @author Copyright (C) June 15, 2019 __Alan Hawse__
** @version __1.0__   __Gen 1__</replaceable>
** @modified ____7/25/2019__Hassane El-Khoury__  __description of edit__</replaceable>
** @@
**
********************************************************************/

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */
/* Inclusion of system and local header files goes here */
/* Header file includes */
#include "cy_device_headers.h"
#include "cycfg.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "ws2812.h"
#include "uartTask.h"

/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */
/* #define and enum statements go here */

/* ==================================================================== */
/* ======================== global variables ========================== */
/* ==================================================================== */
/* Global variables definitions go here */

/* ==================================================================== */
/* ============================== data ================================ */
/* ==================================================================== */
/* Definition of datatypes go here */


/* ==================================================================== */
/* ==================== function prototypes =========================== */
/* ==================================================================== */
/* Function prototypes for public (external) functions go here */



/* ==================================================================== */
/* ============================ functions ============================= */
/* ==================================================================== */
/*******************************************************************************
* Function Name: void Task_Display (void *pvParameters)
********************************************************************************
* Summary:
*  Task that processes the touch command received and the updates the display
*  with the corresponding menu / text page
*
* Parameters:
*  void *pvParameters : Task parameter defined during task creation (unused)
*
* Return:
*  void
*
*******************************************************************************/


void ledTask(void *arg)
{
	(void)arg;
	printf("Starting Blinking LED task\r\n");

	while(1)
	{
		Cy_GPIO_Inv(red_PORT,red_PIN);
		vTaskDelay(500);
	}
}

void ws2812LightShowTask(void *arg)
{
	(void)arg;
	printf("Starting LightShow task\r\n");

	while(1)
	{
		vTaskDelay(100);
	}
}


int main(void)
{
    /* Set up the device based on configurator selections */
    init_cycfg_all();
    __enable_irq();

    xTaskCreate(uartTask,"UART Task",2*1024,0,3,0);

    if(xTaskCreate(ws2812Task,"WS2812 Task",32*1024,0,3,0) == pdFALSE)
    {
    	printf("ws2812 task unsuccessful");
    }

    if(xTaskCreate(ledTask,"LED Task",100,0,3,0) == pdFALSE)
    {
    	printf("LED blinking task unsuccessful");
    }

    xTaskCreate(ws2812LightShowTask,"WS2812 LightShow Task",1024,0,3,0);

    vTaskStartScheduler();
}


