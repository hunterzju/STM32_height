#ifndef HCSR_H
#define HCSR_H

#include "stm32f10x.h"
#include "bsp_TiMbase.h"

#define 		RCC_HCSRGPIO_CLK 		RCC_APB2Periph_GPIOB			//HCSR4��PortB
#define     HCSR_Port						GPIOB
#define     HCSR_Triger_Pin			GPIO_Pin_8								//Triger Pin->PB8
#define     HCSR_Echo_Pin				GPIO_Pin_9								//Echo Pin->PB9

void HCSR_GPIO_Config(void);						//GPIO����
void Triger(void);						//���Ͳ����ź�
float Calculation(void);			//������뺯��


#endif
