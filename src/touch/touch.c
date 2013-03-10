/**************************************************************
** 	��ţ������
**	���ܽ��ܣ� �����������
**  �汾��V1.0  
**	��̳��www.openmcu.com
**	�Ա���http://shop36995246.taobao.com/   
**  ����֧��Ⱥ��121939788 
***************************************************************/ 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_conf.h"
#include "ili932x.h"
#include "RGB.h"
#include "hardware.h"
#include "touch.h"

//#define ADJ_SAVE_ENABLE

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
Pen_Holder Pen_Point;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
EXTI_InitTypeDef EXTI_InitStructure;

extern void Delay(__IO uint32_t nTime);
//SPIд���
//��7843д��1byte���   
void ADS_Write_Byte(uint8_t num)    
{  
	uint8_t count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  
		else TDIN=0;   
		num<<=1;    
		TCLK=0;//��������Ч	   	 
		TCLK=1;      
	} 			    
} 		 
//SPI����� 
//��7846/7843/XPT2046/UH7843/UH7846��ȡadcֵ	   
uint16_t ADS_Read_AD(uint8_t CMD)	  
{ 	 
	uint8_t i;
	uint8_t count=0; 	  
	uint16_t Num=0; 
	TCLK=0;//������ʱ�� 	 
	TCS=0; //ѡ��ADS7843	 
	ADS_Write_Byte(CMD);//����������
	for(i=100;i>0;i--);
//	delay_us(6);//ADS7846��ת��ʱ���Ϊ6us
	TCLK=1;//��1��ʱ�ӣ����BUSY   	    
	TCLK=0; 	 
	for(count=0;count<16;count++)  
	{ 				  
		Num<<=1; 	 
		TCLK=0;//�½�����Ч  	    	   
		TCLK=1;
		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   //ֻ�и�12λ��Ч.
	TCS=1;//�ͷ�ADS7843	 
	return(Num);   
}
//��ȡһ�����ֵ
//�����ȡREAD_TIMES�����,����Щ�����������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
#define READ_TIMES 15 //��ȡ����
#define LOST_VAL 5	  //����ֵ
uint16_t ADS_Read_XY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)
	{				 
		buf[i]=ADS_Read_AD(xy);	    
	}				    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
//���˲�������ȡ
//��Сֵ��������100.
uint8_t Read_ADS(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=ADS_Read_XY(CMD_RDX);
	ytemp=ADS_Read_XY(CMD_RDY);	  												   
	if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;//����ɹ�
}
//2�ζ�ȡADS7846,�����ȡ2����Ч��ADֵ,�������ε�ƫ��ܳ���
//50,��������,����Ϊ������ȷ,����������.	   
//�ú����ܴ�����׼ȷ��
#define ERR_RANGE 50 //��Χ 
uint8_t Read_ADS2(uint16_t *x,uint16_t *y)
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t flag;    
    flag=Read_ADS(&x1,&y1);   
    if(flag==0)return(0);
    flag=Read_ADS(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 
//��ȡһ�����ֵ	
//������ȡһ��,֪��PEN�ɿ��ŷ���!					   
uint8_t Read_TP_Once(void)
{
	uint8_t t=0;	    
	Pen_Int_Set(0);//�ر��ж�
	Pen_Point.Key_Sta=Key_Up;
	Read_ADS2(&Pen_Point.X,&Pen_Point.Y);
	while(PEN==0&&t<=250)
	{
		t++;
		Delay(10);
	};
	Pen_Int_Set(1);//�����ж�		 
	if(t>=250)return 0;//����2.5s ��Ϊ��Ч
	else return 1;	
}

//////////////////////////////////////////////////
//��LCD�����йصĺ���  
//��һ��������
//����У׼�õ�
void Drow_Touch_Point(uint8_t x,uint16_t y)
{
	LCD_DrawLine(x-12,y,x+13,y);//����
	LCD_DrawLine(x,y-12,x,y+13);//����
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	Draw_Circle(x,y,6);//������Ȧ
}	  
//��һ�����
//2*2�ĵ�			   
void Draw_Big_Point(uint8_t x,uint16_t y)
{	    
	LCD_DrawPoint(x,y);//���ĵ� 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}

//ת�����
//��ݴ�������У׼����������ת����Ľ��,������X0,Y0��
void Convert_Pos(void)
{		 	  
	if(Read_ADS2(&Pen_Point.X,&Pen_Point.Y))
	{
		Pen_Point.X0=Pen_Point.xfac*Pen_Point.X+Pen_Point.xoff;
		Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y+Pen_Point.yoff;  
	}
}
//������У׼����
//�õ��ĸ�У׼����
void Touch_Adjust(void)
{								 
	uint16_t pos_temp[4][2];//��껺��ֵ
	uint8_t  cnt=0;	
	uint16_t d1,d2;
	uint32_t tem1,tem2;
	float fac; 	   
	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(WHITE);//����   
	POINT_COLOR=RED;//��ɫ 
	LCD_Clear(WHITE);//���� 
	Drow_Touch_Point(20,20);//����1 
	Pen_Point.Key_Sta=Key_Up;//����ź� 
	Pen_Point.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)
	{
		if(Pen_Point.Key_Sta==Key_Down)//��������
		{
			if(Read_TP_Once())//�õ����ΰ���ֵ
			{  								   
				pos_temp[cnt][0]=Pen_Point.X;
				pos_temp[cnt][1]=Pen_Point.Y;
				cnt++;
			}			 
			switch(cnt)
			{			   
				case 1:
					LCD_Clear(WHITE);//���� 
					Drow_Touch_Point(220,20);//����2
					break;
				case 2:
					LCD_Clear(WHITE);//���� 
					Drow_Touch_Point(20,300);//����3
					break;
				case 3:
					LCD_Clear(WHITE);//���� 
					Drow_Touch_Point(220,300);//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
	    		    	//�Ա����
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2); // distance between x1,y1 and x2,y2
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2); // distance between x3,y3 and x4,y4

					// check factor of error between horizontal pairs
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
					{
						cnt=0;
						LCD_Clear(WHITE);//���� 
						Drow_Touch_Point(20,20);
						continue;
					}

					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���

					// check factor of error between vertical pairs
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
						LCD_Clear(WHITE);//���� 
						Drow_Touch_Point(20,20);
						continue;
					}//��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x2-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y2-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x1-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y1-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���

					// check factor of error between diagonal pairs
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
						LCD_Clear(WHITE);//���� 
						Drow_Touch_Point(20,20);
						continue;
					}

					// everything is square enough
					Pen_Point.xfac=(float)200/(pos_temp[1][0]-pos_temp[0][0]);
					Pen_Point.xoff=(240-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;
						  
					Pen_Point.yfac=(float)280/(pos_temp[2][1]-pos_temp[0][1]);
					Pen_Point.yoff=(320-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;

					POINT_COLOR=BLUE;
					LCD_Clear(WHITE);
					LCD_ShowString(35,110,"Touch Screen Adjust OK!");
					Delay(500);
					LCD_Clear(WHITE);
					return;
			}
		}
	} 
}
/*****************************************************************************
** �������: Touch_Init
** ��������: ��������ʼ��
				��У׼���
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void Touch_Init()
{
	Touch_Configuration();
 	Read_ADS(&Pen_Point.X,&Pen_Point.Y);//��һ�ζ�ȡ��ʼ��			 

  	/* Connect PEN EXTI Line to Key Button GPIO Pin */
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);

  	/* Configure PEN EXTI Line to generate an interrupt on falling edge */  
  	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);

  	/* Generate software interrupt: simulate a falling edge applied on PEN EXTI line */
  	EXTI_GenerateSWInterrupt(EXTI_Line13);

#ifdef ADJ_SAVE_ENABLE
	if(Get_Adjdata())return;//�Ѿ�У׼
	else			   //δУ׼?
	{ 										    
		LCD_Clear(WHITE);//����
	    Touch_Adjust();  //��ĻУ׼ 
		Save_Adjdata();	 
	}			
	Get_Adjdata();	 
#else
	/* use hardcoded values */
	Pen_Point.xfac=0.124688283;
	Pen_Point.xoff=-8;

	Pen_Point.yfac=0.166468486;
	Pen_Point.yoff=-8;
#endif			   
}
/*****************************************************************************
** �������: Touch_Configuration
** ��������: ������IO����
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void Touch_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO
							, ENABLE );  //��Ҫ����

	//������SPI���GPIO��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //ͨ���������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//������SPI���GPIO��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//Configure PA4 pin: TP_CS pin 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//ͨ���������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//Configure PC13 pin: TP_INT pin 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	//��������
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}
/*****************************************************************************
** �������: Pen_Int_Set
** ��������: PEN�ж�����
				EN=1�������ж�
					EN=0: �ر��ж�
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/	 
void Pen_Int_Set(uint8_t en)
{
	if(en)EXTI->IMR|=1<<13;   //����line13�ϵ��ж�	  	
	else EXTI->IMR&=~(1<<13); //�ر�line13�ϵ��ж�	   
}

#ifdef ADJ_SAVE_ENABLE
#define SAVE_ADDR_BASE 40
/*****************************************************************************
** �������: Save_Adjdata
** ��������: ����У׼����EEPROAM�е� ��ַ40
				�˲����漰��ʹ���ⲿEEPROM,���û���ⲿEEPROM,���δ˲��ּ���
				������EEPROM����ĵ�ַ����ַ,ռ��13���ֽ�(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+12)
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void Save_Adjdata(void)
{
	uint8_t temp[4];
	//����У����!		   							  
	temp[0]=(uint32_t)(Pen_Point.xfac*100000000)&0xff;	//����xУ������    
	temp[1]=(uint32_t)(Pen_Point.xfac*100000000)>>8&0xff;//����xУ������    
	temp[2]=(uint32_t)(Pen_Point.xfac*100000000)>>16&0xff;//����xУ������        
	temp[3]=(uint32_t)(Pen_Point.xfac*100000000)>>24&0xff;//����xУ������        
   	I2C_Write(&temp[0],SAVE_ADDR_BASE,4);

	temp[0]=(uint32_t)(Pen_Point.yfac*100000000)&0xff;//����xУ������    
	temp[1]=(uint32_t)(Pen_Point.yfac*100000000)>>8&0xff;//����xУ������    
	temp[2]=(uint32_t)(Pen_Point.yfac*100000000)>>16&0xff;//����xУ������        
	temp[3]=(uint32_t)(Pen_Point.yfac*100000000)>>24&0xff;//����xУ������ 

   	I2C_Write(&temp[0],SAVE_ADDR_BASE+4,4);
	temp[0]=(uint32_t)(Pen_Point.xoff*100000000)&0xff;//����xУ������    
	temp[1]=(uint32_t)(Pen_Point.xoff*100000000)>>8&0xff;//����xУ������    
   	I2C_Write(&temp[0],SAVE_ADDR_BASE+8,2);
	//����xƫ����
	//����yƫ����
	temp[0]=(uint32_t)(Pen_Point.yoff*100000000)&0xff;//����xУ������    
	temp[1]=(uint32_t)(Pen_Point.yoff*100000000)>>8&0xff;//����xУ������    
   	I2C_Write(&temp[0],SAVE_ADDR_BASE+10,2);
	I2C_Read(&temp[0],SAVE_ADDR_BASE+12,1);
	temp[0]&=0XF0;
	temp[0]|=0X0A;//���У׼����
	I2C_Write(&temp[0],SAVE_ADDR_BASE+12,1);			 
}
/*****************************************************************************
** �������: Get_Adjdata
** ��������: �õ�������EEPROM�����У׼ֵ
				����ֵ��1���ɹ���ȡ���
						 0����ȡʧ�ܣ�Ҫ����У׼
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/       
uint8_t Get_Adjdata(void)
{	
	uint8_t temp[4];
	uint32_t tempfac=0;
	I2C_Read(&temp[0],52,1); //����ʮ���ֽڵĵ���λ��������Ƿ�У׼�� 		 
	if((temp[0]&0X0F)==0X0A)		//�������Ѿ�У׼����			   
	{   
		I2C_Read(&temp[0],40,4);
		tempfac = temp[0]|(temp[1]<<8)|(temp[2]<<16)|(temp[3]<<24);	   
		Pen_Point.xfac=(float)tempfac/100000000;//�õ�xУ׼����
		I2C_Read(&temp[0],44,4);
		tempfac = temp[0]|(temp[1]<<8)|(temp[2]<<16)|(temp[3]<<24);	   
			          
		Pen_Point.yfac=(float)tempfac/100000000;//�õ�yУ׼����
	    //�õ�xƫ����
		I2C_Read(&temp[0],48,2);		
		tempfac = temp[0]|(temp[1]<<8);	   		   	  
		Pen_Point.xoff=tempfac;					 
	    //�õ�yƫ����
		I2C_Read(&temp[0],50,2);
		tempfac = temp[0]|(temp[1]<<8);	   
						 	  
		Pen_Point.yoff=tempfac;					 
		return 1;	 
	}
	return 0;
}
#endif
/*********************************************************************************************************
** End of File
*********************************************************************************************************/
