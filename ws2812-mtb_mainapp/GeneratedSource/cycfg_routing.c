/*******************************************************************************
* File Name: cycfg_routing.c
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

#include "cycfg_routing.h"

#include "cy_trigmux.h"

#include "stdbool.h"

void init_cycfg_routing(void)
{
	Cy_TrigMux_Select(TRIG_OUT_1TO1_0_SCB0_TX_TO_PDMA0_TR_IN16, false, TRIGGER_TYPE_LEVEL);
	Cy_TrigMux_Select(TRIG_OUT_1TO1_0_SCB1_TX_TO_PDMA0_TR_IN18, false, TRIGGER_TYPE_LEVEL);
	Cy_TrigMux_Select(TRIG_OUT_1TO1_0_SCB2_TX_TO_PDMA0_TR_IN20, false, TRIGGER_TYPE_LEVEL);
	Cy_TrigMux_Select(TRIG_OUT_1TO1_0_SCB3_TX_TO_PDMA0_TR_IN22, false, TRIGGER_TYPE_LEVEL);
	Cy_TrigMux_Select(TRIG_OUT_1TO1_0_SCB4_TX_TO_PDMA0_TR_IN24, false, TRIGGER_TYPE_LEVEL);
}
