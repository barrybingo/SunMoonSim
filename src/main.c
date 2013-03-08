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
#include "pwm.h"
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	   // �����ж��� Ϊ2

	/* Enable the EXTI15_10_IRQn Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	// �����ж���4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // ����ռ�����ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // ���ø����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // ʹ���ж���4
	NVIC_Init(&NVIC_InitStructure);							  // ��ʼ���ж���4
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

	/* NVIC to enable interrupt on screen touch */
	NVIC_Configuration();

	/* touch screen */
	Touch_Init();


	WriteString(10,40,"Touch here for brightness",BLUE);
	WriteString(10,280,"or here for dull days",BLUE);

	/* init LEDs */
	LEDInit(LED1);
	LEDInit(LED2);
	LEDToggle(LED1);

	pwm_init();

	/* Infinite loop */
	while (1)
	{
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

				/* some diagnostic info */
				LCD_ShowNum(10,10,Pen_Point.X,5,16);
				LCD_ShowNum(100,10,Pen_Point.Y,5,16);
				LCD_ShowNum(10,100,Pen_Point.X0,5,16);
				LCD_ShowNum(100,100,Pen_Point.Y0,5,16);

				/* set dimmer */
				pwm_set((Pen_Point.Y0 / (float)LCD_H) * (float)MAX_LED_BRIGHTNESS);
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
