/******************************************************************************
* �ļ����ƣ�ili932x.c
* ժ Ҫ��֧��ILI9320��ILI9325����IC���Ƶ�QVGA��ʾ����ʹ��16λ���д���
  ��ͷ�ļ������� ��Ļʹ�÷��������IC����
  ע�⣺16λ������ɫ�ʷֲ�>>  BGR(565)

* ��ǰ�汾��V1.3
* �޸�˵�����汾�޶�˵����
  1.�޸ķ�תģʽ�µ�ASCII�ַ�дBug
  2.���ӿ����ڷ�תģʽ�µ��Զ���д
  3.�Ż�ˢͼƬ ʹ����ˮ�߷�����Ч��

* ����������Ϫ
* �޶�������Ϫ
* �������ڣ�2008��9��29��
* ������ڣ�2008��12��11��

*��Ҫ˵����
��.h�ļ��У�#define Immediatelyʱ��������ʾ��ǰ����
�����#define Delay����ֻ����ִ����LCD_WR_REG(0x0007,0x0173);
֮��Ż���ʾ��ִ��һ��LCD_WR_REG(0x0007,0x0173)������д����
�ݶ�������ʾ��
#define Delayһ�����ڿ����������ʾ����ֹ��ʾ��ȫ��ͼ���ˢ��
����
******************************************************************************/
#include "stm32f10x_lib.h"
#include "ili932x.h"
#include "spi_flash.h"


/****************************************************************
��������Lcd���ú���
���ܣ��������к�Lcd��ص�GPIO��ʱ��
���ŷ���Ϊ��
PE����16Bit��������
PD15����Lcd_rst
PD14����Lcd_rd*
PD13����Lcd_wr
PD12����Lcd_rs*
PD11����Lcd_cs
PB5����Lcd_blaklight ���⿿��ЧӦ����������ģ��
*****************************************************************/
void Lcd_Configuration(void)
{
  
	GPIO_InitTypeDef GPIO_InitStructure;
	/*������Ӧʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);  
	/*����Lcd��������Ϊ�������*/
	/*16λ����*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	/*���ƽ�*/
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	/*�������*/
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/**********************************************
��������Lcd��ʼ������
���ܣ���ʼ��Lcd
��ڲ�������
����ֵ����
***********************************************/
void Lcd_Initialize(void)
{
  
	Lcd_Light_ON;
	DataToWrite(0xffff);//������ȫ��
	Set_nWr;
	Set_Cs;
	Set_Rs;
	Set_nRd;
	Set_Rst;
	Set_Rst;
	Delay_nms(1);
	Clr_Rst;
	Delay_nms(1);
	Set_Rst;
	Delay_nms(1);       
#ifdef ILI9325        
	LCD_WR_REG(0x00e3,0x3008);
	LCD_WR_REG(0x00e7,0x0012);
	LCD_WR_REG(0x00ef,0x1231);//Set the internal vcore voltage
	LCD_WR_REG(0x0001,0x0100);//S
#endif  
#ifdef ILI9320
	LCD_WR_REG(0x00e5,0x8000);
	LCD_WR_REG(0x0000,0x0001);
	LCD_WR_REG(0x0001,0x0100);//S
	Delay_nms(10);
#endif        
	LCD_WR_REG(0x0002,0x0700);//Line inversion        
#if   ID_AM==000       
	LCD_WR_REG(0x0003,0x0000);//��Ļ��ת���� TFM=0,TRI=0,SWAP=1,16 bits system interface  swap RGB to BRG���˴�ORG��HWM Ϊ0
#elif ID_AM==001        
	LCD_WR_REG(0x0003,0x0008);      
#elif ID_AM==010  
	LCD_WR_REG(0x0003,0x0010);        
#elif ID_AM==011
	LCD_WR_REG(0x0003,0x0018);
#elif ID_AM==100  
	LCD_WR_REG(0x0003,0x0020);      
#elif ID_AM==101  
	LCD_WR_REG(0x0003,0x0028);      
#elif ID_AM==110  
	LCD_WR_REG(0x0003,0x0030);      
#elif ID_AM==111  
	LCD_WR_REG(0x0003,0x0038);
#endif      
	LCD_WR_REG(0x0004,0x0000);
	LCD_WR_REG(0x0008,0x0207);
	LCD_WR_REG(0x0009,0x0000);
	LCD_WR_REG(0x000a,0x0000);
	LCD_WR_REG(0x000c,0x0001);//�˴����ýӿ����� 16λ system�ӿ�
	LCD_WR_REG(0x000d,0x0000);
	LCD_WR_REG(0x000f,0x0000);      
	//��Դ����
	LCD_WR_REG(0x0010,0x0000);
	LCD_WR_REG(0x0011,0x0000);
	LCD_WR_REG(0x0012,0x0000);
	LCD_WR_REG(0x0013,0x0000);
	Delay_nms(200);
	LCD_WR_REG(0x0010,0x17b0);
	LCD_WR_REG(0x0011,0x0137);
	Delay_nms(50);
	LCD_WR_REG(0x0012,0x0139);
	Delay_nms(50);
	LCD_WR_REG(0x0013,0x1700);
	LCD_WR_REG(0x0029,0x000c);
	//LCD_WR_REG(0x002b,0x000d);
	Delay_nms(50);      
#if   ID_AM==000         
	LCD_WR_REG(0x0020,0x00ef);//GRAMˮƽ��ʼλ��
	LCD_WR_REG(0x0021,0x013f);      
#elif ID_AM==001
	LCD_WR_REG(0x0020,0x00ef);
	LCD_WR_REG(0x0021,0x013f);      
#elif ID_AM==010
	LCD_WR_REG(0x0020,0x0000);
	LCD_WR_REG(0x0021,0x013f);      
#elif ID_AM==011
	LCD_WR_REG(0x0020,0x0000);
	LCD_WR_REG(0x0021,0x013f);       
#elif ID_AM==100
	LCD_WR_REG(0x0020,0x00ef);
	LCD_WR_REG(0x0021,0x0000);      
#elif ID_AM==101  
	LCD_WR_REG(0x0020,0x00ef);
	LCD_WR_REG(0x0021,0x0000);      
#elif ID_AM==110
	LCD_WR_REG(0x0020,0x0000);
	LCD_WR_REG(0x0021,0x0000);      
#elif ID_AM==111
	LCD_WR_REG(0x0020,0x0000);
	LCD_WR_REG(0x0021,0x0000);         
#endif       
	LCD_WR_REG(0x0030,0x0000);
	LCD_WR_REG(0x0031,0x0507);
	LCD_WR_REG(0x0032,0x0104);
	LCD_WR_REG(0x0035,0x0105);
	LCD_WR_REG(0x0036,0x0404);
	LCD_WR_REG(0x0037,0x0603);
	LCD_WR_REG(0x0038,0x0004);
	LCD_WR_REG(0x0039,0x0007);
	LCD_WR_REG(0x003c,0x0501);
	LCD_WR_REG(0x003d,0x0404);
	LCD_WR_REG(0x0050,0x0000);//ˮƽ GRAM��ʼλ��
	LCD_WR_REG(0x0051,0x00ef);//ˮƽGRAM��ֹλ��
	LCD_WR_REG(0x0052,0x0000);//��ֱGRAM��ʼλ��
	LCD_WR_REG(0x0053,0x013f);//��ֱGRAM��ֹλ��
#ifdef ILI9325        
	LCD_WR_REG(0x0060,0xa700);//G
#endif
#ifdef ILI9320        
	LCD_WR_REG(0x0060,0x2700);//G  
#endif    
	LCD_WR_REG(0x0061,0x0001);//Enables the grayscale inversion of the image by setting REV=1.??????????????????????????????
	LCD_WR_REG(0x006a,0x0000);//��ʹ�þ�������
	LCD_WR_REG(0x0080,0x0000);
	LCD_WR_REG(0x0081,0x0000);
	LCD_WR_REG(0x0082,0x0000);
	LCD_WR_REG(0x0083,0x0000);
	LCD_WR_REG(0x0084,0x0000);
	LCD_WR_REG(0x0085,0x0000);
	LCD_WR_REG(0x0090,0x0010);
	LCD_WR_REG(0x0092,0x0000);
	LCD_WR_REG(0x0093,0x0003);
	LCD_WR_REG(0x0095,0x0110);
	LCD_WR_REG(0x0097,0x0000);
	LCD_WR_REG(0x0098,0x0000);     
	//��ʾ����д�� 0x0130�ǲ���ʾ��ǰ����
	//0x0173����ʾ��ǰ����
	LCD_WR_REG(0x0007,0x0130);
#ifdef Immediately
	LCD_WR_REG(0x0007,0x0173);      
#endif        
	//��һ�����صĶ�ȡ��������Чֵ���������ȶ�ȡһ�Σ���ȥ��Чֵ
}


/******************************************
��������Lcdд�����
���ܣ���Lcdָ��λ��д��Ӧ�����������
��ڲ�����Index ҪѰַ�ļĴ�����ַ
          ConfigTemp д������ݻ�����ֵ
����ֵ����
******************************************/
void LCD_WR_REG(u16 Index,u16 CongfigTemp)
{
	Clr_Cs;
	Clr_Rs;
	DataToWrite(Index);
	Clr_nWr;
	//Delay_nus(1);
	Set_nWr;
	Set_Rs;       
	DataToWrite(CongfigTemp);       
	Clr_nWr;
	//Delay_nus(1);
	Set_nWr;
	Set_Cs;
}


/************************************************
��������Lcdд��ʼ����
���ܣ�����Lcd�������� ִ��д����
��ڲ�������
����ֵ����
************************************************/
void Lcd_WR_Start(void)
{
	Clr_Cs;
	Clr_Rs;
	DataToWrite(0x0022);
	Clr_nWr;
	//Delay_nus(1);
	Set_nWr;
	Set_Rs;
}


/*************************************************
��������Lcd�����㶨λ����
���ܣ�ָ��320240Һ���ϵ�һ����Ϊд���ݵ���ʼ��
��ڲ�����x ���� 0~239
          y ���� 0~319
����ֵ����
*************************************************/
void Lcd_SetCursor(u8 x,u16 y)
{ 
	LCD_WR_REG(0x20,x);
	LCD_WR_REG(0x21,y);    
}


/**********************************************
��������Lcdȫ����������
���ܣ���Lcd������Ϊָ����ɫ
��ڲ�����color ָ��Lcdȫ����ɫ RGB(5-6-5)
����ֵ����
***********************************************/
void Lcd_Clear(u16 Color)
{
	u32 temp;
  
	Lcd_SetCursor(0x00, 0x0000);
	LCD_WR_REG(0x0050,0x00);//ˮƽ GRAM��ʼλ��
	LCD_WR_REG(0x0051,239);//ˮƽGRAM��ֹλ��
	LCD_WR_REG(0x0052,0x00);//��ֱGRAM��ʼλ��
	LCD_WR_REG(0x0053,319);//��ֱGRAM��ֹλ��   
	Lcd_WR_Start();
	Set_Rs;
  
	for (temp = 0; temp < 76800; temp++)
	{
		DataToWrite(Color);
		Clr_nWr;
		Set_nWr;
	}
  
	Set_Cs;
}
/**********************************************
��������Lcd��ѡ����
���ܣ�ѡ��Lcd��ָ���ľ�������

ע�⣺xStart�� yStart������Ļ����ת���ı䣬λ���Ǿ��ο���ĸ���

��ڲ�����xStart x�������ʼ��
          ySrart y�������ֹ��
          xLong Ҫѡ�����ε�x���򳤶�
          yLong  Ҫѡ�����ε�y���򳤶�
����ֵ����
***********************************************/
void Lcd_SetBox(u8 xStart,u16 yStart,u8 xLong,u16 yLong,u16 x_offset,u16 y_offset)
{
  
#if ID_AM==000    
	Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+yLong-1+y_offset);

#elif ID_AM==001
	Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+yLong-1+y_offset);
     
#elif ID_AM==010
	Lcd_SetCursor(xStart+x_offset,yStart+yLong-1+y_offset);
     
#elif ID_AM==011 
	Lcd_SetCursor(xStart+x_offset,yStart+yLong-1+y_offset);
     
#elif ID_AM==100
	Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+y_offset);     
     
#elif ID_AM==101
	Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+y_offset);     
     
#elif ID_AM==110
	Lcd_SetCursor(xStart+x_offset,yStart+y_offset); 
     
#elif ID_AM==111
	Lcd_SetCursor(xStart+x_offset,yStart+y_offset);  
     
#endif
     
	LCD_WR_REG(0x0050,xStart+x_offset);//ˮƽ GRAM��ʼλ��
	LCD_WR_REG(0x0051,xStart+xLong-1+x_offset);//ˮƽGRAM��ֹλ��
	LCD_WR_REG(0x0052,yStart+y_offset);//��ֱGRAM��ʼλ��
	LCD_WR_REG(0x0053,yStart+yLong-1+y_offset);//��ֱGRAM��ֹλ�� 
}


void Lcd_ColorBox(u8 xStart,u16 yStart,u8 xLong,u16 yLong,u16 Color)
{
	u32 temp;
  
	Lcd_SetBox(xStart,yStart,xLong,yLong,0,0);
	Lcd_WR_Start();
	Set_Rs;
  
	for (temp=0; temp<xLong*yLong; temp++)
	{
		DataToWrite(Color);
		Clr_nWr;
		Set_nWr;
	}

	Set_Cs;
}


void Lcd_ClearCharBox(u8 x,u16 y,u16 Color)
{
	u32 temp;
  
	Lcd_SetBox(x*8,y*16,8,16,0,0); 
	Lcd_WR_Start();
	Set_Rs;
  
	for (temp=0; temp < 128; temp++)
	{
		DataToWrite(Color); 
		Clr_nWr;
		//Delay_nus(22);
		Set_nWr; 
	}
	
	Set_Cs;
}


/****************************************************************
��������Lcdд1��ASCII�ַ�����
��ڲ�����x,�������꣬�����ҷֱ���0~29 
          y,�������꣬���ϵ��·ֱ�Ϊ0~19
          CharColaor,�ַ�����ɫ 
          CharBackColor,�ַ�������ɫ 
         ASCIICode,��Ӧ�ַ���ASCII��
Ҳ����˵��320240�ֱ��ʵ���ʾ������������ʾ30��ASCII�ַ�����������ʾ20��
����ֵ����

ע�⣡���������������ʹ�ô˺�����Ӧ�ü���Lcd_Rs_H()��Set_Cs;Ϊ���Ż�ϵͳʡȥ��
���ָ�����˺���ִ�е���һ�������д�����RS_L�������д�뽫����
����ΪILI9320��Ϊ��RS_Lʱд���������
*****************************************************************/
void Lcd_WriteASCII(u8 x,u8 y,u16 x_offset,u16 y_offset,u16 CharColor,u16 CharBackColor,u8 ASCIICode)
{
	u8 RowCounter,BitCounter;
	u8 *ASCIIPointer;
	u8 ASCIIBuffer[16];
  
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
	Lcd_SetBox(x*8,y*16,8,16,x_offset,y_offset);    
#else
	Lcd_SetBox(x*16,y*8,16,8,x_offset,y_offset);    
#endif 
        
	Lcd_WR_Start();
	GetASCIICode(ASCIIBuffer,ASCIICode,ASCII_Offset);//ȡ����ַ�����ʾ����
	ASCIIPointer=ASCIIBuffer;
  
	for (RowCounter=0; RowCounter<16; RowCounter++)
	{ 
		for (BitCounter=0; BitCounter<8; BitCounter++)
		{
			if ((*ASCIIPointer&(0x80 >> BitCounter)) == 0x00)
			{
				//Set_Rs;
				DataToWrite(CharBackColor);
				Clr_nWr;
				Set_nWr;
			}
			else
			{
				//Set_Rs;
				DataToWrite(CharColor);
				Clr_nWr;
				Set_nWr; 
			}
		}
		ASCIIPointer++;
	}
	// Set_Cs;
}


void Lcd_WriteASCIIClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u8 ASCIICode)
{
  
  GPIO_InitTypeDef GPIO_InitStructure;
  u8 RowCounter,BitCounter;
  u8 *ASCIIPointer;
  u8 ASCIIBuffer[16];
  u16 Temp;
        
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
  Lcd_SetBox(x*8,y*16,8,16,x_offset,y_offset);    
#else
  Lcd_SetBox(x*16,y*8,16,8,x_offset,y_offset);    
#endif
    
  Lcd_WR_Start();
  GetASCIICode(ASCIIBuffer,ASCIICode,ASCII_Offset);//ȡ����ַ�����ʾ����
  ASCIIPointer=ASCIIBuffer;
  
  for (RowCounter=0; RowCounter<16; RowCounter++)
  { 
    for(BitCounter=0;BitCounter<8;BitCounter++)
    {
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
    ASCIIPointer++;
  }
  // Set_Cs;
  
}
/**************************************************************
��Һ����д��32*16��ASCII�ַ�
���������
x������   x :(0~14)
y������   y :(0~9)
x��ƫ���� x_offset:(��������0~239)
y��ƫ���� y_offset:(��������0~319)  ע�⣺ƫ�������ܹ��󣬵�x���򳬳�16��y���򳬹�32
                                          Ҳ����32*16�ַ���Сʱǿ�ҽ���ʹ��x��y������
                                          ��Ȼƫ��������ʵ�ִ�Χƫ�Ƶ��ǿɶ��Ժ�ά
                                          ���Զ��ϲ�
�ַ���ɫ CharColor:�ַ���ɫ
������ɫ CharBackColor��������ɫ
***************************************************************/
void Lcd_Write32X16ASCII(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,u8 ASCIICode)
{

  u8 RowCounter,BitCounter;
  u8 *ASCIIPointer;
  u8 ASCIIBuffer[16];
  
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
  Lcd_SetBox(x*16,y*32,16,32,x_offset,y_offset);    
#else
  Lcd_SetBox(x*32,y*16,32,16,x_offset,y_offset);    
#endif
    
  Lcd_WR_Start();
  GetASCIICode(ASCIIBuffer,ASCIICode,ASCII_Offset);//ȡ����ַ�����ʾ����
  ASCIIPointer=ASCIIBuffer;
  
  for(RowCounter=0; RowCounter<16; RowCounter++)
  { 
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }      
    }     
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {         
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
    ASCIIPointer++;
  }
  // Set_Cs;
}
//���Ե���ʹ�õ�дһ��ASCII�ַ�����
void Lcd_Write32X16ASCIIWrite(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,u8 ASCIICode)
{
        u8 RowCounter,BitCounter;
        u8 *ASCIIPointer;
        
        //����ASCII�ַ�λ��
#if   (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110) 
        
        Lcd_SetBox(x*16,y*32,16,32,x_offset,y_offset);
        
#else
        
        Lcd_SetBox(x*32,y*16,32,16,x_offset,y_offset);
        
#endif
        
        Lcd_WR_Start();
        CatchASCII(ASCIICode,ASCII_Offset);//ȡ����ַ�����ʾ����
        ASCIIPointer=CharBuffer;
        for(RowCounter=0;RowCounter<16;RowCounter++)
        { 
          for(BitCounter=0;BitCounter<8;BitCounter++)
          {
            if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
            {
               
               Set_Rs;
               DataToWrite(CharBackColor);
               Clr_nWr;
               Set_nWr;
               DataToWrite(CharBackColor);
               Clr_nWr;
               Set_nWr;

            }
            else
            {
               Set_Rs;
               DataToWrite(CharColor);
               Clr_nWr;
               Set_nWr; 
               DataToWrite(CharColor);
               Clr_nWr;
               Set_nWr; 
            }
            
          }
          
        for(BitCounter=0;BitCounter<8;BitCounter++)
          {
            if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
            {
               
               Set_Rs;
               DataToWrite(CharBackColor);
               Clr_nWr;
               Set_nWr;
               DataToWrite(CharBackColor);
               Clr_nWr;
               Set_nWr;

            }
            else
            {
               Set_Rs;
               DataToWrite(CharColor);
               Clr_nWr;
               Set_nWr; 
               DataToWrite(CharColor);
               Clr_nWr;
               Set_nWr; 
            }
            
          }
           ASCIIPointer++;
        }
         Set_Cs;
}
void Lcd_Write32X16ASCIIClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u8 ASCIICode)
{

  GPIO_InitTypeDef GPIO_InitStructure;
  u8 RowCounter,BitCounter;
  u8 *ASCIIPointer;
  u8 ASCIIBuffer[16];
  u16 Temp;

#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
  Lcd_SetBox(x*16,y*32,16,32,x_offset,y_offset);    
#else
  Lcd_SetBox(x*32,y*16,32,16,x_offset,y_offset);    
#endif
    
  Lcd_WR_Start();
  GetASCIICode(ASCIIBuffer,ASCIICode,ASCII_Offset);//ȡ����ַ�����ʾ����
  ASCIIPointer=ASCIIBuffer;
  
  for(RowCounter=0; RowCounter<16; RowCounter++)
  { 
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                     
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }    
    }
          
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
              
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;       
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;          
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }            
    }
    ASCIIPointer++;
  }
  // Set_Cs;
}
/************************************************************
��������Lcdд�ַ�������
���ܣ���ָ��λ��д��һ�������ַ������������Զ����й���
��ڲ�����x,�������꣬�����ҷֱ���0~29 
          y,�������꣬���ϵ��·ֱ�Ϊ0~19
          CharColaor,�ַ�����ɫ 
          CharBackColor,�ַ�������ɫ 
          *s ָ��Ҫд���ַ���
����ֵ����
*************************************************************/
void Lcd_WriteString(u8 x,u8 y,u16 x_offset,u16 y_offset,u16 CharColor,u16 CharBackColor,char *s)
{

  u8 databuff;
  Set_Rs;

  do
  {
    databuff=*s++;  
    Lcd_WriteASCII(x,y,x_offset,y_offset,CharColor,CharBackColor,databuff);
    
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<29)
    {
      x++;
    } 
    else if (y<19)
    {
      x=0;
      y++;
    }   
    else
    {
      x=0;
      y=0;
    }
       
#else
    if (y<39)
    {
      y++;
    }
    else if (x<14)
    {
      y=0;
      x++;
    }
    else
    {
      x=0;
      y=0;
    }
#endif 
       
  }
  while (*s!=0);
  
  Set_Cs;
  
}
void Lcd_WriteStringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s)
{

  u8 databuff;
  Set_Rs;

  do
  {
    databuff=*s++;  
    Lcd_WriteASCIIClarity(x,y,x_offset,y_offset,CharColor,databuff);
    
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<29)
    {
      x++;
    } 
    else if (y<19)
    {
      x=0;
      y++;
    }   
    else
    {
      x=0;
      y=0;
    }       
#else
    if (y<39)
    {
      y++;
    }
    else if (x<14)
    {
      y=0;
      x++;
    }
    else
    {
      x=0;
      y=0;
    }
#endif
    
  }
  while(*s!=0);
  
  Set_Cs;
  
}
/****************************************
д��32X16��ASCII�ַ���
*****************************************/
void Lcd_Write32X16String(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,char *s)
{

  u8 databuff;
  Set_Rs;
  
  do
  {
    databuff=*s++;  
    Lcd_Write32X16ASCII(x,y,x_offset,y_offset,CharColor,CharBackColor,databuff);
    
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<14)
    {
      x++;
    } 
    else if (y<9)
    {
      x=0;
      y++;
    }   
    else
    {
      x=0;
      y=0;
    }   
#else
    if (y<6)
    {
      y++;
    }
    else if (x<19)
    {
      y=0;
      x++;
    }
    else
    {
      x=0;
      y=0;
    }   
#endif
      
  }
  while (*s!=0);
    
  Set_Cs;
  
}
void Lcd_Write32X16StringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s)
{

  u8 databuff;
  Set_Rs;
  
  do
  {
    databuff=*s++;  
    Lcd_Write32X16ASCIIClarity(x,y,x_offset,y_offset,CharColor,databuff);

#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<14)
    {
      x++;
    } 
    else if (y<9)
    {
      x=0;
      y++;
    }   
    else
    {
      x=0;
      y=0;
    }     
#else
    if (y<6)
    {
      y++;
    }
    else if (x<19)
    {
      y=0;
      x++;
    }
    else
    {
      x=0;
      y=0;
    }
#endif
    
  }
  while(*s!=0);
  
  Set_Cs;
  
}
/****************************************************************
��������Lcdд1�����ĺ���
��ڲ�����x,�������꣬�����ҷֱ���0~15
          y,�������꣬���ϵ��·ֱ�Ϊ0~19
          CharColaor,�ַ�����ɫ 
          CharBackColor,�ַ�������ɫ 
         ASCIICode,��Ӧ���ĵı���
Ҳ����˵��320240�ֱ��ʵ���ʾ������������ʾ15�����ַ�����������ʾ20��
����ֵ����

ע�⣡���������������ʹ�ô˺�����Ӧ�ü���Lcd_Rs_H()��Set_Cs;Ϊ���Ż�ϵͳʡȥ��
���ָ�����˺���ִ�е���һ�������д�����RS_L�������д�뽫����
����ΪILI9320��Ϊ��RS_Lʱд���������
*****************************************************************/
void Lcd_WriteChinese(u8 x,u8 y,u16 x_offset,u16 y_offset,u16 CharColor,u16 CharBackColor,u16 ChineseCode)
{

  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];
        
  Lcd_SetBox(x*16,y*16,16,16,x_offset,y_offset);         
  Lcd_WR_Start();
  GetChineseCode(ChineseBuffer,ChineseCode,Chinese_Offset);
  ChinesePointer=ChineseBuffer;
        
  for(ByteCounter=0; ByteCounter<32; ByteCounter++)
  { 
    for(BitCounter=0;BitCounter<8;BitCounter++)
    {
      if((*ChinesePointer & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }     
    }
    ChinesePointer++;
  }
  //Set_Cs;
  
}
void Lcd_WriteChineseClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 ChineseCode)
{

  GPIO_InitTypeDef GPIO_InitStructure;
  u16 Temp;
  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];
  
  Lcd_SetBox(x*16,y*16,16,16,x_offset,y_offset);         
  Lcd_WR_Start();
  GetChineseCode(ChineseBuffer,ChineseCode,Chinese_Offset);
  ChinesePointer=ChineseBuffer;
  
  for(ByteCounter=0; ByteCounter<32; ByteCounter++)
  { 
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if((*ChinesePointer & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }     
    }
  ChinesePointer++;
  }
  //Set_Cs;
  
}
/****************************************************************
��������Lcdд1��32X32���ĺ���
��ڲ�����x,�������꣬�����ҷֱ���0~7
          y,�������꣬���ϵ��·ֱ�Ϊ0~9
          CharColaor,�ַ�����ɫ 
          CharBackColor,�ַ�������ɫ 
         ASCIICode,��Ӧ���ĵı���
Ҳ����˵��320240�ֱ��ʵ���ʾ������������ʾ7�����ַ�����������ʾ10��
����ֵ����

ע�⣡���������������ʹ�ô˺�����Ӧ�ü���Lcd_Rs_H()��Set_Cs;Ϊ���Ż�ϵͳʡȥ��
���ָ�����˺���ִ�е���һ�������д�����RS_L�������д�뽫����
����ΪILI9320��Ϊ��RS_Lʱд���������
*****************************************************************/
void Lcd_Write32X32Chinese(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,u16 ChineseCode)
{
        
  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];
  
  Lcd_SetBox(x*32,y*32,32,32,x_offset,y_offset);         
  Lcd_WR_Start();
  GetChineseCode(ChineseBuffer,ChineseCode,Chinese_Offset);
  ChinesePointer=ChineseBuffer;
  
  for(ByteCounter=0; ByteCounter<16; ByteCounter++)
  { 
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*ChinesePointer)&(0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
    for (BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*(ChinesePointer+1)) & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }          
    for (BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*ChinesePointer) & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }        
    for (BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*(ChinesePointer+1)) & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
    ChinesePointer+=2;
  }
  //Set_Cs;
 }
void Lcd_Write32X32ChineseClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 ChineseCode)
{
        
  GPIO_InitTypeDef GPIO_InitStructure;
  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];
  u16 Temp;

  Lcd_SetBox(x*32,y*32,32,32,x_offset,y_offset);         
  Lcd_WR_Start();
  GetChineseCode(ChineseBuffer,ChineseCode,Chinese_Offset);
  ChinesePointer=ChineseBuffer;       
  
  for(ByteCounter=0; ByteCounter<16; ByteCounter++)
  { 
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*ChinesePointer)&(0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                     
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
          
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*(ChinesePointer+1))&(0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                     
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }

          
          
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*ChinesePointer)&(0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                     
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
          
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*(ChinesePointer+1))&(0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                     
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
      ChinesePointer+=2;
  }
  //Set_Cs;
  
}
void Lcd_WriteChineseString(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,char *s)
{
   
  u8 databuffer;
  u16 ChineseCode;
  Set_Rs;
  
  do
  {
    databuffer=*s++;
    ChineseCode=databuffer<<8;
    ChineseCode=ChineseCode|*s++;
    Lcd_WriteChinese(x,y,x_offset,y_offset,CharColor,CharBackColor,ChineseCode);
    
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<14)
    {
      x++;
    }
    else if (y<19)
    {
      x=0;
      y++;
    }
    else
    {
      x=0;
      y=0;
    }     
#else
    if (y<19)
    {
      y++;
    }
    else if (x<14)
    {
      y=0;
      x++;
    }
    else
    {
      x=0;
      y=0;
    }  
#endif
      
  }
  while(*s!=0);
    
  Set_Cs;

}
void Lcd_WriteChineseStringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s)
{
   u8 databuffer;
   u16 ChineseCode;
   Set_Rs;
    do
   {
       databuffer=*s++;
       ChineseCode=databuffer<<8;
       ChineseCode=ChineseCode|*s++;
       Lcd_WriteChineseClarity(x,y,x_offset,y_offset,CharColor,ChineseCode);
       
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
       if (x<14)
       {
         x++;
       }
       else if (y<19)
       {
         x=0;
         y++;
       }
       else
       {
         x=0;
         y=0;
       }     
#else
       if (y<19)
       {
         y++;
       }
       else if (x<14)
       {
         y=0;
         x++;
       }
       else
       {
         x=0;
         y=0;
       }  
#endif
       
   }
     while(*s!=0);
    Set_Cs;
}
void Lcd_Write32X32ChineseString(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,char *s)
{
   
  u8 databuffer;
  u16 ChineseCode;
   
  Set_Rs;
  do
  {
    databuffer=*s++;
    ChineseCode=databuffer<<8;
    ChineseCode=ChineseCode|*s++;
    Lcd_Write32X32Chinese(x,y,x_offset,y_offset,CharColor,CharBackColor,ChineseCode);
       
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<6)
    {
      x++;
    }
    else if (y<9)
    {
      x=0;
      y++;
    }
    else
    {
      x=0;
      y=0;
    }     
#else
    if (y<9)
    {
      y++;
    }
    else if (x<6)
    {
      y=0;
      x++;
    }
    else
    {
      x=0;
      y=0;
    }  
#endif
     
  }
  while(*s!=0);

  Set_Cs;

}
void Lcd_Write32X32ChineseStringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s)
{
		u8 databuffer;
		u16 ChineseCode;
		Set_Rs;

		do
		{
			databuffer=*s++;
			ChineseCode=databuffer<<8;
			ChineseCode=ChineseCode|*s++;
			Lcd_Write32X32ChineseClarity(x,y,x_offset,y_offset,CharColor,ChineseCode);
    
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  

		if (x<6)
		{
			x++;
		}
		else if (y<9)
		{
			x=0;
			y++;
		}
		else
		{
			x=0;
			y=0;
		}     
#else
		if (y<9)
		{
			y++;
		}
		else if (x<6)
		{
			y=0;
			x++;
		}
			else
		{
			x=0;
			y=0;
		}  
#endif
    
	}
	while (*s!=0);
	
	Set_Cs;
}
/*********************************************************
���ǻ�ͼ����
**********************************************************/
void LcdWritePictureFromSPI(u8 xStart,u16 yStart,u8 xLong,u16 yLong,u32 BaseAddr)
{
	ColorTypeDef color;
	u32 pixels;
	
	Lcd_SetBox(xStart,yStart,xLong,yLong,0,0);
	Lcd_WR_Start();
	Set_Rs;
	
	SPI_FLASH_CS_LOW();
	SPI_FLASH_SendByte(0x0B);//FSTREAD
	SPI_FLASH_SendByte((BaseAddr & 0xFF0000) >> 16);
	SPI_FLASH_SendByte((BaseAddr& 0xFF00) >> 8);
	SPI_FLASH_SendByte(BaseAddr & 0xFF);
	SPI_FLASH_SendByte(0);//Dummy_Byte
	
	SPI2->DR = 0;//Dummy_Byte
	while((SPI2->SR & SPI_I2S_FLAG_RXNE) == (u16) RESET);
	color.U8[1] = SPI2->DR;	
	
	SPI2->DR = 0;//Dummy_Byte
	
	for (pixels=0; pixels<(xLong*yLong); pixels++)
	{

		while((SPI2->SR & SPI_I2S_FLAG_RXNE) == (u16) RESET);
		color.U8[0] = SPI2->DR;
		
		SPI2->DR = 0;//Dummy_Byte
		
		DataToWrite(color.U16);
		Clr_nWr;
		Set_nWr;
		
		while((SPI2->SR & SPI_I2S_FLAG_RXNE) == (u16) RESET);
		color.U8[1] = SPI2->DR;
		
		SPI2->DR = 0;//Dummy_Byte
	}

	SPI_FLASH_CS_HIGH();
	Set_Cs;  
}
/*********************************************************
��������SPIȡASCII���ӳ���
���������u8 ASCII �����ASCII�룬��'A'
          BaseAddr ��ַ ��ASCII��ʾ������FLASH�е���ʾλ��
����ֵ����
˵��������һ��ASCII�룬ȡ������SPI FLASH�е�16Byte��ʾ����
�������ŵ�һ��16byte��ASCII��ʾ����CharBuffer[]��
**********************************************************/
void GetASCIICode(u8* pBuffer,u8 ASCII,u32 BaseAddr)
{
  
  SPI_FLASH_BufferRead(pBuffer,BaseAddr+16*ASCII,16);

}
/*********************************************************
��������SPI������ʾ���ӳ���
���������u16 ASCII ��������ģ���"��"
          BaseAddr ���� ����ʾ������FLASH�е���ʼλ��
����ֵ����
˵��������һ�����ģ�ȡ������SPI FLASH�е�32Byte��ʾ����
�������ŵ�һ��32byte����ʾ����ChineseBuffer[]
**********************************************************/
void GetChineseCode(u8* pBuffer,u16 ChineseCode,u32 BaseAddr)
{
  
  u8 High8bit,Low8bit;
  u16 temp;
  
  temp=ChineseCode-0xA0A0;//���������λ��
  High8bit=(temp>>8);
  Low8bit=(temp&0x00FF);
  
  SPI_FLASH_BufferRead(pBuffer,BaseAddr+32*((High8bit-1)*94+Low8bit-1),32);

}
void Get320240PictureCode(u8* pBuffer,u32 BufferCounter,u32 BaseAddr)
{
    SPI_FLASH_BufferRead(pBuffer,BaseAddr+BufferCounter*32,32);
}
void Delay_nms(int n)
{
  
  u32 f=n,k;
  for (; f!=0; f--)
  {
    for(k=0xFFF; k!=0; k--);
  }
  
}
