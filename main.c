
// 通用定时器TIMx,x[2,3,4,5]定时应用
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_TiMbase.h"
#include "bsp_exti.h"
#include "bsp_lcd.h"
#include "hcsr.h"
#include <stdio.h>

#define CLI() __set_PRIMASK(1)				/* 关总中断 */  
#define SEI() __set_PRIMASK(0)				/* 开总中断 */ 

volatile u32 triger_time = 0; // 10us triger计时变量,>10us
volatile u32 echo_time = 0;   // 10us echo计时变量, =距离*2/声速
u32 distance_time = 0;				// 10us 记录echo高电平时间

uint8_t echo_flag = 0;				// echo_flag=1 ->接收到回波信号


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	float height = 0;
	char cStr[10];
	
	/* led 端口配置 */ 
	LED_GPIO_Config();
	HCSR_GPIO_Config();
	//CLI();
	//SEI();
	
	/* exti line config */
	EXTI_Pxy_Config(); 		//配置PB9为上升沿中断，PB9->Echo

	/* 通用定时器 TIMx,x[2,3,4,5] 定时配置 */	
  TIMx_Configuration();
	
	/* 配置通用定时器 TIMx,x[2,3,4,5]的中断优先级 */
	TIMx_NVIC_Configuration();
	
	LCD_Init ();         //LCD ???
	
  ILI9341_GramScan ( 2 );
  ILI9341_Clear ( 0, 0, 320, 240, macBACKGROUND);
	ILI9341_DispChar_EN ( 60, 60, 'A', macBACKGROUND, macRED );
	
	macTIM_APBxClock_FUN (macTIM_CLK, ENABLE);
	
  while(1)
  {
		Triger();
		//LED1_ON;LED3_OFF;
		
		echo_time = 0;
		
		echo_flag = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
		while(echo_flag == 0)				//等待收到回波信号
		{
			//LED1_OFF;LED2_ON;
			echo_flag = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
		}
		echo_time = 0;
		while(echo_flag == 1)						//等待回波信号接收完成
		{
			//LED2_OFF;LED3_ON;
			echo_flag = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
		}
		distance_time = echo_time;
		//macTIM_APBxClock_FUN (macTIM_CLK, DISABLE);
		height = Calculation();
		sprintf(cStr,"%0.4d",distance_time);
		ILI9341_DispString_EN ( 120, 120, cStr, macBACKGROUND, macRED );
		sprintf(cStr,"%0.4f",height);
		ILI9341_DispString_EN ( 120, 140, cStr, macBACKGROUND, macRED );
		
  }
}
/*********************************************END OF FILE**********************/

