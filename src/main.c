/**
 ******************************************************************************
 * @file    main.c
 * @author  Barry Bingo
 * @brief   Main program body
 ******************************************************************************
 */
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_it.h"
#include "led.h"
#include "key.h"
#include "sun_moon.h"
#include "ili932x.h"
#include "touch.h"
#include "hardware.h"


/* some basic compiler checks */
#if !defined (USE_STDPERIPH_DRIVER)  || !defined (STM32F10X_MD)
 #error "USE_STDPERIPH_DRIVER and STM32F10X_MD need to be compiler preprocessor defines"
#endif

void NVIC_Configuration(void)
{
  	NVIC_InitTypeDef NVIC_InitStructure;

	#ifdef  VECT_TAB_RAM
	  /* Set the Vector Table base location at 0x20000000 */
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
	#else  /* VECT_TAB_FLASH  */
	  /* Set the Vector Table base location at 0x08000000 */
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	#endif

	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the EXTI15_10_IRQn Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Read on-board key button states
  *         Note if KEY one is pressed and held then KEY_Scan will only return 1 for the first call.
  *         All keys have to be released before a new key press will be returned by KEY_Scan.
  * @param  None
  * @retval 0: No keys are pressed
  * 		1: if KEY1 is pressed and was not pressed on last call
  * 		2: if KEY2 is pressed and was not pressed on last call
  */
uint8_t KEY_Scan(void)
{
	static uint8_t NONE_PRESSED=1; // 1 if all keys are released; 0 otherwise

	if(NONE_PRESSED && (KeyState(KEY1)==KEY_PRESSED || KeyState(KEY2)==KEY_PRESSED))
	{
		Delay(10); // small debounce delay for them rattling switches and glitches
		if(KeyState(KEY1)==KEY_PRESSED==0)
		{
			NONE_PRESSED=0;
			return 1;
		}
		else if(KeyState(KEY2)==KEY_PRESSED==0)
		{
			NONE_PRESSED=0;
			return 2;
		}

		return 0;  // a glitch
	}
	else if(KeyState(KEY1)==KEY_RELEASED && KeyState(KEY2)==KEY_RELEASED)
		NONE_PRESSED=1;
	return 0;
}

void Clear_Screen()
{
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(WHITE);
	WriteString(20,40,"Touch here for brightness",RED);
	WriteString(20,280," or here for dull days",RED);

	WriteString(50,135,"KEY1=Calibrate screen",BLUE);
	WriteString(50,155,"KEY2=Clear screen",BLUE);
}

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void) {
	/* local variables */
	uint16_t brightness = 0;
	uint8_t keyscan = 0;

	/* set clocks ticking correctly */
	SystemInit();	   		//72MHzclock, PLL and Flash configuration
	while(SysTick_Config(SystemCoreClock / 1000));

	/* init LCD */
	LCD_Init();
	LCD_Clear(WHITE);

	/* NVIC to enable interrupt on screen touch */
	NVIC_Configuration();

	/* touch screen */
	Touch_Init();

	Clear_Screen();

	/* on board key buttons */
	KeyInit(KEY1);
	KeyInit(KEY2);

	/* on board LEDs */
	LEDInit(LED1);
	LEDInit(LED2);
	LEDToggle(LED1);

	/* the stellar bodies */
	Sun_Moon_Init();

	/* Infinite loop */
	while (1)
	{
		/* check on board button states */
		keyscan = KEY_Scan();
		if (keyscan==1)
		{
		    Touch_Adjust();
		}
		else if (keyscan==2)
		{
			Clear_Screen();
		}

		/* is screen being touched? */
		if(Pen_Point.Key_Sta==Key_Down)
		{
			Pen_Int_Set(0); // turn interrupt off
			do
			{
				LEDToggle(LED1);
				LEDToggle(LED2);

				/* get fresh readings and convert for LCD dimensions */
				Convert_Pos();
				Pen_Point.Key_Sta=Key_Up;

				/* draw on the LCD like a pencil */
				Draw_Big_Point(Pen_Point.X0,Pen_Point.Y0);

				/* shine the sun and moon */
				brightness = (Pen_Point.Y0 / (float)LCD_H) * (float)MAX_SUN_MOON_BRIGHTNESS;
				LCD_ShowNum(100,10,brightness,5,16);

				Set_Sun_Brightness(brightness);
				Set_Moon_Brightness(MAX_SUN_MOON_BRIGHTNESS - brightness);
			}while(PEN==0);

			Pen_Int_Set(1); // turn interrupt back on
		}
		else Delay(1);
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
