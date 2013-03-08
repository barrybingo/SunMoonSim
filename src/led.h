/**
  ******************************************************************************
  * @file    led.h
  * @author  Barry Bingo
  * @brief   STM32-MINI-V3.0 LED control
  *          LED_1 connected to PB8
  *          LED_2 connected to PD9
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
 extern "C" {
#endif

 /* Exported constants --------------------------------------------------------*/

/* number of LED */
#define LEDn                             2

/* LED hardware configuration */
#define LED1_PIN                         GPIO_Pin_8
#define LED1_GPIO_PORT                   GPIOB
#define LED1_GPIO_CLK                    RCC_APB2Periph_GPIOB

#define LED2_PIN                         GPIO_Pin_9
#define LED2_GPIO_PORT                   GPIOB
#define LED2_GPIO_CLK                    RCC_APB2Periph_GPIOB

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
