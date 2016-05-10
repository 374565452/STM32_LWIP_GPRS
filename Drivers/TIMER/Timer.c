#include "Timer.h"
#include "led.h"
#include "usart2.h"
#include <stdio.h>

extern int pd;
extern unsigned int usart2_flag;
extern unsigned int LED1_flag;

u32 len,t;
int ttt=0;
u32 lwip_time=0; //��ϵͳ�����ͼ�ʱ��ʱ�䣬ÿ10�����1


//��ʱ��3�жϷ������		    
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
			lwip_time++;
			if(lwip_time%100==0)
			{
				LED0=!LED0;
				if(LED1_flag==1)
				{	
					LED1=!LED1;
				}
			}
		}
}

//��ʱ��4�жϷ������		    
void TIM4_IRQHandler(void)
{ 	
	if(TIM4->SR&0X01)//�Ǹ����ж�
	{	 			   
		if(USART2_RX_STA!=0)     //˵����USART2�ڽ�������
		{
			USART2_RX_STA|=1<<15;	//��ǽ������
			USART2_RX_STA_BACKUP=USART2_RX_STA;
			TIM4->SR&=~(1<<0);		//����жϱ�־λ		   
			TIM4_Set(0);			//�ر�TIM4  
			
			len=USART2_RX_STA_BACKUP&0x3fff;
			for(t=0;t<len;t++)
			{
				USART1->DR=USART2_RX_BUF[t];
				
				while((USART1->SR&0X40)==0);//�ȴ����ͽ���
			}	
			USART2_RX_STA=0;			

			if(usart2_flag==1)
			{
					EXTI_GenerateSWInterrupt(EXTI_Line4);  //��������ж� ����pppos_input��������
			}
			
		}
		else  //�������USART1�ڽ�������
		{
			USART2_RX_STA|=0;	//��ǽ������			
			TIM4->SR&=~(1<<0);		//����жϱ�־λ		   
			TIM4_Set(0);			//�ر�TIM4  
		}
	}	    
}
//����TIM4�Ŀ���
//sta:0���ر�;1,����;
void TIM4_Set(u8 sta)
{
	if(sta)
	{
    	TIM4->CNT=0;         //���������
		TIM4->CR1|=1<<0;     //ʹ�ܶ�ʱ��4
	}else TIM4->CR1&=~(1<<0);//�رն�ʱ��4	   
}
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM4_Init(u16 arr,u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC->APB1ENR|=1<<2;	//TIM4ʱ��ʹ��    
 	TIM4->ARR=arr;  	//�趨�������Զ���װֵ   
	TIM4->PSC=psc;  	//Ԥ��Ƶ��
 	TIM4->DIER|=1<<0;   //��������ж�				
 	TIM4->CR1|=0x01;  	//ʹ�ܶ�ʱ��4	  	   
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���						 
}


//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = 99; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =7199; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM3
		TIM_IT_Update ,
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
							 
}

//����TIM4�Ŀ���
//sta:0���ر�;1,����;
void TIM2_Set(u8 sta)
{
	if(sta)
	{
    	TIM2->CNT=0;         //���������
		TIM2->CR1|=1<<0;     //ʹ�ܶ�ʱ��4
	}else TIM2->CR1&=~(1<<0);//�رն�ʱ��4	   
}
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM2_Init(u16 arr,u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC->APB1ENR|=1<<2;	//TIM4ʱ��ʹ��    
 	TIM2->ARR=arr;  	//�趨�������Զ���װֵ   
	TIM2->PSC=psc;  	//Ԥ��Ƶ��
 	TIM2->DIER|=1<<0;   //��������ж�				
 	TIM2->CR1|=0x01;  	//ʹ�ܶ�ʱ��4	  	   
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���						 
}


