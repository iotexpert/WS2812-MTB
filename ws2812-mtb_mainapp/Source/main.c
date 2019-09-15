/***************************************************************************//**
* \file main.c
* \version 1.0
*
* \brief
* Minimal new application template for 150MHz PSoC 6 devices. Debug is enabled 
* and platform clocks are set for high performance (144MHz CLK_FAST for CM4 
* core and 72MHz CLK_SLOW for CM0+) but with a peripheral-friendly CLK_PERI 
* frequency (72MHz).
*
********************************************************************************
* \copyright
* Copyright 2017-2019 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include "cy_device_headers.h"
#include "cycfg.h"
#include <stdio.h>
#include <ws2812Task.h>
#include "FreeRTOS.h"
#include "task.h"
#include "uartTask.h"


void ledTask(void *arg)
{
	(void)arg;
	while(1)
	{
		Cy_GPIO_Inv(red_PORT,red_PIN);
		vTaskDelay(500);
	}
}



int main(void)
{
    /* Set up the device based on configurator selections */
    init_cycfg_all();
    __enable_irq();

    xTaskCreate(ledTask,"LED Task",100,0,3,0);
    xTaskCreate(uartTask,"UART Task",1024,0,3,0);
    xTaskCreate(ws2812Task,"WS2812 Task",16*1024,0,3,0);

    vTaskStartScheduler();
}


