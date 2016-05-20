#include "hcsr.h"
#include "bsp_led.h"

/* 				variable					*/
extern volatile u32 triger_time;
extern volatile u32 echo_time; 

float distance;

/*				function					*/
void HCSR_GPIO_Config()
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启GPIOB和GPIOF的外设时钟*/
		RCC_APB2PeriphClockCmd( RCC_HCSRGPIO_CLK, ENABLE); 

		/*选择要控制的GPIOB引脚*/															   
		GPIO_InitStructure.GPIO_Pin = HCSR_Triger_Pin;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   	//设为推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(HCSR_Port, &GPIO_InitStructure);	
		
}

void Triger()
{
	macTIM_APBxClock_FUN (macTIM_CLK, ENABLE);
	triger_time = 0;
	LED2_ON;
	GPIO_SetBits(HCSR_Port, HCSR_Triger_Pin);
	while(triger_time<2)				//定时20us
	{
		;
	}
	GPIO_ResetBits(HCSR_Port,HCSR_Triger_Pin);
	macTIM_APBxClock_FUN (macTIM_CLK, ENABLE);
}

float Calculation()
{
	if(echo_time>12&&echo_time<900)				//测量精度2cm以上，小于1.5m表示有身高
	{
		//distance = 2.0 - echo_time/1000000 * 340.0 / 2;
		distance = echo_time/1000000 * 340.0 / 2;
	}
	else
	{
		distance = 0;
	}
	
	return distance;
}
