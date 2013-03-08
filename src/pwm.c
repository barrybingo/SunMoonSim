#include "stm32f10x_conf.h"


static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
static TIM_OCInitTypeDef  TIM_OCInitStructure;
#define ARR  12000

void pwm_init_PB9(void)
{
    /* for old version */
    GPIO_InitTypeDef GPIO_InitStructure;

    /* new version: use pwm in PB9 TIM4_CH4 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    /* TIM4 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /* GPIOB Configuration:TIM4 Channel4 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_9;//GPIO_Pin_8 |
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = ARR;//12000
    TIM_TimeBaseStructure.TIM_Prescaler = 2;//Ô¤·ÖÆµ2,ÆµÂÊ36M
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    /* PWM1 Mode configuration: Channel4 */
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = (ARR/100)*50; /* ÉèÖÃ³õÊ¼±³¹âÁÁ¶È */
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);

    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    /* TIM4 enable counter */
    TIM_Cmd(TIM4, ENABLE);
}

void pwm_set(unsigned int value)
{
    if(value>100)value=50;
    TIM_OCInitStructure.TIM_Pulse = (ARR/100)*value;
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);
}


void pwm_init(void)
{
	pwm_init_PB9();
	pwm_set(25);
}
