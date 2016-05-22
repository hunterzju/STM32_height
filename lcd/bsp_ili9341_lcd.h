#ifndef      __BSP_ILI9341_LCD_H
#define	     __BSP_ILI9341_LCD_H


#include "stm32f10x.h"



/***************************************************************************************
2^26 =0X0400 0000 = 64MB,ÿ�� BANK ��4*64MB = 256MB
64MB:FSMC_Bank1_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
64MB:FSMC_Bank1_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
64MB:FSMC_Bank1_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
64MB:FSMC_Bank1_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF

ѡ��BANK1-BORSRAM1 ���� TFT����ַ��ΧΪ0X6000 0000 ~ 0X63FF FFFF
FSMC_A16 ��LCD��DC(�Ĵ���/����ѡ��)��
�Ĵ�������ַ = 0X60000000
RAM����ַ = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000
��ѡ��ͬ�ĵ�ַ��ʱ����ַҪ���¼���  
****************************************************************************************/

/******************************* ILI9341 ��ʾ���� FSMC �������� ***************************/
#define      macFSMC_Addr_ILI9341_CMD         ( ( uint32_t ) 0x60000000 )	     //FSMC_Bank1_NORSRAM1����LCD��������ĵ�ַ
#define      macFSMC_Addr_ILI9341_DATA        ( ( uint32_t ) 0x60020000 )      //FSMC_Bank1_NORSRAM1����LCD���ݲ����ĵ�ַ      

#define      macFSMC_Bank1_NORSRAMx           FSMC_Bank1_NORSRAM1



/******************************* ILI9341 ��ʾ��8080ͨѶ���Ŷ��� ***************************/
#define      macILI9341_CS_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define      macILI9341_CS_CLK                RCC_APB2Periph_GPIOD    
#define      macILI9341_CS_PORT               GPIOD   
#define      macILI9341_CS_PIN                GPIO_Pin_7

#define      macILI9341_DC_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define      macILI9341_DC_CLK                RCC_APB2Periph_GPIOD    
#define      macILI9341_DC_PORT               GPIOD   
#define      macILI9341_DC_PIN                GPIO_Pin_11

#define      macILI9341_RST_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define      macILI9341_RST_CLK               RCC_APB2Periph_GPIOE   
#define      macILI9341_RST_PORT              GPIOE
#define      macILI9341_RST_PIN               GPIO_Pin_1

#define      macILI9341_BK_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define      macILI9341_BK_CLK                RCC_APB2Periph_GPIOD    
#define      macILI9341_BK_PORT               GPIOD
#define      macILI9341_BK_PIN                GPIO_Pin_12



/*************************************** ����Ԥ�� ******************************************/
#define      macDEBUG_DELAY()                



/***************** ILI934 ��ʾ��ȫ��Ĭ�ϣ�ɨ�跽��Ϊ1ʱ������Ⱥ����߶� *********************/
#define      macILI9341_Default_Max_Width		  240     //��ʼ���X����
#define      macILI9341_Default_Max_Heigth		320     //��ʼ���Y����




/***************************** ILI934 ��ʾ�������ʼ������������� ***************************/
#define      macILI9341_DispWindow_X_Star		    0     //��ʼ���X����
#define      macILI9341_DispWindow_Y_Star		    0     //��ʼ���Y����

#define      macILI9341_DispWindow_COLUMN		  240     //������
#define      macILI9341_DispWindow_PAGE		    320     //������



/***************************** �� ILI934 ��ʾ������ʾ���ַ��Ĵ�С ***************************/
#define      macWIDTH_EN_CHAR		                 8	      //Ӣ���ַ���� 
#define      macHEIGHT_EN_CHAR		              16		    //Ӣ���ַ��߶� 


#define      macWIDTH_CH_CHAR		                16	    //�����ַ���� 
#define      macHEIGHT_CH_CHAR		              16		  //�����ַ��߶� 

#define      macGetGBKCode( ucBuffer, usChar )  	  //�����ȡ�����ַ���ģ����ĺ�������ucBufferΪ�����ģ��������usCharΪ�����ַ��������룩



/******************************* ���� ILI934 ��ʾ��������ɫ ********************************/
#define      macBACKGROUND		                macBLACK   //Ĭ�ϱ�����ɫ

#define      macWHITE		 		                  0xFFFF	   //��ɫ
#define      macBLACK                         0x0000	   //��ɫ 
#define      macGREY                          0xF7DE	   //��ɫ 
#define      macBLUE                          0x001F	   //��ɫ 
#define      macBLUE2                         0x051F	   //ǳ��ɫ 
#define      macRED                           0xF800	   //��ɫ 
#define      macMAGENTA                       0xF81F	   //����ɫ�����ɫ 
#define      macGREEN                         0x07E0	   //��ɫ 
#define      macCYAN                          0x7FFF	   //����ɫ����ɫ 
#define      macYELLOW                        0xFFE0	   //��ɫ 
#define      macBRED                          0xF81F
#define      macGRED                          0xFFE0
#define      macGBLUE                         0x07FF



/******************************* ���� ILI934 �������� ********************************/
#define      macCMD_SetCoordinateX		 		    0x2A	     //����X����
#define      macCMD_SetCoordinateY		 		    0x2B	     //����Y����
#define      macCMD_SetPixel		 		          0x2C	     //�������



/********************************** ���� ILI934 ���� ***************************************/
void                     ILI9341_Init                    ( void );
void                     ILI9341_Rst                     ( void );
void                     ILI9341_BackLed_Control         ( FunctionalState enumState );
__inline void            ILI9341_Write_Cmd               ( uint16_t usCmd );
__inline void            ILI9341_Write_Data              ( uint16_t usData );
__inline uint16_t        ILI9341_Read_Data               ( void );
void                     ILI9341_GramScan                ( uint8_t ucOtion );
void                     ILI9341_OpenWindow              ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight );
void                     ILI9341_Clear                   ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight, uint16_t usColor );
void                     ILI9341_SetPointPixel           ( uint16_t usX, uint16_t usY, uint16_t usColor );
uint16_t                 ILI9341_GetPointPixel           ( uint16_t usX , uint16_t usY );
void                     ILI9341_DrawLine                ( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2, uint16_t usColor );
void                     ILI9341_DrawRectangle           ( uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint16_t usColor, uint8_t ucFilled );
void                     ILI9341_DrawCircle              ( uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint16_t usColor, uint8_t ucFilled );
void                     ILI9341_DispChar_EN             ( uint16_t usX, uint16_t usY, const char cChar, uint16_t usColor_Background, uint16_t usColor_Foreground );
void                     ILI9341_DispString_EN           ( uint16_t usX, uint16_t usY, const char * pStr, uint16_t usColor_Background, uint16_t usColor_Foreground );
void                     ILI9341_DispChar_CH             ( uint16_t usX, uint16_t usY, uint16_t usChar, uint16_t usColor_Background, uint16_t usColor_Foreground );
void                     ILI9341_DispString_CH           ( uint16_t usX, uint16_t usY, const uint8_t * pStr, uint16_t usColor_Background, uint16_t usColor_Foreground );
void                     ILI9341_DispString_EN_CH        ( uint16_t usX, uint16_t usY, const uint8_t * pStr, uint16_t usColor_Background, uint16_t usColor_Foreground );



#endif /* __BSP_ILI9341_ILI9341_H */

