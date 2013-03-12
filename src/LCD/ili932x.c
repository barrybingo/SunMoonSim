/**************************************************************
** 	��ţ������
**	LCD�����
**  �汾��V1.0  
**	��̳��www.openmcu.com
**	�Ա���http://shop36995246.taobao.com/   
**  ����֧��Ⱥ��121939788 
***************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_conf.h"
#include "hardware.h"
#include "ili932x.h"
#include "Font_Lib.h"	 //�ֿ��ļ���
#include "RGB.h"

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif


//
int printf( const char* format, ... ) { return 0; }


uint16_t POINT_COLOR = BLUE,BACK_COLOR = WHITE;  /* �ֱ����õ����ɫ�͵�ɫ	*/


/*****************************************************************************
** �������: LCD_Write_Reg
** ��������: дָ����
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_Dat)
{
	Write_Cmd(LCD_Reg);
	Write_Dat(LCD_Dat);
}
/*****************************************************************************
** �������: Write_Cmd
** ��������: дָ��
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void Write_Cmd(uint16_t LCD_Reg)
{
	LCD_CS = 0;
	LCD_RS = 0;
	GPIOC->ODR = (GPIOC->ODR&0xff00)|(LCD_Reg&0x00ff);
	GPIOB->ODR = (GPIOB->ODR&0x00ff)|(LCD_Reg&0xff00); 
	LCD_WR = 0;
	LCD_WR = 1;	
	LCD_CS = 1;
}
/*****************************************************************************
** �������: Write_Dat
** ��������: д���
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void Write_Dat(uint16_t LCD_Dat)
{
	LCD_CS = 0;
	LCD_RS = 1;
	GPIOC->ODR = (GPIOC->ODR&0xff00)|(LCD_Dat&0x00ff);
	GPIOB->ODR = (GPIOB->ODR&0x00ff)|(LCD_Dat&0xff00);
	LCD_WR = 0;
	LCD_WR = 1;	
	LCD_CS = 1;
}
/*****************************************************************************
** �������: LCD_ReadReg
** ��������: ��ָ��
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
	uint16_t temp;
	Write_Cmd(LCD_Reg);  //д��Ҫ���ļĴ�����  

	GPIOB->CRH = (GPIOB->CRH & 0x00000000) | 0x44444444; //���˿ڸ�8λ���ó�����
	GPIOC->CRL = (GPIOC->CRL & 0x00000000) | 0x44444444; //���˿ڵ�8λ���ó�����  
	LCD_CS = 0;
	LCD_RS = 1;
	LCD_RD = 0;
	temp = ((GPIOB->IDR&0xff00)|(GPIOC->IDR&0x00ff)); //��ȡ���(���Ĵ���ʱ,������Ҫ��2��) 	
	LCD_RD = 1;
	LCD_CS = 1; 
	GPIOB->CRH = (GPIOB->CRH & 0x00000000) | 0x33333333; //�ͷŶ˿ڸ�8λΪ���
	GPIOC->CRL = (GPIOC->CRL & 0x00000000) | 0x33333333; //�ͷŶ˿ڵ�8λΪ���
	return temp;   
}
/*****************************************************************************
** �������: LCD_ReadDat
** ��������: �����
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
uint16_t LCD_ReadDat()
{
	uint16_t temp;

	GPIOE->CRH = (GPIOE->CRH & 0x00000000) | 0x44444444;  //���˿ڸ�8λ���ó�����
	GPIOE->CRL = (GPIOE->CRL & 0x00000000) | 0x44444444;  //���˿ڵ�8λ���ó�����
	LCD_CS = 0;
	LCD_RS = 1;
	LCD_RD = 0;
	temp = ((GPIOB->IDR&0xff00)|(GPIOC->IDR&0x00ff));	//��ȡ���(���Ĵ���ʱ,������Ҫ��2��)
	LCD_RD = 1;
	LCD_CS = 1;
	GPIOE->CRH = (GPIOE->CRH & 0x00000000) | 0x33333333;  //�ͷŶ˿ڸ�8λΪ���
	GPIOE->CRL = (GPIOE->CRL & 0x00000000) | 0x33333333;  //�ͷŶ˿ڵ�8λΪ���

	return temp;   
}
/*****************************************************************************
** �������: LCD_Configuration
** ��������: LCD_IO������
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);

	/* �������IO ���ӵ�GPIOB *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 
								| GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // ���������ʽ
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // ���IO���������Ϊ50MHZ
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* ���ÿ���IO ���ӵ�PD12.PD13.PD14.PD15 *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 
								| GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 
								| GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // ���������ʽ
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // ���IO���������Ϊ50MHZ
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/*****************************************************************************
** �������: LCD_Init
** ��������: LCD��ʼ��
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_Init(void)
{
	static uint16_t DeviceCode;
	LCD_Configuration();
	LCD_WriteReg(0x0000,0x0001);
	LCD_Delay(5); // LCD_Delay 50 ms 
	DeviceCode = LCD_ReadReg(0x0000);   
	printf(" ID=0x%x\n",DeviceCode); 
	if(DeviceCode==0x9325||DeviceCode==0x9328)//ILI9325
	{
 		LCD_WriteReg(0x00e5,0x78F0);      
//        LCD_WriteReg(0x0000,0x0001);
        LCD_WriteReg(0x0001,0x0100);     
        LCD_WriteReg(0x0002,0x0700);                   
        LCD_WriteReg(0x0003,0x1030);    
        LCD_WriteReg(0x0004,0x0000);                                   
        LCD_WriteReg(0x0008,0x0202);	           
        LCD_WriteReg(0x0009,0x0000);         
        LCD_WriteReg(0x000a,0x0000);         
        LCD_WriteReg(0x000c,0x0001);         
        LCD_WriteReg(0x000d,0x0000);          
        LCD_WriteReg(0x000f,0x0000);
//Power On sequence //
        LCD_WriteReg(0x0010,0x0000);   
        LCD_WriteReg(0x0011,0x0007);
        LCD_WriteReg(0x0012,0x0000);                                                                 
        LCD_WriteReg(0x0013,0x0000);
        LCD_WriteReg(0x0007,0x0001);          
        LCD_Delay(5); 
        LCD_WriteReg(0x0010,0x1690);   
        LCD_WriteReg(0x0011,0x0227);
        LCD_Delay(5); 
        LCD_WriteReg(0x0012,0x009d);                   
        LCD_Delay(5); 
        LCD_WriteReg(0x0013,0x1900);   
        LCD_WriteReg(0x0029,0x0025);
        LCD_WriteReg(0x002b,0x000d);
        LCD_Delay(5); 
        LCD_WriteReg(0x0020,0x0000);                                                            
        LCD_WriteReg(0x0021,0x0000);           
		LCD_Delay(5); 
		//٤��У��
        LCD_WriteReg(0x0030,0x0007); 
        LCD_WriteReg(0x0031,0x0303);   
        LCD_WriteReg(0x0032,0x0003);
        LCD_WriteReg(0x0035,0x0206);
        LCD_WriteReg(0x0036,0x0008); 
        LCD_WriteReg(0x0037,0x0406);
        LCD_WriteReg(0x0038,0x0304);        
        LCD_WriteReg(0x0039,0x0007);     
        LCD_WriteReg(0x003c,0x0602);
        LCD_WriteReg(0x003d,0x0008);
        LCD_Delay(5); 
        LCD_WriteReg(0x0050,0x0000);
        LCD_WriteReg(0x0051,0x00ef);                   
        LCD_WriteReg(0x0052,0x0000);                   
        LCD_WriteReg(0x0053,0x013f); 
        
        LCD_WriteReg(0x0060,0xa700);        
        LCD_WriteReg(0x0061,0x0001); 
        LCD_WriteReg(0x006a,0x0000);
        LCD_WriteReg(0x0080,0x0000);
        LCD_WriteReg(0x0081,0x0000);
        LCD_WriteReg(0x0082,0x0000);
        LCD_WriteReg(0x0083,0x0000);
        LCD_WriteReg(0x0084,0x0000);
        LCD_WriteReg(0x0085,0x0000);
      
        LCD_WriteReg(0x0090,0x0010);     
        LCD_WriteReg(0x0092,0x0600);  
		   
        LCD_WriteReg(0x0007,0x0133);
	}
	else if(DeviceCode==0x9320||DeviceCode==0x9300)
	{
		LCD_WriteReg(0x00,0x0000);
		LCD_WriteReg(0x01,0x0100);	//Driver Output Contral.
		LCD_WriteReg(0x02,0x0700);	//LCD Driver Waveform Contral.
		LCD_WriteReg(0x03,0x1030);//Entry Mode Set.
		//LCD_WriteReg(0x03,0x1018);	//Entry Mode Set.
	
		LCD_WriteReg(0x04,0x0000);	//Scalling Contral.
		LCD_WriteReg(0x08,0x0202);	//Display Contral 2.(0x0207)
		LCD_WriteReg(0x09,0x0000);	//Display Contral 3.(0x0000)
		LCD_WriteReg(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
		LCD_WriteReg(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
		LCD_WriteReg(0x0d,0x0000);	//Frame Maker Position.
		LCD_WriteReg(0x0f,0x0000);	//Extern Display Interface Contral 2.	    
		LCD_Delay(10); 
		LCD_WriteReg(0x07,0x0101);	//Display Contral.
		LCD_Delay(10); 								  
		LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
		LCD_WriteReg(0x11,0x0007);								//Power Control 2.(0x0001)
		LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));				//Power Control 3.(0x0138)
		LCD_WriteReg(0x13,0x0b00);								//Power Control 4.
		LCD_WriteReg(0x29,0x0000);								//Power Control 7.
	
		LCD_WriteReg(0x2b,(1<<14)|(1<<4));	    
		LCD_WriteReg(0x50,0);	//Set X Star
		//ˮƽGRAM��ֹλ��Set X End.
		LCD_WriteReg(0x51,239);	//Set Y Star
		LCD_WriteReg(0x52,0);	//Set Y End.t.
		LCD_WriteReg(0x53,319);	//
	
		LCD_WriteReg(0x60,0x2700);	//Driver Output Control.
		LCD_WriteReg(0x61,0x0001);	//Driver Output Control.
		LCD_WriteReg(0x6a,0x0000);	//Vertical Srcoll Control.
	
		LCD_WriteReg(0x80,0x0000);	//Display Position? Partial Display 1.
		LCD_WriteReg(0x81,0x0000);	//RAM Address Start? Partial Display 1.
		LCD_WriteReg(0x82,0x0000);	//RAM Address End-Partial Display 1.
		LCD_WriteReg(0x83,0x0000);	//Displsy Position? Partial Display 2.
		LCD_WriteReg(0x84,0x0000);	//RAM Address Start? Partial Display 2.
		LCD_WriteReg(0x85,0x0000);	//RAM Address End? Partial Display 2.
	
		LCD_WriteReg(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
		LCD_WriteReg(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
		LCD_WriteReg(0x93,0x0001);	//Panel Interface Contral 3.
		LCD_WriteReg(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
		LCD_WriteReg(0x97,(0<<8));	//
		LCD_WriteReg(0x98,0x0000);	//Frame Cycle Contral.	   
		LCD_WriteReg(0x07,0x0173);	//(0x0173)
		LCD_Delay(10);
	}
	LCD_Clear(BACK_COLOR);	
}
/*****************************************************************************
** �������: LCD_DrawPoint
** ��������: дһ����
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	LCD_SetCursor(x,y);//���ù��λ�� 
	Write_Cmd(R34);//��ʼд��GRAM
	Write_Dat(POINT_COLOR); 	
}
/*****************************************************************************
** �������: LCD_WriteRAM_Prepare
** ��������: Щ׼��
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_WriteRAM_Prepare()
{
	Write_Cmd(R34);	
}
/*****************************************************************************
** �������: LCD_SetCursor
** ��������: ���ù�꺯��
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_SetCursor(uint8_t Xpos,uint16_t Ypos)
{
	LCD_WriteReg(R32, Xpos);
	LCD_WriteReg(R33, Ypos);	
} 
/*****************************************************************************
** �������: LCD_SetDisplayWindow
** ��������: ���ô��ں��� (��Width��Height����һ�£�ǰ����)
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_SetDisplayWindow(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
  	LCD_WriteReg(R80, Xpos);	   	   	//ˮƽ����GRAM��ʼ��ַ
  	LCD_WriteReg(R81, Xpos+Height); 	//ˮƽ����GRAM�����ַ 
  	LCD_WriteReg(R82, Ypos);		  	//��ֱ����GRAM��ʼ��ַ
  	LCD_WriteReg(R83, Ypos+Width);  	//��ֱ����GRAM�����ַ

  	LCD_SetCursor(Xpos, Ypos);			//���ù��λ��
}
/*****************************************************************************
** �������: LCD_ShowString
** ��������: ��ʾ�ַ���
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304 
void LCD_ShowString(uint8_t x,uint16_t y,__I uint8_t *p)
{         
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;LCD_Clear(WHITE);}
        LCD_ShowChar(x,y,*p,16,0);
        x+=8;
        p++;
    }  
}
/*****************************************************************************
** �������: LCD_ShowChar
** ��������: ��ʾһ���ַ���
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_ShowCharBig(uint8_t x,uint16_t y,uint8_t chars, uint8_t scale, uint8_t mode)
{
	uint8_t temp;
    uint16_t pos,t;

    if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY) return;	    
											
	if(!mode)
	{
		LCD_Fill(x, y, x+(scale*8), y+(scale*16), BACK_COLOR);
	}

	for(pos=0;pos<16;pos++)
	{
		uint16_t Y = y+(pos * scale);
		temp=ASCII_1608[chars-0x20][pos];
		for(t=0;t<8;t++)
		{
			uint16_t X = x+(t*scale);
			if((temp<<t)&0x80) {
				LCD_Fill(X, Y, X+scale, Y+scale, POINT_COLOR);
			}
		}
	}
}

void LCD_ShowChar(uint8_t x,uint16_t y,uint8_t chars,uint8_t size,uint8_t mode)
{
	uint8_t temp;
    uint8_t pos,t;
    if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY) return;

	LCD_SetDisplayWindow(x,y,(size/2-1),size-1);  //���ô���

	LCD_WriteRAM_Prepare();        //��ʼд��GRAM
	if(!mode) 						//�ǵ��ӷ�ʽ
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=ASCII_1206[chars-0x20][pos];//����1206����
			else temp=ASCII_1608[chars-0x20][pos];		 //����1608����
			for(t=0;t<size/2;t++)
		    {
		        //if(temp&0x01)            	 			//�ȴ���λ��ȡģ�й�ϵ
		        if((temp<<t)&0x80)						//�ȴ���λ
				{
					Write_Dat(RED);
				}
				else
				{
					Write_Dat(WHITE);
		        }
				//temp>>=1; 	   							//����ȴ���λ��ȥ��������
		    }
		}
	}
	else//���ӷ�ʽ
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=ASCII_1206[chars-0x20][pos];	//����1206����
			else temp=ASCII_1608[chars-0x20][pos];		 	//����1608����
			for(t=0;t<size/2;t++)
		    {
		        if((temp<<t)&0x80)LCD_DrawPoint(x+t,y+pos);	//��һ����
		        //temp>>=1; 								//����ȴ���λ��ȥ��������
		    }
		}
	}
	/* �ָ������С	*/
	LCD_WriteReg(R80, 0x0000); //ˮƽ����GRAM��ʼ��ַ
	LCD_WriteReg(R81, 0x00EF); //ˮƽ����GRAM�����ַ
	LCD_WriteReg(R82, 0x0000); //��ֱ����GRAM��ʼ��ַ
	LCD_WriteReg(R83, 0x013F); //��ֱ����GRAM�����ַ
}
/*****************************************************************************
** �������: LCD_Clear
** ��������: ����Ļ����
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_Clear(uint16_t Color)
{
	uint32_t index=0;      
	LCD_SetCursor(0x00,0x0000); //���ù��λ�� 
	LCD_WriteRAM_Prepare();     //��ʼд��GRAM	 	  
	for(index=0;index<76800;index++)
	{
		Write_Dat(Color);    
	}
}
/*****************************************************************************
** �������: WriteString
** ��������: ��ָ��λ�ÿ�ʼ��ʾһ���ַ��һ������
				֧���Զ�����
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/   			   
void WriteString(uint16_t x0, uint16_t y0,uint8_t *pcStr, uint16_t color, uint8_t transparent)
{
    uint16_t usWidth = 0;

    while(1)
    {
        if(*pcStr == 0) 
		{
            break;                                     /* �ַ����            */
        }      
        x0 = x0 + (usWidth);                           /* �����ַ���ʾ�ɽ���         */
        if(*pcStr > 0x80)                              /* �ж�Ϊ����                   */
        {
		    if((x0 + 16) > LCD_W)                      /* ���ʣ��ռ��Ƿ��㹻         */
            {
			    x0 = 0;
                y0 = y0 + 16;                          /* �ı���ʾ���                 */
                if(y0 > LCD_H)                         /* ����곬��                   */
                {
				    y0 = 0;
                }
            }
		    usWidth = WriteOneASCII((uint8_t *)&ASCII_1608[('*' - 0x20)][0], x0, y0, color, transparent);
            pcStr += 1;
        }
		else 
		{                                               /* �ж�Ϊ�Ǻ���                 */
            if (*pcStr == '\r')                         /* ����                         */
            { 
			    y0 = y0 + 16;                           /* �ı���ʾ���                 */
                if(y0 > LCD_H)                          /* ����곬��                   */
                {
				    y0 = 0;
                }
                pcStr++;
                usWidth = 0;
                continue;
            } 
			else if (*pcStr == '\n')                    /* ���뵽���                   */
            {
			    x0 = 0;
                pcStr++;
                usWidth = 0;
                continue;
            } 
			else 
			{
                if((x0 + 8) > LCD_W)                     /* ���ʣ��ռ��Ƿ��㹻         */
                {
				    x0 = 0;
                    y0 = y0 + 16;                        /* �ı���ʾ���                 */
                    if(y0 > LCD_H)                       /* ����곬��                   */
                    { 
					    y0 = 0;
                    }
                }
                usWidth = WriteOneASCII((uint8_t *)&ASCII_1608[(*pcStr - 0x20)][0], x0, y0, color, transparent);
                                                         /* ASCII���21H��ֵ��Ӧ��λ��3��*/
                pcStr += 1;
            }
		}
	}												  	  
}

/*****************************************************************************
** �������: WriteOneASCII
** ��������: ��ʾһ��ָ����С���ַ�
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
uint16_t WriteOneASCII(uint8_t *pucMsk,
                              uint16_t x0,
                              uint16_t y0,
                              uint16_t color,
                              uint8_t transparent)
{
    uint16_t i,j;
    uint16_t y;
    uint8_t ucChar;
    
    y = y0;
    for(i=0; i<16; i++) {                                 /* 16��                         */
        ucChar = *pucMsk++;
        #ifdef __DISPLAY_BUFFER                           /* ʹ���Դ���ʾ                 */
        for(j=0; j<8; j++) {                              /* 8��                          */
            if((ucChar << j)& 0x80) {                     /* ��ʾ��ģ                     */
                DispBuf[240*(y0+i) + x0+j] = color;
            }
        }
        #else                                             /* ֱ����ʾ                     */
        
        LCD_SetCursor(x0, y);                             /* ����д��ݵ�ַָ��           */
		LCD_WriteRAM_Prepare();        					  /* ��ʼд��GRAM	    		  */
        for(j=0; j<8; j++) {                              /* 8��                          */
            if((ucChar << j) & 0x80) {                    /* ��ʾ��ģ                     */
                Write_Dat(color);
            } else {
            	if (transparent==0)
            		Write_Dat(BACK_COLOR);
            	else
            	{
            		/* skip this one */
                    LCD_SetCursor(x0+j+1, y);
            		LCD_WriteRAM_Prepare();
            	}
            }
        }
        y++;
        #endif
    }
    return (8);                                           /* ����16λ�п�                 */
}
/*****************************************************************************
** �������: Num_power
** ��������: M��N�η�
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
uint32_t Num_power(uint8_t m,uint8_t n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}
/*****************************************************************************
** �������: LCD_ShowNum
** ��������: ��ָ��λ����ʾһ������
				num:��ֵ(0~4294967295);
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_ShowNumBig(uint8_t x,uint16_t y,u32 num,uint8_t len,uint8_t scale)
{
	uint8_t t,temp;
	uint8_t enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/Num_power(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowCharBig(x+(8*scale)*t,y,' ',scale,0);
				continue;
			}else enshow=1;

		}
	 	LCD_ShowCharBig(x+(8*scale)*t,y,temp+'0',scale,0);
	}
}
void LCD_ShowNum(uint8_t x,uint16_t y,u32 num,uint8_t len,uint8_t size, uint8_t mode)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/Num_power(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,mode);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode);
	}
}
/*****************************************************************************
** �������: LCD_WriteBMP
** ��������: ��ָ����λ����ʾһ��ͼƬ
				Xpos��YposΪͼƬ��ʾ��ַ��Height��Width ΪͼƬ�Ŀ�Ⱥ͸߶�
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_WriteBMP(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width, uint8_t *bitmap)
{
  	uint32_t index;
  	uint32_t size = Height * Width;
  	uint16_t *bitmap_ptr = (uint16_t *)bitmap;

  	LCD_SetDisplayWindow(Xpos, Ypos, Width-1, Height-1);

  	//LCD_WriteReg(0x03, 0x1038);	//�����Ҫ������ʾͼƬ������ȥ������ ��ͬʱ��Width��Hight����һ�¾Ϳ���

  	LCD_WriteRAM_Prepare();

  	for(index = 0; index < size; index++)
  	{
    	Write_Dat(*bitmap_ptr++);
  	}
	//�ָ������С	 
	LCD_WriteReg(R80, 0x0000); //ˮƽ����GRAM��ʼ��ַ
	LCD_WriteReg(R81, 0x00EF); //ˮƽ����GRAM�����ַ
	LCD_WriteReg(R82, 0x0000); //��ֱ����GRAM��ʼ��ַ
	LCD_WriteReg(R83, 0x013F); //��ֱ����GRAM�����ַ
}

/*
 * Write bitmap of size HeightxWidth to Xpos,Ypos with double sized output dimensions
 */
void LCD_WriteBMPx2(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width, uint8_t *bitmap)
{
  	uint32_t x,y;

  	LCD_SetDisplayWindow(Xpos, Ypos, (2*Width)-1, (2*Height)-1);
  	LCD_WriteRAM_Prepare();

  	for(x = 0; x < Height; x++)
  	{
  		for(y = 0; y < Width; y++)
  		{
  			uint16_t c = ((uint16_t *)bitmap)[(x*Width)+y];
  			Write_Dat(c);
  			Write_Dat(c);
  		}
  		for(y = 0; y < Width; y++)
  		{
  			uint16_t c = ((uint16_t *)bitmap)[(x*Width)+y];
  			Write_Dat(c);
  			Write_Dat(c);
  		}
  	}
	//�ָ������С
	LCD_WriteReg(R80, 0x0000); //ˮƽ����GRAM��ʼ��ַ
	LCD_WriteReg(R81, 0x00EF); //ˮƽ����GRAM�����ַ
	LCD_WriteReg(R82, 0x0000); //��ֱ����GRAM��ʼ��ַ
	LCD_WriteReg(R83, 0x013F); //��ֱ����GRAM�����ַ
}


/*****************************************************************************
** �������: LCD_DrawLine
** ��������: ��ָ��������λ�û�һ����
				x1,y1:�������  x2,y2:�յ����
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; 				//����������� 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; 		//���õ������� 
	else if(delta_x==0)incx=0;	//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;	//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ����������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )	//������� 
	{  
		LCD_DrawPoint(uRow,uCol);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
/*****************************************************************************
** �������: LCD_DrawLine
** ��������: ��ָ��λ�û�һ��ָ����С��Բ
				(x,y):���ĵ� 	 r    :�뾶
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void Draw_Circle(uint8_t x0,uint16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a);             //3           
		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0-a,y0+b);             //1       
		LCD_DrawPoint(x0-b,y0-a);             //7           
		LCD_DrawPoint(x0-a,y0-b);             //2             
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0-b);             //5
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-b,y0+a);             
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a,y0+b);
	}
} 
/*****************************************************************************
** �������: LCD_Fill
** ��������: ��ָ�����������ָ����ɫ
				�����С:  (xend-xsta)*(yend-ysta) 
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_Fill(uint8_t xsta,uint16_t ysta,uint8_t xend,uint16_t yend,uint16_t color)
{                    
    uint32_t n;
	//���ô���										
	LCD_WriteReg(R80, xsta); //ˮƽ����GRAM��ʼ��ַ
	LCD_WriteReg(R81, xend); //ˮƽ����GRAM�����ַ
	LCD_WriteReg(R82, ysta); //��ֱ����GRAM��ʼ��ַ
	LCD_WriteReg(R83, yend); //��ֱ����GRAM�����ַ	
	LCD_SetCursor(xsta,ysta);//���ù��λ��  
	LCD_WriteRAM_Prepare();  //��ʼд��GRAM	 	   	   
	n=(u32)(yend-ysta+1)*(xend-xsta+1);    
	while(n--){Write_Dat(color);}//��ʾ��������ɫ. 
	//�ָ�����
	LCD_WriteReg(R80, 0x0000); //ˮƽ����GRAM��ʼ��ַ
	LCD_WriteReg(R81, 0x00EF); //ˮƽ����GRAM�����ַ
	LCD_WriteReg(R82, 0x0000); //��ֱ����GRAM��ʼ��ַ
	LCD_WriteReg(R83, 0x013F); //��ֱ����GRAM�����ַ	    
}
/*****************************************************************************
** �������: LCD_Delay
** ��������: ����LCD����ʱ
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void LCD_Delay (uint32_t nCount)
{
	__IO uint16_t i;	 	
	for (i=0;i<nCount*100;i++);
}
/*********************************************************************************************************
** End of File
*********************************************************************************************************/
