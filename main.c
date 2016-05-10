/***********************BSP**************************/
#include "led.h"
#include "NVIC.h"
#include "usart1.h"
#include "usart2.h"
#include "Timer.h"
#include "string.h"
#include "key.h"
#include "delay.h"

/**********************LWIP***********************/
#include "ppp.h"
#include "init.h"
#include "lwip/inet.h"
#include "tcp.h"

/******************UC/OS**************************/
//#include "usos-ii.h"


#define LWIP_DEBUG

//AT���
 char AT_AT[]="AT\r\n";
 char AT_CGCLASS[]="AT+CGCLASS=\"B\"\r\n";
 char AT_CGDOCNT[]="AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n";
 char AT_ATD[]="ATD*99#\r\n";
 char PPP_LCP_RE[]={0X7E,0XFF,0X7D,0X23,0XC0,0X21,0X7D,0X21,0X7D,0X21,0X7D,\
										0X20,0X7D,0X2A,0X7D,0X22,0X7D,0X26,0X7D,0X20,0X7D,0X2A,\
										0X7D,0X20,0X7D,0X20,0X22,0X7D,0X28,0X7E};
 
 char PPP_LCP_ACK[]={0X7E,0XFF,0X7D,0X23,0XC0,0X21,0X7D,0X22,0X7D,0X21,0X7D,\
										 0X2E,0X7D,0X22,0X7D,0X26,0X7D,0X20,0X7D,0X2A,0X7D,0X20,\
										 0X7D,0X20,0X7D,0X23,0X7D,0X24,0XC0,0X23,0XCD,0XCE,0X7E};

char PPP_IPCP_REQ[]={0X7E,0XFF,0X03,0X80,0X21,0X01,0X02,0X00,0X0A,0X03,0X06,\
											0X00,0X00,0X00,0X00,0X14,0XFE,0X7E};
										 

const static char TCP_TestData[]="This is LwIP TCP Client test\r\n";		

//PPP�ص�����											
static void linkStatusCB(void* ctx, int errCode, void* arg);				
void TCP_Client_Init();
				

	unsigned int usart2_flag=0;
	int pd; //PPP���Ӿ��
	unsigned int LED1_flag=0;
										 
 int main(void)
 {	
	sio_fd_t usart1; 
	int connected = 0;
	volatile int setup = 0;
	
	 

	delay_init(72);
	NVIC_Configuration();// �����ж����ȼ�����
	usart1_init(9600);	 //���ڳ�ʼ��Ϊ9600
	usart2_init(115200);  //usart2�ĳ�ʼ�������˶�ʱ��TIM4�ĳ�ʼ��
	LED_Init();		  	 //��ʼ����LED���ӵ�Ӳ���ӿ� 
	KEY_Init();          	//��ʼ���밴�����ӵ�Ӳ���ӿ�
  EXIT_init();         //��ʼ��EXIT�ⲿ�жϡ������õ�������ж�
	TIM3_Int_Init();   //10ms�ж�һ�Σ�lwip_time��1
	TIM4_Init(99,7199);		//10ms�ж�
	TIM4_Set(0);			//�رն�ʱ��4
	USART1_RX_STA=0;
	USART2_RX_STA=0;
	
 //��ʹ��pengdingλ֮ǰ�������Ҫ����Ȼ����Ī������Ĵ���CM3Ȩ��ָ�ϳϲ�����Ҳ~	 
	EXTI_ClearITPendingBit(EXTI_Line1);  
	//EXTI_ClearITPendingBit(EXTI_Line2);    //���EXIT2�ǲ����ж�����USART2��������֮�����ǻ��Զ������ж�
																					 //�����Ǻ�ĳ��IO�ڳ�ͻ��
	EXTI_ClearITPendingBit(EXTI_Line3);
	EXTI_ClearITPendingBit(EXTI_Line4);
	
	lwip_init();
	pppInit();
	
	

		while(KEY_Scan(0)!=KEY0_PRES); 
		delay_ms(2000);
		USART2_SEND(AT_AT,strlen(AT_AT));
		delay_ms(200);   //200ms�ȽϺ��� ̫��������
		USART2_SEND(AT_AT,strlen(AT_AT));
		delay_ms(200);
		USART2_SEND(AT_CGCLASS,strlen(AT_CGCLASS));
		delay_ms(200);
		USART2_SEND(AT_CGDOCNT,strlen(AT_CGDOCNT));
		delay_ms(200);
		USART2_SEND(AT_ATD,strlen(AT_ATD));
		delay_ms(300);

	
		//����֮�����PPP����
		while(KEY_Scan(0)!=KEY0_PRES);  
		delay_ms(1000);	
		usart2_flag=1; //USART2������ͬʱ����PC��LWIPЭ��ջ
		pppSetAuth(PPPAUTHTYPE_ANY,NULL,NULL);
		pd=pppOverSerialOpen(usart1, linkStatusCB, &connected);
		
		
		delay_ms(300);
		
    if (pd >= 0) {
			
      // the thread was successfully started.
      while (!connected ) 
			{
				//waitiing;
			}
			TCP_Client_Init();
			while(1)
			{
				/* create some socket connections, 
         * do writes and reads on the sockets, close them, etc */
			}
        
     
				
		  /* calling pppClose will end the pppInputThread associated with pd*/
      pppClose(pd);
    }
}
 

//PPP���ӳɹ��Ļص�����
static void linkStatusCB(void *ctx, int errCode, void *arg) {
   int *connected = (int *) ctx;

   struct ppp_addrs *addrs = arg;
	 addrs=addrs;
	 *connected = 1;

   if (errCode ==PPPERR_NONE) {
       /* We are connected */
       *connected = 1;
				LED1_flag=1;
//      syslog(LOG_DEBUG, "ip_addr = %s", inet_ntoa(addrs->our_ipaddr));
//       syslog(LOG_DEBUG, "netmask = %s", inet_ntoa(addrs->netmask));
//       syslog(LOG_DEBUG, "dns1    = %s", inet_ntoa(addrs->dns1));
//       syslog(LOG_DEBUG, "dns2    = %s", inet_ntoa(addrs->dns2));
   } else {
       /* We have lost connection */
   }
}

//TCP���ӳɹ��Ļص�����
err_t TcpCli_Connected(void *arg,struct tcp_pcb *pcb,err_t err)
{
tcp_write(pcb,TCP_TestData,sizeof(TCP_TestData),0); 
tcp_close(pcb);
return ERR_OK;
}

void TCP_Client_Init()
{
struct tcp_pcb *Clipcb;
struct ip_addr ipaddr;
IP4_ADDR(&ipaddr,121,42,49,56); // ��Զ������IP
Clipcb = tcp_new();  //����ͨ�ŵ�TCP���ƿ�
tcp_bind(Clipcb,IP_ADDR_ANY,8000);  //�󶨱��ص�ַ�Ͷ˿ں�
tcp_connect(Clipcb,&ipaddr,8080,TcpCli_Connected);
}


