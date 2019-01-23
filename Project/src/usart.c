/*
 * =====================================================================================
 *
 *       Filename:  uart.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013/10/2 18:14:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  madcc09 (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stm32f4xx.h>
#include <stm32f4xx_usart.h>
#include <stdio.h>
#include "usart.h"

//���Ҫ�޸����������ڣ��ǵ��޸ģ�
//IOʱ��ʹ�ܣ�RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//����ʱ��ʹ�ܣ�RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
//ע�ⲻͬ���ڵ� �������������ܲ�ͬ������ APB1,APB2 �ȡ� 
//���ù��ܣ� GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
//USART_Init(USART6, &USART_InitStructure);
//USART_Cmd(USART6, ENABLE);
//fputc,fgetc �У� USART_SendData(USART6, (u8) ch);��
//���ʹ���˴����жϣ���Ҫ�޸�NVIC�е��ж�����
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Enable USART6 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Configure port as pushpull, 50MHz and No pull up & down  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	/* USART6 configured as follow:
	   - BaudRate = 115200 baud  
	   - Word Length = 8 Bits
	   - One Stop Bit
	   - No parity
	   - Hardware flow control disabled (RTS and CTS signals)
	   - Receive and transmit enabled
	   */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;


	/* USART configuration */
	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	/* Enable USART */
	USART_Cmd(USART2, ENABLE);

}
void USART2_IRQHandler(void)
{ 
	unsigned int i; 

	if(USART_GetFlagStatus(USART2,USART_IT_RXNE)==SET) 
	{               
//		if(USART_GetFlagStatus(USART2,USART_FLAG_ORE)==SET) //���-������������Ҫ�ȶ�SR,�ٶ�DR�Ĵ��� �������������жϵ����� 
//		{ 
//			USART_ClearFlag(USART2,USART_FLAG_ORE);  //�� SR 
//			i=USART_ReceiveData(USART2);       //�� DR 
//			USART_ITConfig(USART2,USART_IT_TXE, DISABLE);
//			USART_ITConfig(USART2,USART_IT_TXE, ENABLE);
//			while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET) ; 
//
//		} 

		i = USART_ReceiveData(USART2); 
		USART_SendData(USART2,i); 
		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
		{ 
			/* ��������жϱ�־*/ 
			USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		}        
	}

	if(USART_GetFlagStatus(USART2,USART_IT_TC)==SET)
	{
		if(USART_GetITStatus(USART2, USART_IT_TC) != RESET) 
		{ 
			/* ��������жϱ�־*/ 
			USART_ClearITPendingBit(USART2, USART_IT_TC);
		}
	}
//	//���ʹ����USART_IT_TXE���ǵü��������⼸�䣬��������жϻ�һֱ������
//	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)  //��������ж�
//	{
//		USART_ClearITPendingBit(USART2, USART_IT_TXE);  //����жϱ�־
//		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);    //ʧ�ܷ����ж�
//	}	
}
/*******************************************************************************
* Function Name  : int fputc(int ch, FILE *f)
* Description    : Retargets the C library printf function to the USART.printf�ض���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int fputc(int ch, FILE *f)
{
  /* Write a character to the USART */
  USART_SendData(USART2, (u8) ch);

  /* Loop until the end of transmission */
  while(!(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == SET))
  {
  }

  return ch;
}

/*******************************************************************************
* Function Name  : int fgetc(FILE *f)
* Description    : Retargets the C library printf function to the USART.fgetc�ض���
* Input          : None
* Output         : None
* Return         : ��ȡ�����ַ�
*******************************************************************************/
int fgetc(FILE *f)
{
  /* Loop until received a char */
  while(!(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET))
  {
  }
  
    /* Read a character from the USART and RETURN */
  return (USART_ReceiveData(USART2));
}
