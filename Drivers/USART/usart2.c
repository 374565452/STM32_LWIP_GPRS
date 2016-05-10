#include "Timer.h"
#include "usart2.h" 
#include "stm32f10x.h"
#include "string.h"
#include "led.h"
#include "delay.h"
 

//���ڷ��ͻ����� 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
 	  
//���ڽ��ջ����� 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
u8 USART2_SEND_LEN;
unsigned  char * p_usart2_send_data=NULL;



//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u16 USART2_RX_STA=0;   	 
u16 USART2_RX_STA_BACKUP=0;   	

//USART2�жϴ����� �������ǹ涨�õģ������Ը���
void USART2_IRQHandler(void)
{
	u8 res;	
	if(USART2->SR&(1<<5))//���յ�����  λ5��RXNE����־���Ƿ��������յ�
	{	 
		res=USART2->DR; 			 
		if(USART2_RX_STA<USART2_MAX_RECV_LEN)		//�����Խ�������
		{
			TIM4->CNT=0;         					//���������
			if(USART2_RX_STA==0) TIM4_Set(1);	 	//ʹ�ܶ�ʱ��4���ж� 
			USART2_RX_BUF[USART2_RX_STA++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			USART2_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
			//EXTI_GenerateSWInterrupt(EXTI_Line1);  //��������ж�
		} 
	}  											 
}


//USART2�������ݺ���
void USART2_SEND( char *p_data, unsigned int len)
{
	u8 t;
	char *pdata;
	pdata=p_data;
	for(t=0;t<len;t++)
	{
			USART2->DR=*pdata;
			pdata++;
			while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//��ʼ��IO ����2
//
//bound:������	  
void usart2_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	 
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	  USART_DeInit(USART2);  //��λ����1
	 //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3

   //Usart2 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�2
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�2
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	  NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	 USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	 USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	 USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

   USART_Init(USART2, &USART_InitStructure); //��ʼ������
   USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
   USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
		
		
}


