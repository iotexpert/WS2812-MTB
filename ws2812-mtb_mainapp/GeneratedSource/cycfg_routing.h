/*******************************************************************************
* File Name: cycfg_routing.h
*
* Description:
* Establishes all necessary connections between hardware elements.
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

#if !defined(CYCFG_ROUTING_H)
#define CYCFG_ROUTING_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "cycfg_notices.h"
void init_cycfg_routing(void);
#define init_cycfg_connectivity() init_cycfg_routing()
#define ioss_0_port_0_pin_2_HSIOM P0_2_SCB0_SPI_MOSI
#define ioss_0_port_10_pin_0_HSIOM P10_0_SCB1_SPI_MOSI
#define ioss_0_port_5_pin_0_HSIOM P5_0_SCB5_UART_RX
#define ioss_0_port_5_pin_1_HSIOM P5_1_SCB5_UART_TX
#define ioss_0_port_6_pin_0_HSIOM P6_0_SCB3_SPI_MOSI
#define ioss_0_port_6_pin_4_HSIOM P6_4_CPUSS_SWJ_SWO_TDO
#define ioss_0_port_6_pin_6_HSIOM P6_6_CPUSS_SWJ_SWDIO_TMS
#define ioss_0_port_6_pin_7_HSIOM P6_7_CPUSS_SWJ_SWCLK_TCLK
#define ioss_0_port_8_pin_0_HSIOM P8_0_SCB4_SPI_MOSI
#define ioss_0_port_9_pin_0_HSIOM P9_0_SCB2_SPI_MOSI

#define WS0_DMA_tr_in_0_TRIGGER_OUT TRIG_OUT_1TO1_0_SCB0_TX_TO_PDMA0_TR_IN16
#define WS0_SPI_tr_tx_req_0_TRIGGER_IN TRIG_IN_1TO1_0_SCB0_TX_TO_PDMA0_TR_IN16
#define WS1_DMA_tr_in_0_TRIGGER_OUT TRIG_OUT_1TO1_0_SCB1_TX_TO_PDMA0_TR_IN18
#define WS1_SPI_tr_tx_req_0_TRIGGER_IN TRIG_IN_1TO1_0_SCB1_TX_TO_PDMA0_TR_IN18
#define WS2_DMA_tr_in_0_TRIGGER_OUT TRIG_OUT_1TO1_0_SCB2_TX_TO_PDMA0_TR_IN20
#define WS2_SPI_tr_tx_req_0_TRIGGER_IN TRIG_IN_1TO1_0_SCB2_TX_TO_PDMA0_TR_IN20
#define WS3_DMA_tr_in_0_TRIGGER_OUT TRIG_OUT_1TO1_0_SCB3_TX_TO_PDMA0_TR_IN22
#define WS3_SPI_tr_tx_req_0_TRIGGER_IN TRIG_IN_1TO1_0_SCB3_TX_TO_PDMA0_TR_IN22
#define WS4_DMA_tr_in_0_TRIGGER_OUT TRIG_OUT_1TO1_0_SCB4_TX_TO_PDMA0_TR_IN24
#define WS4_SPI_tr_tx_req_0_TRIGGER_IN TRIG_IN_1TO1_0_SCB4_TX_TO_PDMA0_TR_IN24

#if defined(__cplusplus)
}
#endif


#endif /* CYCFG_ROUTING_H */
