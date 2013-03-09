/**
  ******************************************************************************
  * @file    key.h
  * @author  Barry Bingo
  * @brief   STM32-MINI-V3.0 push button access
  *          KEY_1 on-board KEY1 button @ A1
  *          KEY_2 on-board KEY2 button @ A0
  *
  *          Uses polling method via function KeyState(Key_TypeDef key)
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __KEY_H
#define __KEY_H

 /* Exported constants --------------------------------------------------------*/

/* number of LED */
#define NUM_KEYS                         2

/* KEY hardware configuration */
#define KEY1_PIN                         GPIO_Pin_1
#define KEY1_GPIO_PORT                   GPIOA
#define KEY1_GPIO_CLK                    RCC_APB2Periph_GPIOA

#define KEY2_PIN                         GPIO_Pin_0
#define KEY2_GPIO_PORT                   GPIOA
#define KEY2_GPIO_CLK                    RCC_APB2Periph_GPIOA

/* exported types -------------------------------------------------------------*/
typedef enum
{
  KEY1 = 0,
  KEY2 = 1
} Key_TypeDef;

typedef enum
{
  KEY_RELEASED = 0,
  KEY_PRESSED = 1,
} KeyState_TypeDef;

/* exported functions ---------------------------------------------------------*/

void KeyInit(Key_TypeDef key);
KeyState_TypeDef KeyState(Key_TypeDef key);

#endif __KEY_H
