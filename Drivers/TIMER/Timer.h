#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"

extern u16 USART2_RX_STA;
extern u16 USART1_RX_STA;
extern u32 lwip_time; //��ϵͳ�����ͼ�ʱ��ʱ�䣬ÿ10�����1

void TIM3_Int_Init(void);
//����TIM4�Ŀ���
//sta:0���ر�;1,����;
void TIM4_Set(u8 sta);

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM4_Init(u16 arr,u16 psc);

void TIM2_Init(u16 arr,u16 psc);		//10ms�ж�
void TIM2_Set(u8 sta);			//�رն�ʱ��4
#endif
