#include "bsp_ili9341_lcd.h"
#include "ascii.h"	



static void                   ILI9341_Delay               ( __IO uint32_t nCount );
static void                   ILI9341_GPIO_Config         ( void );
static void                   ILI9341_FSMC_Config         ( void );
static void                   ILI9341_REG_Config          ( void );
static void                   ILI9341_SetCursor           ( uint16_t usX, uint16_t usY );
static __inline void          ILI9341_FillColor           ( uint32_t ulAmout_Point, uint16_t usColor );
static uint16_t               ILI9341_Read_PixelData      ( void );



/**
  * @brief  ��ILI9341д������
  * @param  usCmd :Ҫд��������Ĵ�����ַ��
  * @retval ��
  */	
__inline void ILI9341_Write_Cmd ( uint16_t usCmd )
{
	* ( __IO uint16_t * ) ( macFSMC_Addr_ILI9341_CMD ) = usCmd;
	
}


/**
  * @brief  ��ILI9341д������
  * @param  usData :Ҫд�������
  * @retval ��
  */	
__inline void ILI9341_Write_Data ( uint16_t usData )
{
	* ( __IO uint16_t * ) ( macFSMC_Addr_ILI9341_DATA ) = usData;
	
}


/**
  * @brief  ��ILI9341��ȡ����
  * @param  ��
  * @retval ��ȡ��������
  */	
__inline uint16_t ILI9341_Read_Data ( void )
{
	return ( * ( __IO uint16_t * ) ( macFSMC_Addr_ILI9341_DATA ) );
	
}


/**
  * @brief  ���� ILI9341 ����ʱ����
  * @param  nCount ����ʱ����ֵ
  * @retval ��
  */	
static void ILI9341_Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}


/**
  * @brief  ��ʼ��ILI9341��IO����
  * @param  ��
  * @retval ��
  */
static void ILI9341_GPIO_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	/* ʹ�ܸ���IOʱ��*/
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_AFIO, ENABLE );
	
	
	/* ʹ��FSMC��Ӧ��Ӧ�ܽ�ʱ��*/
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE );
		
	
	/* ����FSMC���Ӧ��������,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10 */	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_8 | GPIO_Pin_9 | 
																GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init ( GPIOD, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
																GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
																GPIO_Pin_15;
	GPIO_Init ( GPIOE,  & GPIO_InitStructure ); 
	
	
	/* ����FSMC���Ӧ�Ŀ�����
	 * PD4-FSMC_NOE   :LCD-RD
	 * PD5-FSMC_NWE   :LCD-WR
	 * PD7-FSMC_NE1   :LCD-CS
	 * PD11-FSMC_A16  :LCD-DC
	 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_Init (GPIOD, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
	GPIO_Init (GPIOD, & GPIO_InitStructure );
	
	macILI9341_CS_APBxClock_FUN ( macILI9341_CS_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = macILI9341_CS_PIN; 
	GPIO_Init ( macILI9341_CS_PORT, & GPIO_InitStructure );  
	
	macILI9341_DC_APBxClock_FUN ( macILI9341_DC_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = macILI9341_DC_PIN; 
	GPIO_Init ( macILI9341_DC_PORT, & GPIO_InitStructure );
	

  /* ����LCD��λRST���ƹܽ�*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	macILI9341_RST_APBxClock_FUN ( macILI9341_RST_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = macILI9341_RST_PIN; 
	GPIO_Init ( macILI9341_RST_PORT, & GPIO_InitStructure );
	
	
	/* ����LCD������ƹܽ�BK*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	
	macILI9341_BK_APBxClock_FUN ( macILI9341_BK_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = macILI9341_BK_PIN; 
	GPIO_Init ( macILI9341_BK_PORT, & GPIO_InitStructure );
	

}


 /**
  * @brief  LCD  FSMC ģʽ����
  * @param  ��
  * @retval ��
  */
static void ILI9341_FSMC_Config ( void )
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  p; 
	
	
	/* ʹ��FSMCʱ��*/
	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_FSMC, ENABLE );


	p.FSMC_AddressSetupTime      = 0x02;	 //��ַ����ʱ��
	p.FSMC_AddressHoldTime       = 0x00;	 //��ַ����ʱ��
	p.FSMC_DataSetupTime         = 0x05;	 //���ݽ���ʱ��
	p.FSMC_BusTurnAroundDuration = 0x00;
	p.FSMC_CLKDivision           = 0x00;
	p.FSMC_DataLatency           = 0x00;
	p.FSMC_AccessMode            = FSMC_AccessMode_B;	 //ģʽB�Ƚ�������ILI9341
	
	FSMC_NORSRAMInitStructure.FSMC_Bank                  = macFSMC_Bank1_NORSRAMx;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
	//FSMC_NORSRAMInitStructure.FSMC_MemoryType          = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = & p;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = & p;  
	
	FSMC_NORSRAMInit ( & FSMC_NORSRAMInitStructure ); 
	
	
	/* ʹ�� FSMC_Bank1_NORSRAM4 */
	FSMC_NORSRAMCmd ( macFSMC_Bank1_NORSRAMx, ENABLE );  
		
		
}


/**
 * @brief  ��ʼ��ILI9341�Ĵ���
 * @param  ��
 * @retval ��
 */
static void ILI9341_REG_Config ( void )
{
	/*  Power control B (CFh)  */
	macDEBUG_DELAY  ();
	ILI9341_Write_Cmd ( 0xCF  );
	ILI9341_Write_Data ( 0x00  );
	ILI9341_Write_Data ( 0x81  );
	ILI9341_Write_Data ( 0x30  );
	
	/*  Power on sequence control (EDh) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xED );
	ILI9341_Write_Data ( 0x64 );
	ILI9341_Write_Data ( 0x03 );
	ILI9341_Write_Data ( 0x12 );
	ILI9341_Write_Data ( 0x81 );
	
	/*  Driver timing control A (E8h) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xE8 );
	ILI9341_Write_Data ( 0x85 );
	ILI9341_Write_Data ( 0x10 );
	ILI9341_Write_Data ( 0x78 );
	
	/*  Power control A (CBh) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xCB );
	ILI9341_Write_Data ( 0x39 );
	ILI9341_Write_Data ( 0x2C );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x34 );
	ILI9341_Write_Data ( 0x02 );
	
	/* Pump ratio control (F7h) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xF7 );
	ILI9341_Write_Data ( 0x20 );
	
	/* Driver timing control B */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xEA );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );
	
	/* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xB1 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x1B );
	
	/*  Display Function Control (B6h) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xB6 );
	ILI9341_Write_Data ( 0x0A );
	ILI9341_Write_Data ( 0xA2 );
	
	/* Power Control 1 (C0h) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xC0 );
	ILI9341_Write_Data ( 0x35 );
	
	/* Power Control 2 (C1h) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xC1 );
	ILI9341_Write_Data ( 0x11 );
	
	/* VCOM Control 1 (C5h) */
	ILI9341_Write_Cmd ( 0xC5 );
	ILI9341_Write_Data ( 0x45 );
	ILI9341_Write_Data ( 0x45 );
	
	/*  VCOM Control 2 (C7h)  */
	ILI9341_Write_Cmd ( 0xC7 );
	ILI9341_Write_Data ( 0xA2 );
	
	/* Enable 3G (F2h) */
	ILI9341_Write_Cmd ( 0xF2 );
	ILI9341_Write_Data ( 0x00 );
	
	/* Gamma Set (26h) */
	ILI9341_Write_Cmd ( 0x26 );
	ILI9341_Write_Data ( 0x01 );
	macDEBUG_DELAY ();
	
	/* Positive Gamma Correction */
	ILI9341_Write_Cmd ( 0xE0 ); //Set Gamma
	ILI9341_Write_Data ( 0x0F );
	ILI9341_Write_Data ( 0x26 );
	ILI9341_Write_Data ( 0x24 );
	ILI9341_Write_Data ( 0x0B );
	ILI9341_Write_Data ( 0x0E );
	ILI9341_Write_Data ( 0x09 );
	ILI9341_Write_Data ( 0x54 );
	ILI9341_Write_Data ( 0xA8 );
	ILI9341_Write_Data ( 0x46 );
	ILI9341_Write_Data ( 0x0C );
	ILI9341_Write_Data ( 0x17 );
	ILI9341_Write_Data ( 0x09 );
	ILI9341_Write_Data ( 0x0F );
	ILI9341_Write_Data ( 0x07 );
	ILI9341_Write_Data ( 0x00 );
	
	/* Negative Gamma Correction (E1h) */
	ILI9341_Write_Cmd ( 0XE1 ); //Set Gamma
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x19 );
	ILI9341_Write_Data ( 0x1B );
	ILI9341_Write_Data ( 0x04 );
	ILI9341_Write_Data ( 0x10 );
	ILI9341_Write_Data ( 0x07 );
	ILI9341_Write_Data ( 0x2A );
	ILI9341_Write_Data ( 0x47 );
	ILI9341_Write_Data ( 0x39 );
	ILI9341_Write_Data ( 0x03 );
	ILI9341_Write_Data ( 0x06 );
	ILI9341_Write_Data ( 0x06 );
	ILI9341_Write_Data ( 0x30 );
	ILI9341_Write_Data ( 0x38 );
	ILI9341_Write_Data ( 0x0F );
	
	/* memory access control set */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0x36 ); 	
	ILI9341_Write_Data ( 0xC8 );    /*����  ���Ͻǵ� (���)�����½� (�յ�)ɨ�跽ʽ*/
	macDEBUG_DELAY ();
	
	/* column address control set */
	ILI9341_Write_Cmd ( macCMD_SetCoordinateX ); 
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0xEF );
	
	/* page address control set */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( macCMD_SetCoordinateY ); 
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x01 );
	ILI9341_Write_Data ( 0x3F );
	
	/*  Pixel Format Set (3Ah)  */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0x3a ); 
	ILI9341_Write_Data ( 0x55 );
	
	/* Sleep Out (11h)  */
	ILI9341_Write_Cmd ( 0x11 );	
	ILI9341_Delay ( 0xAFFf<<2 );
	macDEBUG_DELAY ();
	
	/* Display ON (29h) */
	ILI9341_Write_Cmd ( 0x29 ); 
	
	
}


/**
 * @brief  ILI9341��ʼ�����������Ҫ�õ�lcd��һ��Ҫ�����������
 * @param  ��
 * @retval ��
 */
void ILI9341_Init ( void )
{
	ILI9341_GPIO_Config ();
	ILI9341_FSMC_Config ();
	
	ILI9341_BackLed_Control ( ENABLE );      //����LCD�����
	ILI9341_Rst ();
	ILI9341_REG_Config ();
}


/**
 * @brief  ILI9341����LED����
 * @param  enumState �������Ƿ�ʹ�ܱ���LED
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg ENABLE :ʹ�ܱ���LED
  *     @arg DISABLE :���ñ���LED
 * @retval ��
 */
void ILI9341_BackLed_Control ( FunctionalState enumState )
{
	if ( enumState )
		GPIO_ResetBits ( macILI9341_BK_PORT, macILI9341_BK_PIN );
	
	else
		GPIO_SetBits ( macILI9341_BK_PORT, macILI9341_BK_PIN );
		
}



/**
 * @brief  ILI9341 �����λ
 * @param  ��
 * @retval ��
 */
void ILI9341_Rst ( void )
{			
	GPIO_ResetBits ( macILI9341_RST_PORT, macILI9341_RST_PIN );	 //�͵�ƽ��λ

	ILI9341_Delay ( 0xAFFf<<2 ); 					   

	GPIO_SetBits ( macILI9341_RST_PORT, macILI9341_RST_PIN );		 	 

	ILI9341_Delay ( 0xAFFf<<2 ); 	
	
}


/**
 * @brief  ����ILI9341��GRAM��ɨ�跽�� 
 * @param  ucOption ��ѡ��GRAM��ɨ�跽�� 
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 1 :���Ͻ�->���½�
  *     @arg 2 :���½�->���Ͻ�
  *     @arg 3 :���½�->���Ͻ�
  *     @arg 4 :���Ͻ�->���½�
 * @retval ��
 */
void ILI9341_GramScan ( uint8_t ucOption )
{	
	switch ( ucOption )
	{
		case 1:
//   ���Ͻ�->���½�      ��ʾ��Ӣ��ʱ�õ�������ģʽ 
//		____ x(240)       //Һ������Ϊ�ο���
//	 |  
//	 |	y(320)        
		  
			ILI9341_Write_Cmd ( 0x36 ); 
			ILI9341_Write_Data ( 0xC8 );   
			ILI9341_Write_Cmd ( macCMD_SetCoordinateX ); 
			ILI9341_Write_Data ( 0x00 );	/* x start */	
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x00 );  /* x end */	
			ILI9341_Write_Data ( 0xEF );

			ILI9341_Write_Cmd ( macCMD_SetCoordinateY ); 
			ILI9341_Write_Data ( 0x00 );	/* y start */  
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x01 );	/* y end */   
			ILI9341_Write_Data ( 0x3F );
					
		  break;
		
		case 2:
//		���½�->���Ͻ�      ��ʾ����ͷͼ��ʱ�õ�������ģʽ 
//		|x(320)            //Һ������Ϊ�ο���
//		|
//		|___ y(240)
		  
			ILI9341_Write_Cmd ( 0x36 ); 
			ILI9341_Write_Data ( 0x68 );	
			ILI9341_Write_Cmd ( macCMD_SetCoordinateX ); 
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x01 );
			ILI9341_Write_Data ( 0x3F );	

			ILI9341_Write_Cmd ( macCMD_SetCoordinateY ); 
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0xEF );		
		
		  break;
		
		case 3:
//		���½�->���Ͻ�        ��ʾBMPͼƬʱ�õ�������ģʽ 
//		           |x(320)   //Һ������Ϊ�ο���
//		           |           
//		y(240) ____|
		
			ILI9341_Write_Cmd ( 0x36 ); 
			ILI9341_Write_Data ( 0x28 );	
			ILI9341_Write_Cmd ( macCMD_SetCoordinateX ); 
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x01 );
			ILI9341_Write_Data ( 0x3F );	

			ILI9341_Write_Cmd ( macCMD_SetCoordinateY ); 
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0xEF );			
		  
		  break;

		case 4:
//		���½ǽ�->���Ͻ�
//		|y(320)              //Һ������Ϊ�ο���
//		|
//		|___ x(240)			
		  
			ILI9341_Write_Cmd ( 0x36 ); 
			ILI9341_Write_Data ( 0x48 );	
			ILI9341_Write_Cmd ( macCMD_SetCoordinateX ); 
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0xEF );	

			ILI9341_Write_Cmd ( macCMD_SetCoordinateY ); 
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x00 );
			ILI9341_Write_Data ( 0x01 );
			ILI9341_Write_Data ( 0x3F );		
		
	    break;
		
	}
	
	
	/* write gram start */
	ILI9341_Write_Cmd ( macCMD_SetPixel );
	
	
}


/**
 * @brief  ��ILI9341��ʾ���Ͽ���һ������
 * @param  usX �����ض�ɨ�跽���´��ڵ����X����
 * @param  usY �����ض�ɨ�跽���´��ڵ����Y����
 * @param  usWidth �����ڵĿ��
 * @param  usHeight �����ڵĸ߶�
 * @retval ��
 */
void ILI9341_OpenWindow ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight )
{	
	ILI9341_Write_Cmd ( macCMD_SetCoordinateX ); 				 /* ����X���� */
	ILI9341_Write_Data ( usX >> 8  );	 /* �ȸ�8λ��Ȼ���8λ */
	ILI9341_Write_Data ( usX & 0xff  );	 /* ������ʼ��ͽ�����*/
	ILI9341_Write_Data ( ( usX + usWidth - 1 ) >> 8  );
	ILI9341_Write_Data ( ( usX + usWidth - 1 ) & 0xff  );

	ILI9341_Write_Cmd ( macCMD_SetCoordinateY ); 			     /* ����Y����*/
	ILI9341_Write_Data ( usY >> 8  );
	ILI9341_Write_Data ( usY & 0xff  );
	ILI9341_Write_Data ( ( usY + usHeight - 1 ) >> 8 );
	ILI9341_Write_Data ( ( usY + usHeight - 1) & 0xff );
	
}


/**
 * @brief  �趨ILI9341�Ĺ������
 * @param  usX �����ض�ɨ�跽���¹���X����
 * @param  usY �����ض�ɨ�跽���¹���Y����
 * @retval ��
 */
static void ILI9341_SetCursor ( uint16_t usX, uint16_t usY )	
{
	ILI9341_OpenWindow ( usX, usY, 1, 1 );
}


/**
 * @brief  ��ILI9341��ʾ������ĳһ��ɫ������ص�
 * @param  ulAmout_Point ��Ҫ�����ɫ�����ص������Ŀ
 * @param  usColor ����ɫ
 * @retval ��
 */
static __inline void ILI9341_FillColor ( uint32_t ulAmout_Point, uint16_t usColor )
{
	uint32_t i = 0;
	
	
	/* memory write */
	ILI9341_Write_Cmd ( macCMD_SetPixel );	
		
	for ( i = 0; i < ulAmout_Point; i ++ )
		ILI9341_Write_Data ( usColor );
		
	
}


/**
 * @brief  ��ILI9341��ʾ����ĳһ������ĳ����ɫ��������
 * @param  usX �����ض�ɨ�跽���´��ڵ����X����
 * @param  usY �����ض�ɨ�跽���´��ڵ����Y����
 * @param  usWidth �����ڵĿ��
 * @param  usHeight �����ڵĸ߶�
 * @param  usColor ����ɫ
 * @retval ��
 */
void ILI9341_Clear ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight, uint16_t usColor )
{
	ILI9341_OpenWindow ( usX, usY, usWidth, usHeight );

	ILI9341_FillColor ( usWidth * usHeight, usColor );		
	
}


/**
 * @brief  ��ILI9341��ʾ����ĳһ����ĳ����ɫ�������
 * @param  usX �����ض�ɨ�跽���¸õ��X����
 * @param  usY �����ض�ɨ�跽���¸õ��Y����
 * @param  usColor ������������ɫ
 * @retval ��
 */
void ILI9341_SetPointPixel ( uint16_t usX, uint16_t usY, uint16_t usColor )	
{	
	if ( ( usX < macILI9341_DispWindow_COLUMN ) && ( usY < macILI9341_DispWindow_PAGE ) )
  {
		ILI9341_SetCursor ( usX, usY );
		
		ILI9341_FillColor ( 1, usColor );
	}
	
}


/**
 * @brief  ��ȡILI9341 GRAN ��һ����������
 * @param  ��
 * @retval ��������
 */
static uint16_t ILI9341_Read_PixelData ( void )	
{	
	uint16_t usR=0, usG=0, usB=0 ;

	
	ILI9341_Write_Cmd ( 0x2E );   /* ������ */
	
	usR = ILI9341_Read_Data (); 	/*FIRST READ OUT DUMMY DATA*/
	
	usR = ILI9341_Read_Data ();  	/*READ OUT RED DATA  */
	usB = ILI9341_Read_Data ();  	/*READ OUT BLUE DATA*/
	usG = ILI9341_Read_Data ();  	/*READ OUT GREEN DATA*/	
	
  return ( ( ( usR >> 11 ) << 11 ) | ( ( usG >> 10 ) << 5 ) | ( usB >> 11 ) );
	
}


/**
 * @brief  ��ȡ ILI9341 ��ʾ����ĳһ����������������
 * @param  usX �����ض�ɨ�跽���¸õ��X����
 * @param  usY �����ض�ɨ�跽���¸õ��Y����
 * @retval ��������
 */
uint16_t ILI9341_GetPointPixel ( uint16_t usX, uint16_t usY )
{ 
	uint16_t usPixelData;

	
	ILI9341_SetCursor ( usX, usY );
	
	usPixelData = ILI9341_Read_PixelData ();
	
	return usPixelData;
	
}


/**
 * @brief  �� ILI9341 ��ʾ����ʹ�� Bresenham �㷨���߶� 
 * @param  usX1 �����ض�ɨ�跽�����߶ε�һ���˵�X����
 * @param  usY1 �����ض�ɨ�跽�����߶ε�һ���˵�Y����
 * @param  usX2 �����ض�ɨ�跽�����߶ε���һ���˵�X����
 * @param  usY2 �����ض�ɨ�跽�����߶ε���һ���˵�Y����
 * @param  usColor ���߶ε���ɫ
 * @retval ��
 */
void ILI9341_DrawLine ( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2, uint16_t usColor )
{
	uint16_t us; 
	uint16_t usX_Current, usY_Current;
	
	int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance; 
	int32_t lIncrease_X, lIncrease_Y; 	
	
	
	lDelta_X = usX2 - usX1; //������������ 
	lDelta_Y = usY2 - usY1; 
	
	usX_Current = usX1; 
	usY_Current = usY1; 
	
	
	if ( lDelta_X > 0 ) 
		lIncrease_X = 1; //���õ������� 
	
	else if ( lDelta_X == 0 ) 
		lIncrease_X = 0;//��ֱ�� 
	
	else 
  { 
    lIncrease_X = -1;
    lDelta_X = - lDelta_X;
  } 

	
	if ( lDelta_Y > 0 )
		lIncrease_Y = 1; 
	
	else if ( lDelta_Y == 0 )
		lIncrease_Y = 0;//ˮƽ�� 
	
	else 
  {
    lIncrease_Y = -1;
    lDelta_Y = - lDelta_Y;
  } 

	
	if (  lDelta_X > lDelta_Y )
		lDistance = lDelta_X; //ѡȡ�������������� 
	
	else 
		lDistance = lDelta_Y; 

	
	for ( us = 0; us <= lDistance + 1; us ++ )//������� 
	{  
		ILI9341_SetPointPixel ( usX_Current, usY_Current, usColor );//���� 
		
		lError_X += lDelta_X ; 
		lError_Y += lDelta_Y ; 
		
		if ( lError_X > lDistance ) 
		{ 
			lError_X -= lDistance; 
			usX_Current += lIncrease_X; 
		}  
		
		if ( lError_Y > lDistance ) 
		{ 
			lError_Y -= lDistance; 
			usY_Current += lIncrease_Y; 
		} 
		
	}  
	
	
}   


/**
 * @brief  �� ILI9341 ��ʾ���ϻ�һ������
 * @param  usX_Start �����ض�ɨ�跽���¾��ε���ʼ��X����
 * @param  usY_Start �����ض�ɨ�跽���¾��ε���ʼ��Y����
 * @param  usWidth�����εĿ�ȣ���λ�����أ�
 * @param  usHeight�����εĸ߶ȣ���λ�����أ�
 * @param  usColor �����ε���ɫ
 * @param  ucFilled ��ѡ���Ƿ����þ���
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 0 :���ľ���
  *     @arg 1 :ʵ�ľ���
 * @retval ��
 */
void ILI9341_DrawRectangle ( uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint16_t usColor, uint8_t ucFilled )
{
	if ( ucFilled )
	  ILI9341_Clear ( usX_Start, usY_Start, usWidth, usHeight, usColor);
	
	else
	{
		ILI9341_DrawLine ( usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start, usColor );
		ILI9341_DrawLine ( usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1, usColor );
		ILI9341_DrawLine ( usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1, usColor );
		ILI9341_DrawLine ( usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1, usColor );		
	}

}


/**
 * @brief  �� ILI9341 ��ʾ����ʹ�� Bresenham �㷨��Բ
 * @param  usX_Center �����ض�ɨ�跽����Բ�ĵ�X����
 * @param  usY_Center �����ض�ɨ�跽����Բ�ĵ�Y����
 * @param  usRadius��Բ�İ뾶����λ�����أ�
 * @param  usColor ��Բ����ɫ
 * @param  ucFilled ��ѡ���Ƿ�����Բ
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 0 :����Բ
  *     @arg 1 :ʵ��Բ
 * @retval ��
 */
void ILI9341_DrawCircle ( uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint16_t usColor, uint8_t ucFilled )
{
	int16_t sCurrentX, sCurrentY;
	int16_t sError;
	
	
	sCurrentX = 0; sCurrentY = usRadius;	  
	
	sError = 3 - ( usRadius << 1 );     //�ж��¸���λ�õı�־
	
	
	while ( sCurrentX <= sCurrentY )
	{
		int16_t sCountY;
		
		
		if ( ucFilled ) 			
			for ( sCountY = sCurrentX; sCountY <= sCurrentY; sCountY ++ ) 
			{                      
				ILI9341_SetPointPixel ( usX_Center + sCurrentX, usY_Center + sCountY,   usColor );           //1���о����� 
				ILI9341_SetPointPixel ( usX_Center - sCurrentX, usY_Center + sCountY,   usColor );           //2       
				ILI9341_SetPointPixel ( usX_Center - sCountY,   usY_Center + sCurrentX, usColor );           //3
				ILI9341_SetPointPixel ( usX_Center - sCountY,   usY_Center - sCurrentX, usColor );           //4
				ILI9341_SetPointPixel ( usX_Center - sCurrentX, usY_Center - sCountY,   usColor );           //5    
        ILI9341_SetPointPixel ( usX_Center + sCurrentX, usY_Center - sCountY,   usColor );           //6
				ILI9341_SetPointPixel ( usX_Center + sCountY,   usY_Center - sCurrentX, usColor );           //7 	
        ILI9341_SetPointPixel ( usX_Center + sCountY,   usY_Center + sCurrentX, usColor );           //0				
			}
		
		else
		{          
			ILI9341_SetPointPixel ( usX_Center + sCurrentX, usY_Center + sCurrentY, usColor );             //1���о�����
			ILI9341_SetPointPixel ( usX_Center - sCurrentX, usY_Center + sCurrentY, usColor );             //2      
			ILI9341_SetPointPixel ( usX_Center - sCurrentY, usY_Center + sCurrentX, usColor );             //3
			ILI9341_SetPointPixel ( usX_Center - sCurrentY, usY_Center - sCurrentX, usColor );             //4
			ILI9341_SetPointPixel ( usX_Center - sCurrentX, usY_Center - sCurrentY, usColor );             //5       
			ILI9341_SetPointPixel ( usX_Center + sCurrentX, usY_Center - sCurrentY, usColor );             //6
			ILI9341_SetPointPixel ( usX_Center + sCurrentY, usY_Center - sCurrentX, usColor );             //7 
			ILI9341_SetPointPixel ( usX_Center + sCurrentY, usY_Center + sCurrentX, usColor );             //0
    }			
		
		
		sCurrentX ++;

		
		if ( sError < 0 ) 
			sError += 4 * sCurrentX + 6;	  
		
		else
		{
			sError += 10 + 4 * ( sCurrentX - sCurrentY );   
			sCurrentY --;
		} 	
		
		
	}
	
	
}


/**
 * @brief  �� ILI9341 ��ʾ������ʾһ��Ӣ���ַ�
 * @param  usX �����ض�ɨ�跽�����ַ�����ʼX����
 * @param  usY �����ض�ɨ�跽���¸õ����ʼY����
 * @param  cChar ��Ҫ��ʾ��Ӣ���ַ�
 * @param  usColor_Background ��ѡ��Ӣ���ַ��ı���ɫ
 * @param  usColor_Background ��ѡ��Ӣ���ַ���ǰ��ɫ
 * @retval ��
 */
void ILI9341_DispChar_EN ( uint16_t usX, uint16_t usY, const char cChar, uint16_t usColor_Background, uint16_t usColor_Foreground )
{
	uint8_t ucTemp, ucRelativePositon, ucPage, ucColumn;

	
	ucRelativePositon = cChar - ' ';
	
	ILI9341_OpenWindow ( usX, usY, macWIDTH_EN_CHAR, macHEIGHT_EN_CHAR );
	
	ILI9341_Write_Cmd ( macCMD_SetPixel );	
	
	for ( ucPage = 0; ucPage < macHEIGHT_EN_CHAR; ucPage ++ )
	{
		ucTemp = ucAscii_1608 [ ucRelativePositon ] [ ucPage ];
		
		for ( ucColumn = 0; ucColumn < macWIDTH_EN_CHAR; ucColumn ++ )
		{
			if ( ucTemp & 0x01 )
				ILI9341_Write_Data ( usColor_Foreground );
			
			else
				ILI9341_Write_Data ( usColor_Background );								
			
			ucTemp >>= 1;		
			
		}/* д��һ�� */
		
	}/* ȫ��д�� */
	
}


/**
 * @brief  �� ILI9341 ��ʾ������ʾӢ���ַ���
 * @param  usX �����ض�ɨ�跽�����ַ�������ʼX����
 * @param  usY �����ض�ɨ�跽�����ַ�������ʼY����
 * @param  pStr ��Ҫ��ʾ��Ӣ���ַ������׵�ַ
 * @param  usColor_Background ��ѡ��Ӣ���ַ����ı���ɫ
 * @param  usColor_Background ��ѡ��Ӣ���ַ�����ǰ��ɫ
 * @retval ��
 */
void ILI9341_DispString_EN ( uint16_t usX, uint16_t usY, const char * pStr, uint16_t usColor_Background, uint16_t usColor_Foreground )
{
	while ( * pStr != '\0' )
	{
		if ( ( usX - macILI9341_DispWindow_X_Star + macWIDTH_EN_CHAR ) > macILI9341_DispWindow_COLUMN )
		{
			usX = macILI9341_DispWindow_X_Star;
			usY += macHEIGHT_EN_CHAR;
		}
		
		if ( ( usY - macILI9341_DispWindow_Y_Star + macHEIGHT_EN_CHAR ) > macILI9341_DispWindow_PAGE )
		{
			usX = macILI9341_DispWindow_X_Star;
			usY = macILI9341_DispWindow_Y_Star;
		}
		
		ILI9341_DispChar_EN ( usX, usY, * pStr, usColor_Background, usColor_Foreground );
		
		pStr ++;
		
		usX += macWIDTH_EN_CHAR;
		
	}
	
}


/**
 * @brief  �� ILI9341 ��ʾ������ʾһ�������ַ�
 * @param  usX �����ض�ɨ�跽�����ַ�����ʼX����
 * @param  usY �����ض�ɨ�跽�����ַ�����ʼY����
 * @param  usChar ��Ҫ��ʾ�������ַ��������룩
 * @param  usColor_Background ��ѡ�������ַ��ı���ɫ
 * @param  usColor_Background ��ѡ�������ַ���ǰ��ɫ
 * @retval ��
 */ 
void ILI9341_DispChar_CH ( uint16_t usX, uint16_t usY, uint16_t usChar, uint16_t usColor_Background, uint16_t usColor_Foreground )
{
	uint8_t ucPage, ucColumn;
	uint8_t ucBuffer [ 32 ];	

  uint16_t usTemp; 	

	
	ILI9341_OpenWindow ( usX, usY, macWIDTH_CH_CHAR, macHEIGHT_CH_CHAR );
	
	ILI9341_Write_Cmd ( macCMD_SetPixel );
	   
  macGetGBKCode ( ucBuffer, usChar );	//ȡ��ģ����
	
	for ( ucPage = 0; ucPage < macHEIGHT_CH_CHAR; ucPage ++ )
	{
    /* ȡ�������ֽڵ����ݣ���lcd�ϼ���һ�����ֵ�һ�� */
		usTemp = ucBuffer [ ucPage * 2 ];
		usTemp = ( usTemp << 8 );
		usTemp |= ucBuffer [ ucPage * 2 + 1 ];
		
		for ( ucColumn = 0; ucColumn < macWIDTH_CH_CHAR; ucColumn ++ )
		{			
			if ( usTemp & ( 0x01 << 15 ) )  //��λ��ǰ 
			  ILI9341_Write_Data ( usColor_Foreground );
				
			else
				ILI9341_Write_Data ( usColor_Background );
			
			usTemp <<= 1;
			
		}
		
	}
	
}


/**
 * @brief  �� ILI9341 ��ʾ������ʾ�����ַ���
 * @param  usX �����ض�ɨ�跽�����ַ�������ʼX����
 * @param  usY �����ض�ɨ�跽�����ַ�������ʼY����
 * @param  pStr ��Ҫ��ʾ��Ӣ���ַ������׵�ַ
 * @param  usColor_Background ��ѡ���ַ����ı���ɫ
 * @param  usColor_Background ��ѡ���ַ�����ǰ��ɫ
 * @retval ��
 */
void ILI9341_DispString_CH ( uint16_t usX, uint16_t usY, const uint8_t * pStr, uint16_t usColor_Background, uint16_t usColor_Foreground )
{	
	uint16_t usCh;
	
	
	while( * pStr != '\0' )
	{		
		if ( ( usX - macILI9341_DispWindow_X_Star + macWIDTH_CH_CHAR ) > macILI9341_DispWindow_COLUMN )
		{
			usX = macILI9341_DispWindow_X_Star;
			usY += macHEIGHT_CH_CHAR;
		}
		
		if ( ( usY - macILI9341_DispWindow_Y_Star + macHEIGHT_CH_CHAR ) > macILI9341_DispWindow_PAGE )
		{
			usX = macILI9341_DispWindow_X_Star;
			usY = macILI9341_DispWindow_Y_Star;
		}	
		
		usCh = * ( uint16_t * ) pStr;	
	  usCh = ( usCh << 8 ) + ( usCh >> 8 );

		ILI9341_DispChar_CH ( usX, usY, usCh, usColor_Background, usColor_Foreground );
		
		usX += macWIDTH_CH_CHAR;
		
		pStr += 2;           //һ�����������ֽ� 

	}	   
	
}


/**
 * @brief  �� ILI9341 ��ʾ������ʾ��Ӣ���ַ���
 * @param  usX �����ض�ɨ�跽�����ַ�������ʼX����
 * @param  usY �����ض�ɨ�跽�����ַ�������ʼY����
 * @param  pStr ��Ҫ��ʾ���ַ������׵�ַ
 * @param  usColor_Background ��ѡ���ַ����ı���ɫ
 * @param  usColor_Background ��ѡ���ַ�����ǰ��ɫ
 * @retval ��
 */
void ILI9341_DispString_EN_CH ( uint16_t usX, uint16_t usY, const uint8_t * pStr, uint16_t usColor_Background, uint16_t usColor_Foreground )
{
	uint16_t usCh;
	
	
	while( * pStr != '\0' )
	{
		if ( * pStr <= 126 )	           	//Ӣ���ַ�
		{
			if ( ( usX - macILI9341_DispWindow_X_Star + macWIDTH_EN_CHAR ) > macILI9341_DispWindow_COLUMN )
			{
				usX = macILI9341_DispWindow_X_Star;
				usY += macHEIGHT_EN_CHAR;
			}
			
			if ( ( usY - macILI9341_DispWindow_Y_Star + macHEIGHT_EN_CHAR ) > macILI9341_DispWindow_PAGE )
			{
				usX = macILI9341_DispWindow_X_Star;
				usY = macILI9341_DispWindow_Y_Star;
			}			
		
		  ILI9341_DispChar_EN ( usX, usY, * pStr, usColor_Background, usColor_Foreground );
			
			usX += macWIDTH_EN_CHAR;
		
		  pStr ++;

		}
		
		else	                            //�����ַ�
		{
			if ( ( usX - macILI9341_DispWindow_X_Star + macWIDTH_CH_CHAR ) > macILI9341_DispWindow_COLUMN )
			{
				usX = macILI9341_DispWindow_X_Star;
				usY += macHEIGHT_CH_CHAR;
			}
			
			if ( ( usY - macILI9341_DispWindow_Y_Star + macHEIGHT_CH_CHAR ) > macILI9341_DispWindow_PAGE )
			{
				usX = macILI9341_DispWindow_X_Star;
				usY = macILI9341_DispWindow_Y_Star;
			}	
			
			usCh = * ( uint16_t * ) pStr;	
			
			usCh = ( usCh << 8 ) + ( usCh >> 8 );		

			ILI9341_DispChar_CH ( usX, usY, usCh, usColor_Background, usColor_Foreground );
			
			usX += macWIDTH_CH_CHAR;
			
			pStr += 2;           //һ�����������ֽ� 
		
    }
		
  }
	
	
} 

