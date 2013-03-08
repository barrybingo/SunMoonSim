/**
  ******************************************************************************
  * @file    led.c
  * @author  Barry Bingo
  * @brief   STM32-MINI-V3.0 LED control
  *
  *          Very easy to use.  First initialise the hardware then call On,Off or Toggle
  *          e.g.
  *
  *          LEDInit(LED1);
  *
  *          LEDOn(LED1);
  *          LEDOff(LED1);
  *          LEDToggle(LED1);
  ******************************************************************************
  */

#include "stm32f10x_conf.h"
#include "led.h"

GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN};
const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK, LED2_GPIO_CLK};

/**
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be configured.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void LEDInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable the GPIO_LED Clock */
  RCC_APB2PeriphClockCmd(GPIO_CLK[Led], ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN[Led];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);

  /* start with LED off */
  GPIO_PORT[Led]->BRR = GPIO_PIN[Led];
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void LEDOn(Led_TypeDef Led)
{
  GPIO_PORT[Led]->BSRR = GPIO_PIN[Led];
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void LEDOff(Led_TypeDef Led)
{
  GPIO_PORT[Led]->BRR = GPIO_PIN[Led];
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void LEDToggle(Led_TypeDef Led)
{
  GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
}
