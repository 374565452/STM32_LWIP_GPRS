#include "NVIC.h"
#include "usart1.h"
#include "usart2.h"
#include "led.h"
#include "delay.h"
#include "stm32f10x.h"
#include "ppp.h"

extern unsigned char AT_AT;
extern int pd;  //��main����������
int time=0;

void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//2bit��ռ���ȼ���2bit�����ȼ�

}

//����ж� ʹ���ж�4
//���жϺ������ڽ�����ͨ��USART2���͵�SIM900ģ��
void EXTI4_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)   
    {
				EXTI_ClearITPendingBit(EXTI_Line4);
				pppos_input(pd,USART2_RX_BUF,USART2_RX_STA_BACKUP&0x3fff);
		}
}


//LED��˸
void EXTI3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line3);
			//	LED0=!LED0;
    }
}


//void EXTI2_IRQHandler(void)
//{
//    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
//    {
//            EXTI_ClearITPendingBit(EXTI_Line2);
//    }
////		LED0=!LED0;
//}


//USART1��PC��������
void EXTI1_IRQHandler(void)
{
			u8 t;
	//LED1=!LED1;
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)   
    {
			EXTI_ClearITPendingBit(EXTI_Line1);  //�������λ
			
			//��������
			//p_usart1_send_data=&AT_AT;
			p_usart1_send_data=USART2_RX_BUF;
			USART1_SEND_LEN=USART2_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			for(t=0;t<USART1_SEND_LEN;t++)
			{
				USART1->DR=*p_usart1_send_data;
				p_usart1_send_data++;
				while((USART1->SR&0X40)==0);//�ȴ����ͽ���
			}			
			USART2_RX_STA=0;
			USART1_SEND_LEN=0;
			p_usart1_send_data=NULL;
		}	
	
}

//����ж����ú���
void EXIT_init()
{
  EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//��������ж�
  EXTI_InitStructure.EXTI_Line=EXTI_Line4|EXTI_Line3|EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd=ENABLE;
  EXTI_Init(&EXTI_InitStructure);  	
	
 
	
  
 //�Ѿ�����Ϊ2bit��ռ���ȼ���2bit�����ȼ�
  NVIC_InitStructure.NVIC_IRQChannel=EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  NVIC_InitStructure.NVIC_IRQChannel=EXTI3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel=EXTI2_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
//  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

