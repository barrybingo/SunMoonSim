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
#include "Bitmaps/background120x160.c"


/* some basic compiler checks ---------------------------------------*/
#if !defined (USE_STDPERIPH_DRIVER)  || !defined (STM32F10X_MD)
 #error "USE_STDPERIPH_DRIVER and STM32F10X_MD need to be compiler preprocessor defines"
#endif

/* RTC -------------------------------------------------------------------------------*/
#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */
__IO uint32_t TimeDisplay = 0;

/* GUI typedefs, local globals and defines-------------------------------------------------*/
typedef void(*SCREEN_PTR)();
SCREEN_PTR currentScreenPtr;

#define gui_h_margin 20
#define gui_v_margin 45
#define gui_button_height 50
#define gui_button_width  LCD_W-(2*gui_h_margin)

#define GUI_V_GAP  (LCD_H - (nButtons*gui_button_height) - (gui_v_margin*2)) / (nButtons-1)
#define GUI_V_POS(n) gui_v_margin+(n*(gui_button_height+GUI_V_GAP))

#define ShowButton(x,str) ButtonWidget(GEN_ID, gui_h_margin, GUI_V_POS(x), gui_button_width, gui_button_height, str)


/* function prototypes ----------------------------------------------*/
void NVIC_Configuration_TouchScreenPen(void);
uint8_t KEY_Scan(void);

/* GUI control */
void Render_Current_Screen();
void Change_To_Screen(SCREEN_PTR scrPtr);

/* GUI screens */
void Main_Menu_SCREEN();
void Wavy_SCREEN();
void Manual_SCREEN();
void Settings_SCREEN();
void Change_Time_SCREEN();
void Test_SCREEN();

/* RTC control */
void NVIC_Configuration_RTC(void);
void RTC_Configuration(void);
uint32_t Time_Regulate(void);
void Time_Adjust(void);
void Time_Show(void);
void Time_Display(uint32_t TimeVar);

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

	/* RTC */
	NVIC_Configuration_RTC();

	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		/* Backup data register value is not correct or not yet programmed (when
		 the first time the program is executed) */

		printf("\r\n\n RTC not yet configured....");

		/* RTC Configuration */
		RTC_Configuration();

		printf("\r\n RTC configured....");

		/* Adjust time by values entered by the user on the hyperterminal */
		Time_Adjust();

		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	} else {
		/* Check if the Power On Reset flag is set */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST ) != RESET)
		{
			printf("\r\n\n Power On Reset occurred....");
		}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST ) != RESET)
		{
			printf("\r\n\n External Reset occurred....");
		}

		printf("\r\n No need to configure RTC....");
		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();

		/* Enable the RTC Second */
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}

#ifdef RTCClockOutput_Enable
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Disable the Tamper Pin */
	BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
	 functionality must be disabled */

	/* Enable RTC Clock Output on Tamper Pin */
	BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif

	/* Clear reset flags */
	RCC_ClearFlag();

	/* init LCD */
	LCD_Init();
	LCD_Clear(BLACK);

	/* NVIC to enable interrupt on screen touch */
	NVIC_Configuration_TouchScreenPen();

	/* touch screen */
	Touch_Init();

	/* GUI */
	Change_To_Screen(Main_Menu_SCREEN);

	/* on board key buttons */
	KeyInit(KEY1);
	KeyInit(KEY2);

	/* on board LEDs */
	LEDInit(LED1);
	LEDInit(LED2);

	/* the stellar bodies */
	Sun_Moon_Init();

	/* Infinite loop */
	while (1)
	{
	    /* If 1s has been elapsed */
	    if (TimeDisplay == 1)
	    {
	      /* Display current time */
	      Time_Display(RTC_GetCounter());
	      TimeDisplay = 0;
	    }

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
void NVIC_Configuration_TouchScreenPen(void)
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
	LCD_WriteBMPx2(0, 0, 160,120, background120x160_bmp);
	currentScreenPtr = scrPtr;
}

/**
  * @brief  Undulating wavy lights
  * @param  None
  * @retval None
  */
void Wavy_SCREEN()
{
	const uint16_t nButtons = 1;

	if (ShowButton(0,"Wavy ret"))
	{
		Change_To_Screen(Main_Menu_SCREEN);
		return;
	}
}

/**
  * @brief  Set moon and sun using sliders
  * @param  None
  * @retval None
  */
void Manual_SCREEN()
{
	const uint16_t nButtons = 1;

	if (ShowButton(0,"Man ret"))
	{
		Change_To_Screen(Main_Menu_SCREEN);
		return;
	}
}

/**
  * @brief  Change some basic settings
  * @param  None
  * @retval None
  */
void Settings_SCREEN()
{
	const uint16_t nButtons = 3;

	if (ShowButton(0,"Change Time"))
	{
		Change_To_Screen(Change_Time_SCREEN);
		return;
	}

	if (ShowButton(1,"Testing"))
	{
		Change_To_Screen(Test_SCREEN);
		return;
	}


	if (ShowButton(2,"Main Menu"))
	{
		Change_To_Screen(Main_Menu_SCREEN);
		return;
	}

}

/**
  * @brief  Adjust RTC
  * @param  None
  * @retval None
  */
void Change_Time_SCREEN()
{
	const uint16_t nButtons = 1;

	if (ShowButton(0,"CTime ret"))
	{
		Change_To_Screen(Main_Menu_SCREEN);
		return;
	}
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

	if (ButtonWidget(GEN_ID,20,210,100,50,"Main Menu"))
		Change_To_Screen(Main_Menu_SCREEN);
}

/**
  * @brief  Main menu screen
  * @param  None
  * @retval None
  */
void Main_Menu_SCREEN()
{
	const uint16_t nButtons = 3;

	if (ShowButton(0,"Wavy Mode"))
	{
		Change_To_Screen(Wavy_SCREEN);
		return;
	}

	if (ShowButton(1,"Manual Mode"))
	{
		Change_To_Screen(Manual_SCREEN);
		return;
	}

	if (ShowButton(2,"Settings"))
	{
		Change_To_Screen(Settings_SCREEN);
		return;
	}
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration_RTC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval Current time RTC counter value
  */
uint32_t Time_Regulate(void)
{
  uint32_t Tmp_HH = 15, Tmp_MM = 4, Tmp_SS = 12;

  /* Return the value to store in RTC counter register */
  return((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
}

/**
  * @brief  Adjusts time.
  * @param  None
  * @retval None
  */
void Time_Adjust(void)
{
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time */
  RTC_SetCounter(Time_Regulate());
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

/**
  * @brief  Displays the current time.
  * @param  TimeVar: RTC counter value.
  * @retval None
  */
void Time_Display(uint32_t TimeVar)
{
  uint32_t THH = 0, TMM = 0, TSS = 0;

  /* Reset RTC Counter when Time is 23:59:59 */
  if (RTC_GetCounter() == 0x0001517F)
  {
     RTC_SetCounter(0x0);
     /* Wait until last write operation on RTC registers has finished */
     RTC_WaitForLastTask();
  }

  /* Compute  hours */
  THH = TimeVar / 3600;
  /* Compute minutes */
  TMM = (TimeVar % 3600) / 60;
  /* Compute seconds */
  TSS = (TimeVar % 3600) % 60;

  LCD_ShowNum(5,5,THH,2,16);
  LCD_ShowNum(23,5,TMM,2,16);
  LCD_ShowNum(43,5,TSS,2,16);
  //printf("Time: %0.2d:%0.2d:%0.2d\r", THH, TMM, TSS);
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
