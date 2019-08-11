/*******************************************************************************
* File Name: cycfg_peripherals.h
*
* Description:
* Peripheral Hardware Block configuration
* This file was automatically generated and should not be modified.
* 
********************************************************************************
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
********************************************************************************/

#if !defined(CYCFG_PERIPHERALS_H)
#define CYCFG_PERIPHERALS_H

#include "cycfg_notices.h"
#include "cy_scb_spi.h"
#include "cy_sysclk.h"
#include "cy_scb_uart.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define WS0_SPI_HW SCB0
#define WS0_SPI_IRQ scb_0_interrupt_IRQn
#define WS1_SPI_HW SCB1
#define WS1_SPI_IRQ scb_1_interrupt_IRQn
#define WS2_SPI_HW SCB2
#define WS2_SPI_IRQ scb_2_interrupt_IRQn
#define WS3_SPI_HW SCB3
#define WS3_SPI_IRQ scb_3_interrupt_IRQn
#define WS4_SPI_HW SCB4
#define WS4_SPI_IRQ scb_4_interrupt_IRQn
#define UART_STDIO_HW SCB5
#define UART_STDIO_IRQ scb_5_interrupt_IRQn

extern const cy_stc_scb_spi_config_t WS0_SPI_config;
extern const cy_stc_scb_spi_config_t WS1_SPI_config;
extern const cy_stc_scb_spi_config_t WS2_SPI_config;
extern const cy_stc_scb_spi_config_t WS3_SPI_config;
extern const cy_stc_scb_spi_config_t WS4_SPI_config;
extern const cy_stc_scb_uart_config_t UART_STDIO_config;

void init_cycfg_peripherals(void);

#if defined(__cplusplus)
}
#endif


#endif /* CYCFG_PERIPHERALS_H */
