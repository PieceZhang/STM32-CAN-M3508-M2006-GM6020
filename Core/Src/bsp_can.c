/******************************************************************************
/// @brief
/// @copyright Copyright (c) 2017 <dji-innovations, Corp. RM Dept.>
/// @license MIT License
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction,including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense,and/or sell
/// copies of the Software, and to permit persons to whom the Software is furnished
/// to do so,subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
/// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
*******************************************************************************/

#include "can.h"
#include "bsp_can.h"

// list of 4, store the information (in structure 'moto_measure_t') of 4 motors respectively
moto_measure_t moto_chassis[4] = {0};


/*******************************************************************************************
  * @Func		my_can_filter_init
  * @Brief    CAN1和CAN2滤波器配置
  * @Param		CAN_HandleTypeDef* hcan
  * @Retval		None
  * @Date     2015/11/30
 *******************************************************************************************/
void my_can_filter_init_recv_all(CAN_HandleTypeDef* _hcan)
{
	//can1 &can2 use same filter config
	CAN_FilterTypeDef		CAN_FilterConfigStructure;

//	CAN_FilterConfigStructure.FilterNumber = 0;
	CAN_FilterConfigStructure.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterConfigStructure.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterConfigStructure.FilterIdHigh = 0x0000;
	CAN_FilterConfigStructure.FilterIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdHigh = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterConfigStructure.FilterBank = 14;//can1(0-13)和can2(14-27)分别得到一半的filter
	CAN_FilterConfigStructure.FilterActivation = ENABLE;

	if(HAL_CAN_ConfigFilter(_hcan, &CAN_FilterConfigStructure) != HAL_OK)
	{
//		while(1); //show error!
	}
}

uint32_t FlashTimer;
/*******************************************************************************************
  * @Func			void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* _hcan)
  * @Brief    HAL库中标准的CAN接收完成回调函数，需要在此处理通过CAN总线接收到的数据
  * @Param		
  * @Retval		None 
  * @Date     2015/11/24
 *******************************************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
	CAN_RxHeaderTypeDef		Rx1Message;
	uint8_t Data[8];
	uint8_t MotorID;
	
	Rx1Message.StdId = 0x201;  // 0x200 + CANID    0x200?
	Rx1Message.ExtId = 0;  // CAN_ID_STD, ExtId is invalid
	Rx1Message.IDE = CAN_ID_STD;
	Rx1Message.RTR = CAN_RTR_DATA;
	Rx1Message.DLC = 0x08;
	
	if(HAL_GetTick() - FlashTimer>500)
	{
		HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
		FlashTimer = HAL_GetTick();
	}

	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &Rx1Message, Data);
	
	switch(Rx1Message.StdId)
	{
		case CAN_2006Moto1_ID: MotorID=0; break;
		case CAN_2006Moto2_ID: MotorID=1; break;
		case CAN_2006Moto3_ID: MotorID=2; break;
		case CAN_2006Moto4_ID: MotorID=3; break;
	}
		
	
//	get_moto_measure(&moto_chassis[MotorID], Data);
	/* get_moto_measure (1 motor) */
	moto_chassis[MotorID].last_angle = moto_chassis[MotorID].angle;
	moto_chassis[MotorID].angle = (uint16_t)(Data[0]<<8 | Data[1]) ;
	moto_chassis[MotorID].speed_rpm  = (int16_t)(Data[2]<<8 | Data[3]);
	moto_chassis[MotorID].real_current = (Data[4]<<8 | Data[5])*5.f/16384.f;
	moto_chassis[MotorID].hall = Data[6];
	if(moto_chassis[MotorID].angle - moto_chassis[MotorID].last_angle > 4096)
		moto_chassis[MotorID].round_cnt --;
	else if (moto_chassis[MotorID].angle - moto_chassis[MotorID].last_angle < -4096)
		moto_chassis[MotorID].round_cnt ++;
	moto_chassis[MotorID].total_angle = moto_chassis[MotorID].round_cnt * 8192 + moto_chassis[MotorID].angle - moto_chassis[MotorID].offset_angle;
	
	
	/*#### add enable can it again to solve can receive only one ID problem!!!####**/
	__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);


}

/*******************************************************************************************
  * @Func			void get_moto_measure(moto_measure_t *ptr, CAN_HandleTypeDef* hcan)
  * @Brief    接收3508电机通过CAN发过来的信息
  * @Param		
  * @Retval		None
  * @Date     2015/11/24
 *******************************************************************************************/
//void get_moto_measure(moto_measure_t *ptr, uint8_t Data[])
//{
//	ptr->last_angle = ptr->angle;
//	ptr->angle = (uint16_t)(Data[0]<<8 | Data[1]) ;
//	ptr->speed_rpm  = (int16_t)(Data[2]<<8 | Data[3]);
//	ptr->real_current = (Data[4]<<8 | Data[5])*5.f/16384.f;
//	ptr->hall = Data[6];
//	
//	if(ptr->angle - ptr->last_angle > 4096)
//		ptr->round_cnt --;
//	else if (ptr->angle - ptr->last_angle < -4096)
//		ptr->round_cnt ++;
//	ptr->total_angle = ptr->round_cnt * 8192 + ptr->angle - ptr->offset_angle;
//}


/*this function should be called after system+can init */
//void get_moto_offset(moto_measure_t *ptr, CAN_HandleTypeDef* hcan)
//{
//	ptr->angle = (uint16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1]) ;
//	ptr->offset_angle = ptr->angle;
//}


#define ABS(x)	( (x>0) ? (x) : (-x) )
/**
*@bref 电机上电角度=0， 之后用这个函数更新3510电机的相对开机后（为0）的相对角度。
	*/
//void get_total_angle(moto_measure_t *p){
//	
//	int res1, res2, delta;
//	if(p->angle < p->last_angle){			//可能的情况
//		res1 = p->angle + 8192 - p->last_angle;	//正转，delta=+
//		res2 = p->angle - p->last_angle;				//反转	delta=-
//	}else{	//angle > last
//		res1 = p->angle - 8192 - p->last_angle ;//反转	delta -
//		res2 = p->angle - p->last_angle;				//正转	delta +
//	}
//	//不管正反转，肯定是转的角度小的那个是真的
//	if(ABS(res1)<ABS(res2))
//		delta = res1;
//	else
//		delta = res2;

//	p->total_angle += delta;
//	p->last_angle = p->angle;
//}


void set_moto_current(CAN_HandleTypeDef* hcan, s16 iq1, s16 iq2, s16 iq3, s16 iq4)
{
	CAN_TxHeaderTypeDef CanTx;
	uint8_t Data[8];
	uint32_t Mailbox=0;
	
	CanTx.StdId = 0x201;  // 0x200 + CANID    0x200?
	CanTx.ExtId = 0;  // CAN_ID_STD, ExtId is invalid
	CanTx.IDE = CAN_ID_STD;
	CanTx.RTR = CAN_RTR_DATA;
	CanTx.DLC = 0x08;
	CanTx.TransmitGlobalTime = DISABLE;
	
	Data[0] = (iq1 >> 8);
	Data[1] = iq1;
	Data[2] = (iq2 >> 8);
	Data[3] = iq2;
	Data[4] = iq3 >> 8;
	Data[5] = iq3;
	Data[6] = iq4 >> 8;
	Data[7] = iq4;
	
	HAL_CAN_AddTxMessage(hcan, &CanTx, Data, &Mailbox);
}	
