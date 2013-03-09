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

GPIO_TypeDef* LED_GPIO_PORT[NUM_LEDS] = {LED1_GPIO_PORT, LED2_GPIO_PORT};
const uint16_t LED_GPIO_PIN[NUM_LEDS] = {LED1_PIN, LED2_PIN};
const uint32_t LED_GPIO_CLK[NUM_LEDS] = {LED1_GPIO_CLK, LED2_GPIO_CLK};

/**
  * @brief  Configures LED GPIO.
  * @param  led: Specifies the LED to be configured.
  * @retval None
  */
void LEDInit(Led_TypeDef led)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable the GPIO_LED Clock */
  RCC_APB2PeriphClockCmd(LED_GPIO_CLK[led], ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN[led];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(LED_GPIO_PORT[led], &GPIO_InitStructure);

  /* start with LED off */
  LED_GPIO_PORT[led]->BRR = LED_GPIO_PIN[led];
}

/**
  * @brief  Turns selected LED on.
  * @param  led: Specifies the LED to be set on.
  * @retval None
  */
void LEDOn(Led_TypeDef led)
{
	LED_GPIO_PORT[led]->BSRR = LED_GPIO_PIN[led];
}

/**
  * @brief  Turns selected LED off.
  * @param  led: Specifies the LED to be set off.
  * @retval None
  */
void LEDOff(Led_TypeDef led)
{
	LED_GPIO_PORT[led]->BRR = LED_GPIO_PIN[led];
}

/**
  * @brief  Toggles the selected LED.
  * @param  led: Specifies the LED to be toggled.
  * @retval None
  */
void LEDToggle(Led_TypeDef led)
{
	LED_GPIO_PORT[led]->ODR ^= LED_GPIO_PIN[led];
}
