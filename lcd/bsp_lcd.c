#include "bsp_lcd.h"



/**
  * @brief  LCD ��ʼ������
  * @param  ��
  * @retval ��
  */
void LCD_Init ( void )
{
	ILI9341_Init ();
	
	#if macLCD_XPT2046_ENABLE
	
	  XPT2046_Init ();
	
	#endif
	
}


