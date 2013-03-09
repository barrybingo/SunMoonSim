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
#include "gui_button.h"
#include "sun_moon.h"
#include "ili932x.h"
#include "touch.h"
#include "hardware.h"


/* some basic compiler checks ---------------------------------------*/
#if !defined (USE_STDPERIPH_DRIVER)  || !defined (STM32F10X_MD)
 #error "USE_STDPERIPH_DRIVER and STM32F10X_MD need to be compiler preprocessor defines"
#endif

/* local globals ----------------------------------------------------*/
typedef void(*SCREEN_PTR)();
SCREEN_PTR currentScreenPtr;


/* function prototypes ----------------------------------------------*/
void NVIC_Configuration(void);
uint8_t KEY_Scan(void);

void Render_Current_Screen();
void Change_To_Screen(SCREEN_PTR scrPtr);
void Main_Menu_SCREEN();
void Test_SCREEN();

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void) {
	/* local variables */
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

	/* GUI */
	currentScreenPtr = &Main_Menu_SCREEN;

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
		    Change_To_Screen(Main_Menu_SCREEN);
		}
		else if (keyscan==2)
		{
			Change_To_Screen(Main_Menu_SCREEN);
		}

		/* is screen being touched? */
		if(Pen_Point.Key_Sta==Key_Down)
		{
			Pen_Int_Set(0); // turn interrupt off
			do
			{
				/* get fresh readings and convert for LCD dimensions */
				Convert_Pos();
				Pen_Point.Key_Sta=Key_Up;

				/* update GUI inputs */
		        uistate.mousex = Pen_Point.X0;
		        uistate.mousey = Pen_Point.Y0;
	            uistate.mousedown = 1;

	            /* draw GUI */
	            Render_Current_Screen();
			}while(PEN==0);

			Pen_Int_Set(1); // turn interrupt back on
		}
		else
		{
			/* update GUI inputs */
			uistate.mousedown = 0;

            /* draw GUI */
            Render_Current_Screen();
		}
	}
}

/**
  * @brief  Initialise NVIC external interrupts.
  * 		Used for touch screen pen down.
  * @param  None
  * @retval None
  */
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

	/* Enable the EXTI15_10_IRQn Interrupt
	 * This will fire when the touch screen pen is down
	 * */
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

/**
  * @brief  Passes UI inputs to current screen and
  *         signals for the current screen to draw itself onto the LCD
  * @param  None
  * @retval None
  */
void Render_Current_Screen()
{
	  Imgui_Prepare();
	  currentScreenPtr();
	  Imgui_Finish();
}

/**
  * @brief  Clears LCD and switches current screen
  * @param  None
  * @retval None
  */
void Change_To_Screen(SCREEN_PTR scrPtr)
{
	LCD_Clear(WHITE);
	currentScreenPtr = scrPtr;
}

/**
  * @brief  Testing screen with various diagnostic info displayed
  * @param  None
  * @retval None
  */
void Test_SCREEN()
{
	uint16_t brightness = 0;

	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	WriteString(20,40,"Touch here for brightness",RED);
	WriteString(20,280," or here for dull days",RED);

	WriteString(50,135,"KEY1=Calibrate screen",BLUE);
	WriteString(50,155,"KEY2=Main menu",BLUE);

	/* draw on the LCD like a pencil */
	Draw_Big_Point(Pen_Point.X0,Pen_Point.Y0);

	/* shine the sun and moon */
	brightness = (Pen_Point.Y0 / (float)LCD_H) * (float)MAX_SUN_MOON_BRIGHTNESS;
	LCD_ShowNum(100,10,brightness,5,16);

	Set_Sun_Brightness(brightness);
	Set_Moon_Brightness(MAX_SUN_MOON_BRIGHTNESS - brightness);

	if (ButtonWidget(GEN_ID,20,210,100,50,"RETURN"))
		Change_To_Screen(Main_Menu_SCREEN);
}

/**
  * @brief  Main menu screen
  * @param  None
  * @retval None
  */
void Main_Menu_SCREEN()
{
  if (ButtonWidget(GEN_ID,20,50,100,50,"TEST"))
  {
	  Change_To_Screen(Test_SCREEN);
	  return;
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
