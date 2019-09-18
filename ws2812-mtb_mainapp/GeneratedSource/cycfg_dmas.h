/*******************************************************************************
* File Name: cycfg_dmas.h
*
* Description:
* DMA configuration
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

#if !defined(CYCFG_DMAS_H)
#define CYCFG_DMAS_H

#include "cycfg_notices.h"
#include "cy_dma.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define cpuss_0_dw1_0_chan_12_HW DW1
#define cpuss_0_dw1_0_chan_12_CHANNEL 12
#define cpuss_0_dw1_0_chan_12_IRQ cpuss_interrupts_dw1_12_IRQn
#define cpuss_0_dw1_0_chan_8_HW DW1
#define cpuss_0_dw1_0_chan_8_CHANNEL 8
#define cpuss_0_dw1_0_chan_8_IRQ cpuss_interrupts_dw1_8_IRQn

extern const cy_stc_dma_descriptor_config_t cpuss_0_dw1_0_chan_12_Descriptor_0_config;
extern const cy_stc_dma_descriptor_config_t cpuss_0_dw1_0_chan_12_Descriptor_1_config;
extern const cy_stc_dma_descriptor_config_t cpuss_0_dw1_0_chan_12_Descriptor_2_config;
extern const cy_stc_dma_descriptor_config_t cpuss_0_dw1_0_chan_12_Descriptor_3_config;
extern cy_stc_dma_descriptor_t cpuss_0_dw1_0_chan_12_Descriptor_0;
extern cy_stc_dma_descriptor_t cpuss_0_dw1_0_chan_12_Descriptor_1;
extern cy_stc_dma_descriptor_t cpuss_0_dw1_0_chan_12_Descriptor_2;
extern cy_stc_dma_descriptor_t cpuss_0_dw1_0_chan_12_Descriptor_3;
extern const cy_stc_dma_channel_config_t cpuss_0_dw1_0_chan_12_channelConfig;
extern const cy_stc_dma_crc_config_t cpuss_0_dw1_0_chan_12_crcConfig;
extern const cy_stc_dma_descriptor_config_t cpuss_0_dw1_0_chan_8_Descriptor_0_config;
extern const cy_stc_dma_descriptor_config_t cpuss_0_dw1_0_chan_8_Descriptor_1_config;
extern const cy_stc_dma_descriptor_config_t cpuss_0_dw1_0_chan_8_Descriptor_2_config;
extern const cy_stc_dma_descriptor_config_t cpuss_0_dw1_0_chan_8_Descriptor_3_config;
extern cy_stc_dma_descriptor_t cpuss_0_dw1_0_chan_8_Descriptor_0;
extern cy_stc_dma_descriptor_t cpuss_0_dw1_0_chan_8_Descriptor_1;
extern cy_stc_dma_descriptor_t cpuss_0_dw1_0_chan_8_Descriptor_2;
extern cy_stc_dma_descriptor_t cpuss_0_dw1_0_chan_8_Descriptor_3;
extern const cy_stc_dma_channel_config_t cpuss_0_dw1_0_chan_8_channelConfig;
extern const cy_stc_dma_crc_config_t cpuss_0_dw1_0_chan_8_crcConfig;


#if defined(__cplusplus)
}
#endif


#endif /* CYCFG_DMAS_H */
