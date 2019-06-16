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

#define WS_DMA_HW DW0
#define WS_DMA_CHANNEL 16
#define WS_DMA_IRQ cpuss_interrupts_dw0_16_IRQn

extern const cy_stc_dma_descriptor_config_t WS_DMA_Descriptor_0_config;
extern const cy_stc_dma_descriptor_config_t WS_DMA_Descriptor_1_config;
extern const cy_stc_dma_descriptor_config_t WS_DMA_Descriptor_2_config;
extern const cy_stc_dma_descriptor_config_t WS_DMA_Descriptor_3_config;
extern cy_stc_dma_descriptor_t WS_DMA_Descriptor_0;
extern cy_stc_dma_descriptor_t WS_DMA_Descriptor_1;
extern cy_stc_dma_descriptor_t WS_DMA_Descriptor_2;
extern cy_stc_dma_descriptor_t WS_DMA_Descriptor_3;
extern const cy_stc_dma_channel_config_t WS_DMA_channelConfig;
extern const cy_stc_dma_crc_config_t WS_DMA_crcConfig;


#if defined(__cplusplus)
}
#endif


#endif /* CYCFG_DMAS_H */
