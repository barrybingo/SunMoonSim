#include "stm32f10x_conf.h"
#include "pwm.h"

void PWM_Init_Output(PWM_Output* output)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(output->RCC_APB2Periph, ENABLE);
    RCC_APB1PeriphClockCmd(output->RCC_APB1Periph, ENABLE);

    /* GPIOB Configuration:TIM4 Channel4 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin   =  output->GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(output->GPIO_PORT, &GPIO_InitStructure);

    /* Time base configuration */
    output->TIM_TimeBaseStructure.TIM_Period = output->TIM_Period;
    output->TIM_TimeBaseStructure.TIM_Prescaler = 2;//Ô¤·ÖÆµ2,ÆµÂÊ36M
    output->TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    output->TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(output->GPIO_TIM, &output->TIM_TimeBaseStructure);

    /* PWM1 Mode configuration: Channel1 */
    output->TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    output->TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    /* PWM1 Mode configuration: ChannelX */
    output->TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    output->TIM_OCInitStructure.TIM_Pulse = (output->TIM_Period/100)*50;

    switch( output->Channel )
    {
        case 1:
            TIM_OC1Init(output->GPIO_TIM, &output->TIM_OCInitStructure);
            TIM_OC1PreloadConfig(output->GPIO_TIM, TIM_OCPreload_Enable);
        	break;
        case 2:
            TIM_OC2Init(output->GPIO_TIM, &output->TIM_OCInitStructure);
            TIM_OC2PreloadConfig(output->GPIO_TIM, TIM_OCPreload_Enable);
        	break;
        case 3:
            TIM_OC3Init(output->GPIO_TIM, &output->TIM_OCInitStructure);
            TIM_OC3PreloadConfig(output->GPIO_TIM, TIM_OCPreload_Enable);
        	break;
        case 4:
            TIM_OC4Init(output->GPIO_TIM, &output->TIM_OCInitStructure);
            TIM_OC4PreloadConfig(output->GPIO_TIM, TIM_OCPreload_Enable);
        	break;
    }

    TIM_ARRPreloadConfig(output->GPIO_TIM, ENABLE);
    /* output->GPIO_TIM enable counter */
    TIM_Cmd(output->GPIO_TIM, ENABLE);
}

void PWM_Set_Output(PWM_Output* output, uint16_t value)
{
    output->TIM_OCInitStructure.TIM_Pulse = (output->TIM_Period/100)*value;

    switch( output->Channel )
    {
        case 1:
        	TIM_OC3Init(output->GPIO_TIM, &output->TIM_OCInitStructure);
        	break;
        case 2:
        	TIM_OC3Init(output->GPIO_TIM, &output->TIM_OCInitStructure);
        	break;
        case 3:
        	TIM_OC3Init(output->GPIO_TIM, &output->TIM_OCInitStructure);
        	break;
        case 4:
        	TIM_OC4Init(output->GPIO_TIM, &output->TIM_OCInitStructure);
        	break;
    }
}




