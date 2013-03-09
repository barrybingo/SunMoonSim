#include "stm32f10x_conf.h"
#include "key.h"

GPIO_TypeDef* KEY_GPIO_PORT[NUM_KEYS] = {KEY1_GPIO_PORT, KEY2_GPIO_PORT};
const uint16_t KEY_GPIO_PIN[NUM_KEYS] = {KEY1_PIN, KEY2_PIN};
const uint32_t KEY_GPIO_CLK[NUM_KEYS] = {KEY1_GPIO_CLK, KEY2_GPIO_CLK};

/**
  * @brief  Configures key push button GPIO.
  * @param  key: Specifies the key button to be configured.
  *   This parameter can be one of following parameters:
  *     @arg KEY1
  *     @arg KEY2
  * @retval None
  */
void KeyInit(Key_TypeDef key)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable the GPIO_LED Clock */
	RCC_APB2PeriphClockCmd(KEY_GPIO_CLK[key], ENABLE);

	GPIO_InitStructure.GPIO_Pin = KEY_GPIO_PIN[key];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(KEY_GPIO_PORT[key], &GPIO_InitStructure);
}

/**
  * @brief  Read key push button state
  * @param  key: Specifies the bush button key
  *   This parameter can be one of following parameters:
  *     @arg KEY1
  *     @arg KEY2
  * @retval KEY_PRESSED or KEY_RELEASED
  */
KeyState_TypeDef KeyState(Key_TypeDef key)
{
	return (GPIO_ReadInputDataBit(KEY_GPIO_PORT[key], KEY_GPIO_PIN[key]) ? KEY_RELEASED : KEY_PRESSED);
}
