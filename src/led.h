/**
  ******************************************************************************
  * @file    led.h
  * @author  Barry Bingo
  * @brief   STM32-MINI-V3.0 LED control
  *          LED_1 on-board LED1 @ A2
  *          LED_2 on-board LED1 @ A3
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __LED_H
#define __LED_H

 /* Exported constants --------------------------------------------------------*/

/* number of LED */
#define NUM_LEDS                             2

/* LED hardware configuration */
#define LED1_PIN                         GPIO_Pin_2
#define LED1_GPIO_PORT                   GPIOA
#define LED1_GPIO_CLK                    RCC_APB2Periph_GPIOA

#define LED2_PIN                         GPIO_Pin_3
#define LED2_GPIO_PORT                   GPIOA
#define LED2_GPIO_CLK                    RCC_APB2Periph_GPIOA

/* exported types -------------------------------------------------------------*/
typedef enum
{
  LED1 = 0,
  LED2 = 1,
} Led_TypeDef;

/* exported functions ---------------------------------------------------------*/
void LEDInit(Led_TypeDef Led);
void LEDOn(Led_TypeDef Led);
void LEDOff(Led_TypeDef Led);
void LEDToggle(Led_TypeDef Led);


#endif  /* __LED_H */
