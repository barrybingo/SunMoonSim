#ifndef  __ILI932X_H
#define  __ILI932X_H

#include "RGB.h"

#define LCD_W 240
#define LCD_H 320

#define LCD_CS  PCout(8)
#define LCD_RS	 PCout(9)
#define LCD_WR  PCout(10)
#define LCD_RD	 PCout(11)



/***********************************************************************************
9320/9325 LCD�Ĵ���
***********************************************************************************/  
#define R0             0x00
#define R1             0x01
#define R2             0x02
#define R3             0x03
#define R4             0x04
#define R5             0x05
#define R6             0x06
#define R7             0x07
#define R8             0x08
#define R9             0x09
#define R10            0x0A
#define R12            0x0C
#define R13            0x0D
#define R14            0x0E
#define R15            0x0F
#define R16            0x10
#define R17            0x11
#define R18            0x12
#define R19            0x13
#define R20            0x14
#define R21            0x15
#define R22            0x16
#define R23            0x17
#define R24            0x18
#define R25            0x19
#define R26            0x1A
#define R27            0x1B
#define R28            0x1C
#define R29            0x1D
#define R30            0x1E
#define R31            0x1F
#define R32            0x20
#define R33            0x21
#define R34            0x22
#define R36            0x24
#define R37            0x25
#define R40            0x28
#define R41            0x29
#define R43            0x2B
#define R45            0x2D
#define R48            0x30
#define R49            0x31
#define R50            0x32
#define R51            0x33
#define R52            0x34
#define R53            0x35
#define R54            0x36
#define R55            0x37
#define R56            0x38
#define R57            0x39
#define R59            0x3B
#define R60            0x3C
#define R61            0x3D
#define R62            0x3E
#define R63            0x3F
#define R64            0x40
#define R65            0x41
#define R66            0x42
#define R67            0x43
#define R68            0x44
#define R69            0x45
#define R70            0x46
#define R71            0x47
#define R72            0x48
#define R73            0x49
#define R74            0x4A
#define R75            0x4B
#define R76            0x4C
#define R77            0x4D
#define R78            0x4E
#define R79            0x4F
#define R80            0x50
#define R81            0x51
#define R82            0x52
#define R83            0x53
#define R96            0x60
#define R97            0x61
#define R106           0x6A
#define R118           0x76
#define R128           0x80
#define R129           0x81
#define R130           0x82
#define R131           0x83
#define R132           0x84
#define R133           0x85
#define R134           0x86
#define R135           0x87
#define R136           0x88
#define R137           0x89
#define R139           0x8B
#define R140           0x8C
#define R141           0x8D
#define R143           0x8F
#define R144           0x90
#define R145           0x91
#define R146           0x92
#define R147           0x93
#define R148           0x94
#define R149           0x95
#define R150           0x96
#define R151           0x97
#define R152           0x98
#define R153           0x99
#define R154           0x9A
#define R157           0x9D
#define R192           0xC0
#define R193           0xC1
#define R229           0xE5	

extern uint16_t  POINT_COLOR; //Ĭ�Ϻ�ɫ
extern uint16_t  BACK_COLOR;  //������ɫ.Ĭ��Ϊ��ɫ

/* �������� */
void LCD_WriteRAM_Prepare(void);
void LCD_SetCursor(uint8_t Xpos,uint16_t Ypos);
void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_Dat);
uint16_t LCD_ReadDat(void);
void Write_Cmd(uint16_t LCD_Reg);
void Write_Dat(uint16_t LCD_Dat);
uint16_t LCD_ReadReg(uint16_t LCD_Reg);
void LCD_Init(void);
void LCD_Configuration(void);
void LCD_Delay(uint32_t nCount);
uint32_t Num_power(uint8_t m,uint8_t n);
void LCD_SetDisplayWindow(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void LCD_ShowChar(uint8_t x,uint16_t y,uint8_t chars,uint8_t size,uint8_t mode);
void LCD_ShowCharBig(uint8_t x,uint16_t y,uint8_t chars, uint8_t scale, uint8_t mode);
void LCD_ShowNum(uint8_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size, uint8_t mode);
void LCD_ShowNumBig(uint8_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t scale);
void LCD_WriteBMP(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width, uint8_t *bitmap);
void LCD_WriteBMPx2(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width, uint8_t *bitmap);
void LCD_Clear(uint16_t Color);
uint16_t WriteOneASCII(uint8_t *pucMsk, uint16_t x0, uint16_t y0, uint16_t color, uint8_t transparent);
void WriteString(uint16_t x0, uint16_t y0,uint8_t *pcStr, uint16_t color, uint8_t transparent);
void LCD_Fill(uint8_t xsta,uint16_t ysta,uint8_t xend,uint16_t yend,uint16_t color);
#endif
/****************************** End File ********************************/
