#ifndef __LCD_HW_H__
#define __LCD_HW_H__

typedef struct
{
	/* user defined */
	TIM_TypeDef*  GPIO_TIM;
	uint8_t		  Channel;
	uint32_t      RCC_APB1Periph;
	uint32_t      RCC_APB2Periph;
	uint16_t      GPIO_Pin;
	GPIO_TypeDef* GPIO_PORT;
	uint16_t      TIM_Period;

	/* internal usage */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
} PWM_Output;

void PWM_Init_Output(PWM_Output* output);
void PWM_Set_Output(PWM_Output* output, uint16_t value);

#endif


