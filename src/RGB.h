#ifndef __RGB_H_
#define __RGB_H_

#include "stdint.h"

typedef uint16_t COLOR;

/*  RGB values for rainbow colours and black holes */
#define   BLACK        0x0000                    /* ��ɫ�� 0, 0, 0               */
#define   NAVY         0x000F                    /* ����ɫ�� 0, 0, 128           */
#define   DGREEN       0x03E0                    /* ����ɫ�� 0, 128, 0           */
#define   DCYAN        0x03EF                    /* ����ɫ�� 0, 128, 128         */
#define   MAROON       0x7800                    /* ���ɫ��128, 0, 0            */
#define   PURPLE       0x780F                    /* ��ɫ�� 128, 0, 128           */
#define   OLIVE        0x7BE0                    /* ����̣�128, 128, 0          */
#define   LGRAY        0xC618                    /* �Ұ�ɫ��192, 192, 192        */
#define   DGRAY        0x7BEF                    /* ���ɫ��128, 128, 128        */
#define   ORANGE       0xFB20					  /* 255,102,0 */
#define   BLUE         0x001F                    /* ��ɫ�� 0, 0, 255             */
#define   GREEN        0x07E0                 	 /* ��ɫ�� 0, 255, 0             */
#define   CYAN         0x07FF                    /* ��ɫ�� 0, 255, 255           */
#define   RED          0xF800                    /* ��ɫ�� 255, 0, 0             */
#define   MAGENTA      0xF81F                    /* Ʒ�죺 255, 0, 255           */
#define   YELLOW       0xFFE0                    /* ��ɫ�� 255, 255, 0           */
#define   WHITE        0xFFFF                    /* ��ɫ�� 255, 255, 255         */
#define   IDMCOLOR(color) (((color & 0x001F) << 11) | ((color & 0xF800) >> 11) | (color & 0x07E0))

#define RGB(red,green,blue) (((uint16_t)red&0xf8) << 8) | (((uint16_t)green&0xfc) << 3) | (blue&0xf8 >> 3)

#endif
