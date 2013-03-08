/**
 ******************************************************************************
 * @file    main.c
 * @author  Barry Bingo
 * @brief   Main program body
 ******************************************************************************
 */

#include "stm32f10x_conf.h"
#include "stm32f10x_it.h"
#include "led.h"
#include "pwm.h"
#include "ili932x.h"
#include "RGB.h"


/* some basic compiler checks */
#if !defined (USE_STDPERIPH_DRIVER)  || !defined (STM32F10X_MD)
 #error "USE_STDPERIPH_DRIVER and STM32F10X_MD need to be compiler preprocessor defines"
#endif


/**
 * @brief  Delays execution for specified time
 * @param  ms: milliseconds
 * @retval None
 */
void delay_ms(uint32_t ms) {
	uint32_t now = GetMsTicks();
	while ((GetMsTicks() - now) < ms)
		;
}

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void) {
	unsigned int brightness = 0;

	/* initialisation code */
	SystemInit();	   		//72MHzclock, PLL and Flash configuration
	while(SysTick_Config(SystemCoreClock / 1000));

	/* init LCD */
	LCD_Init();
	LCD_Clear(WHITE);
	WriteString(10,40,"Touch here for brightness",BLUE);
	WriteString(10,280,"or here for dull days",BLUE);

	/* init LEDs */
	LEDInit(LED1);
	pwm_init();

	/* Infinite loop */
	while (1)
	{
		delay_ms(50);
		LEDToggle(LED1);

		if (brightness > MAX_LED_BRIGHTNESS)
			brightness = 0;
		pwm_set(brightness++);
	}
}

#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif
