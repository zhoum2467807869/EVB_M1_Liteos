/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#include "nb_iot_demo.h"
#include "sys_init.h"
#if defined USE_NB_NEUL95
#include "nb_iot/los_nb_api.h"
#include "at_device/bc95.h"
#endif

msg_for_SMOKE   SMOKE_send;

VOID data_collection_task(VOID)
{
	UINT32 uwRet = LOS_OK;
	
	short int Value;
	short int MaxValue=1000;
	MX_ADC1_Init();								       	//初始化传感器
	OLED_ShowString(30,2,(uint8_t *)"Smoke:",16);
	while (1)
	{

		printf("This is data_collection_task !\r\n");

		SMOKE_send.CSQ=nb_get_csq();                  	//读取信号指令
		printf("\r\n++++++++++++++++++++++CSQ is  %d\r\n",SMOKE_send.CSQ);

		HAL_ADC_Start(&hadc1);	
		HAL_ADC_PollForConversion(&hadc1, 50);
		Value = HAL_ADC_GetValue(&hadc1);				//读取烟雾浓度
		printf("\r\n******************************MQ2 Value is  %d\r\n",Value);
		OLED_ShowNum(72,2,Value,sizeof(Value)+1,16);

		sprintf(SMOKE_send.Value, "%4d", Value);
		sprintf(SMOKE_send.MaxValue, "%4d", MaxValue);	

		uwRet=LOS_TaskDelay(1000);
		if(uwRet !=LOS_OK)
		return;
	
	}
}
UINT32 creat_data_collection_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;
	UINT32 TskHandle;
    task_init_param.usTaskPrio = 0;
    task_init_param.pcName = "data_collection_task";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)data_collection_task;
    task_init_param.uwStackSize = 0x800;

    uwRet = LOS_TaskCreate(&TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}

VOID data_report_task(VOID)
{
	UINT32 uwRet = LOS_OK;

	while(1)
	{

		if(los_nb_report((const char*)(&SMOKE_send),sizeof(SMOKE_send))>=0)	//发送数据到平台	
				printf("ocean_send_data OK!\n");                                              
		else                                                                                  
			{
				printf("ocean_send_data Fail!\n"); 
			}

		uwRet=LOS_TaskDelay(Updateperiod);
		if(uwRet !=LOS_OK)
		return;
	
	}
}
UINT32 creat_data_report_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;
	  UINT32 TskHandle;

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "data_report_task";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)data_report_task;
    task_init_param.uwStackSize = 0x400;
    uwRet = LOS_TaskCreate(&TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}



void nb_iot_entry(void)
{
    UINT32 uwRet = LOS_OK;

    uwRet = creat_data_collection_task();
    if (uwRet != LOS_OK)
    {
        return ;
    }
		
		uwRet = creat_data_report_task();
    if (uwRet != LOS_OK)
    {
        return ;
    }
}
