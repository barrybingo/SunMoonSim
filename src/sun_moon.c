#include "stm32f10x_conf.h"
#include "sun_moon.h"
#include "pwm.h"

PWM_Output Sun_PWM;
PWM_Output Moon_PWM;

void Sun_Moon_Init(void)
{
	// Sun @ PB0/ADC12_IN8/TIM3_CH3/TIM1_CH2N
	Sun_PWM.GPIO_TIM = TIM3;
	Sun_PWM.Channel = 3;
	Sun_PWM.RCC_APB1Periph = RCC_APB1Periph_TIM3;
	Sun_PWM.RCC_APB2Periph = RCC_APB2Periph_GPIOB;
	Sun_PWM.GPIO_Pin = GPIO_Pin_0;
	Sun_PWM.GPIO_PORT = GPIOB;
	Sun_PWM.TIM_Period = 12000;

	PWM_Init_Output(&Sun_PWM);
	PWM_Set_Output(&Sun_PWM, 0);

	// Moon @ PB1/ADC12_IN9/TIM3_CH4/TIM1_CH3N
	Moon_PWM.GPIO_TIM = TIM3;
	Moon_PWM.Channel = 4;
	Moon_PWM.RCC_APB1Periph = RCC_APB1Periph_TIM3;
	Moon_PWM.RCC_APB2Periph = RCC_APB2Periph_GPIOB;
	Moon_PWM.GPIO_Pin = GPIO_Pin_1;
	Moon_PWM.GPIO_PORT = GPIOB;
	Moon_PWM.TIM_Period = 12000;

	PWM_Init_Output(&Moon_PWM);
	PWM_Set_Output(&Moon_PWM, 0);
}

void Set_Sun_Brightness(uint16_t b)
{
	if (b>MAX_SUN_MOON_BRIGHTNESS)
		b = MAX_SUN_MOON_BRIGHTNESS;

	PWM_Set_Output(&Sun_PWM, b);
}
void Set_Moon_Brightness(uint16_t b)
{
	if (b>MAX_SUN_MOON_BRIGHTNESS)
		b = MAX_SUN_MOON_BRIGHTNESS;

	PWM_Set_Output(&Moon_PWM, b);
}
