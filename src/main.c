/**
 ******************************************************************************
 * @file    main.c
 * @author  Barry Bingo
 * @brief   Main program body
 ******************************************************************************
 */
#include <stdio.h>
#include <math.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_it.h"
#include "led.h"
#include "key.h"
#include "pwm.h"
#include "gui_button.h"
#include "ili932x.h"
#include "touch.h"
#include "hardware.h"
#include "Bitmaps/background120x160.c"

#define VERSION_TEXT "Orange-Test 0.3"

/* some basic compiler checks ---------------------------------------*/
#if !defined (USE_STDPERIPH_DRIVER)  || !defined (STM32F10X_MD)
 #error "USE_STDPERIPH_DRIVER and STM32F10X_MD need to be compiler preprocessor defines"
#endif

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

/* RTC -------------------------------------------------------------------------------*/
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */
__IO uint32_t TimeDisplay = 0;
uint32_t GlobalTime = 0;  // Seconds passed in this day since midnight
#define  RefreshGlobalTime() GlobalTime = RTC_GetCounter() % 0x00015180;

/* GUI typedefs, local globals and defines-------------------------------------------------*/
typedef void(*SCREEN_PTR)(uint8_t);
SCREEN_PTR currentScreenPtr;

#define gui_h_margin 20
#define gui_v_margin 45
#define gui_button_height 50
#define gui_button_width  LCD_W-(2*gui_h_margin)

#define GUI_V_GAP  (LCD_H - (nButtons*gui_button_height) - (gui_v_margin*2)) / (nButtons-1)
#define GUI_V_POS(n) gui_v_margin+(n*(gui_button_height+GUI_V_GAP))

#define ShowButton(x,str) ButtonWidget(GEN_ID, gui_h_margin, GUI_V_POS(x), gui_button_width, gui_button_height, str, fullrender)

/* Sun Moon */
#define MAX_DIMNESS  160  // most LEDs knock out at 100
#define FIVEMINS_PER_DAY 288

typedef struct
{
	/* Light intensity function parameters */
	uint16_t freq;
	uint16_t amp;
	int16_t shift;
	float phase;

	/* Output */
	PWM_Output PWM;

} MOVING_LIGHT_SOURCE;

MOVING_LIGHT_SOURCE Sun, Moon;


/* function prototypes ----------------------------------------------*/
void NVIC_Configuration_TouchScreenPen(void);
uint8_t KEY_Scan(void);

/* GUI control */
void Render_Current_Screen();
void Change_To_Screen(SCREEN_PTR scrPtr);

/* GUI screens */
void Main_Menu_SCREEN(uint8_t fullrender);
void Wavy_SCREEN(uint8_t fullrender);
void Manual_SCREEN(uint8_t fullrender);
void Settings_SCREEN(uint8_t fullrender);
void Config_Sun_SCREEN(uint8_t fullrender);
void Config_Moon_SCREEN(uint8_t fullrender);
void Change_Time_SCREEN(uint8_t fullrender);

void Config_MLS_SCREEN(uint8_t fullrender, uint8_t bSun);

/* RTC control */
void NVIC_Configuration_RTC(void);
void RTC_Configuration(void);
void Time_Adjust(uint32_t newTime);
void Time_Display();

/* Sun and Moon dimmers */
void Sun_Moon_Dimmers_Init(void);
uint16_t Get_DimmerValueBasedOnTime(MOVING_LIGHT_SOURCE* mls, uint32_t time);
void SetAllDimmersBasedOnTime(uint32_t time);

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
		Time_Adjust(0);

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
	BKP_RTCOutputConfig(BKP_RTCOutputSource_None);
#else
	BKP_RTCOutputConfig(BKP_RTCOutputSource_None);
#endif

	/* Clear reset flags */
	RCC_ClearFlag();

	/* NVIC to enable interrupt on screen touch */
	NVIC_Configuration_TouchScreenPen();

	/* touch screen */
	Touch_Init();

	/* init LCD */
	LCD_Init();
	LCD_Clear(BLACK);

	/* GUI */
	Change_To_Screen(Main_Menu_SCREEN);

	/* on board key buttons */
	KeyInit(KEY1);
	KeyInit(KEY2);

	/* on board LEDs */
	LEDInit(LED1);
	LEDInit(LED2);

	/* the stellar bodies */
	Sun_Moon_Dimmers_Init();

	/* Infinite loop */
	while (1)
	{
	    /* If 1s has been elapsed */
	    if (TimeDisplay == 1)
	    {
	    	RefreshGlobalTime();

			/* Display current time */
			Time_Display();
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
			LEDOn(LED2);
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
			LEDOff(LED2);
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
	currentScreenPtr(0);
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
	Time_Display();
	Imgui_Prepare();
	scrPtr(1);
	Imgui_Finish();

	currentScreenPtr = scrPtr;
}

/**
  * @brief  Undulating wavy lights
  * @param  None
  * @retval None
  */
void Wavy_SCREEN(uint8_t fullrender)
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
void Manual_SCREEN(uint8_t fullrender)
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
void Settings_SCREEN(uint8_t fullrender)
{
	const uint16_t nButtons = 4;

	if (ShowButton(0,"Change Time"))
	{
		Change_To_Screen(Change_Time_SCREEN);
		return;
	}

	if (ShowButton(1,"Config Sun"))
	{
		Change_To_Screen(Config_Sun_SCREEN);
		return;
	}

	if (ShowButton(2,"Config Moon"))
	{
		Change_To_Screen(Config_Moon_SCREEN);
		return;
	}

	if (ShowButton(3,"Main Menu"))
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
void Change_Time_SCREEN(uint8_t fullrender)
{
	/* time */
	static uint32_t timeDisplayed = 0;
	uint32_t THH = 0, TMM = 0, TSS = 0;

	/* buttons */
	const uint8_t BUTTON_TOP_Y = 5;
	const uint8_t BUTTON_BOTTOM_Y = 154;
	const uint8_t BUTTON_SIZE = 64;

	/* Compute  hours */
	THH = GlobalTime / 3600;
	/* Compute minutes */
	TMM = (GlobalTime % 3600) / 60;
	/* Compute seconds */
	TSS = (GlobalTime % 3600) % 60;

	POINT_COLOR = GREEN;
	BACK_COLOR = BLACK;

	if (timeDisplayed != GlobalTime)
	{
		LCD_ShowNumBig(10,80,THH,2,4);
		LCD_ShowNumBig(85,80,TMM,2,4);
		if (TMM<10)
			LCD_ShowCharBig(85,80,'0',4,1);
		LCD_ShowNumBig(160,80,TSS,2,4);
		if (TSS<10)
			LCD_ShowCharBig(160,80,'0',4,1);
		timeDisplayed = GlobalTime;

		SetAllDimmersBasedOnTime(GlobalTime);
	}


	if (ButtonWidget(GEN_ID,20,230,200,50,"OK",fullrender))
	{
		Change_To_Screen(Settings_SCREEN);
		return;
	}

	// + hour
	if (ButtonWidget(GEN_ID,10,BUTTON_TOP_Y,BUTTON_SIZE,BUTTON_SIZE,"+H",fullrender))
		Time_Adjust(GlobalTime+3599);

	// - hour
	if (ButtonWidget(GEN_ID,10,BUTTON_BOTTOM_Y,BUTTON_SIZE,BUTTON_SIZE,"-H",fullrender))
		Time_Adjust(GlobalTime-3601);

	// + min
	if (ButtonWidget(GEN_ID,85,BUTTON_TOP_Y,BUTTON_SIZE,BUTTON_SIZE,"+M",fullrender))
		Time_Adjust(GlobalTime+59);

	// - min
	if (ButtonWidget(GEN_ID,85,BUTTON_BOTTOM_Y,BUTTON_SIZE,BUTTON_SIZE,"-M",fullrender))
		Time_Adjust(GlobalTime-61);

	// + sec
	if (ButtonWidget(GEN_ID,160,BUTTON_TOP_Y,BUTTON_SIZE,BUTTON_SIZE,"+S",fullrender))
		Time_Adjust(GlobalTime+1);

	// - sec
	if (ButtonWidget(GEN_ID,160,BUTTON_BOTTOM_Y,BUTTON_SIZE,BUTTON_SIZE,"-S",fullrender))
		Time_Adjust(GlobalTime-2);
}


/**
  * @brief  Main menu screen
  * @param  None
  * @retval None
  */
void Main_Menu_SCREEN(uint8_t fullrender)
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

	/* update Sun and Moon on screen entry and every 10 seconds there after */
	if (fullrender || (GlobalTime%10 == 0))
		SetAllDimmersBasedOnTime(GlobalTime);
}

void Config_Sun_SCREEN(uint8_t fullrender)
{
	Config_MLS_SCREEN(fullrender, 1);
}

void Config_Moon_SCREEN(uint8_t fullrender)
{
	Config_MLS_SCREEN(fullrender, 0);
}

/**
  * @brief  Modify dimmer function parameters for the Sun or Moon
  * @param  fullrender: non zero to fully redraw of screen
  *         bSun: non zero if sun, moon otherwise
  * @retval None
  */
void Config_MLS_SCREEN(uint8_t fullrender, uint8_t bSun)
{
	static uint32_t scrollingX = 0;

	const uint8_t BUTTON_TOP_Y = 200;
	const uint8_t BUTTON_BOTTOM_Y = 230;
	const uint8_t BUTTON_SIZE = 24;

	const uint16_t indent=5;
	const uint16_t m=5;
	const uint16_t h=180;
	const uint16_t w=LCD_W-m-m;
	const float xScale = (float)(w-indent)/(float)FIVEMINS_PER_DAY;

	uint16_t x,y;

	MOVING_LIGHT_SOURCE* lightSource = bSun ? &Sun : &Moon;
	MOVING_LIGHT_SOURCE* otherLightSource = bSun ? &Moon : &Sun;

	// + freq
	if (ButtonWidget(GEN_ID,5,BUTTON_TOP_Y,40,BUTTON_SIZE,"+Hz",fullrender))
	{
		fullrender=1;
		lightSource->freq--;
	}

	// - freq
	if (ButtonWidget(GEN_ID,5,BUTTON_BOTTOM_Y,40,BUTTON_SIZE,"-Hz",fullrender))
	{
		fullrender=1;
		lightSource->freq++;
	}

	// + Amplitude
	if (ButtonWidget(GEN_ID,50,BUTTON_TOP_Y,45,BUTTON_SIZE,"+Amp",fullrender))
	{
		fullrender=1;
		lightSource->amp+=2;
	}

	// - Amplitude
	if (ButtonWidget(GEN_ID,50,BUTTON_BOTTOM_Y,45,BUTTON_SIZE,"-Amp",fullrender) && lightSource->amp>0)
	{
		fullrender=1;
		lightSource->amp-=2;
	}

	// + Phase
	if (ButtonWidget(GEN_ID,100,BUTTON_TOP_Y,60,BUTTON_SIZE,"+Phase",fullrender))
	{
		fullrender=1;
		lightSource->phase-=0.1;
	}

	// - Phase
	if (ButtonWidget(GEN_ID,100,BUTTON_BOTTOM_Y,60,BUTTON_SIZE,"-Phase",fullrender))
	{
		fullrender=1;
		lightSource->phase+=0.1;
	}

	// + ShiftY
	if (ButtonWidget(GEN_ID,165,BUTTON_TOP_Y,60,BUTTON_SIZE,"+Shift",fullrender))
	{
		fullrender=1;
		lightSource->shift+=2;
	}

	// - ShiftY
	if (ButtonWidget(GEN_ID,165,BUTTON_BOTTOM_Y,60,BUTTON_SIZE,"-Shift",fullrender))
	{
		fullrender=1;
		lightSource->shift-=2;
	}

	// return
	if (ButtonWidget(GEN_ID,30,270,180,24,"OK",fullrender))
	{
		Change_To_Screen(Settings_SCREEN);
		return;
	}

	/* graph */
	if (fullrender)
	{
		DrawRect(m, m, w, h, BLACK);

		DrawLine(m+indent, m, m+indent, h+m, WHITE); // y axis
		DrawLine(m, h-m, w+m, h-m, WHITE); // x axis

		uint16_t current5MinPeriod = GlobalTime / (5*60);
		DrawLine(m+indent+(current5MinPeriod*xScale), m, m+indent+(current5MinPeriod*xScale), h+m, RED); // current time

		/* plot graph */
		for (x=0; x<FIVEMINS_PER_DAY; x++)
		{
			/* bright graph for light source under consideration */
			y = Get_DimmerValueBasedOnTime(lightSource, (uint32_t)x*300U);
			DrawPoint(m+indent+(x*xScale),h+m-indent-indent-y,GREEN);

			/* draw darker graph for the other light source */
			y = Get_DimmerValueBasedOnTime(otherLightSource, (uint32_t)x*300U);
			DrawPoint(m+indent+(x*xScale),h+m-indent-indent-y,0x2222);

		}

		/* draw some stats */
		POINT_COLOR = GREEN;
		LCD_ShowNum(10,10, lightSource->freq,3,16,1);
		LCD_ShowNum(10,26, lightSource->amp,3,16,1);
		LCD_ShowNum(10,42, (uint32_t)(lightSource->phase*10),3,16,1);
		LCD_ShowNum(10,58, lightSource->shift,3,16,1);

		POINT_COLOR = 0x2222;
		LCD_ShowNum(198,10, otherLightSource->freq,3,16,1);
		LCD_ShowNum(198,26, otherLightSource->amp,3,16,1);
		LCD_ShowNum(198,42, (uint32_t)(otherLightSource->phase*10),3,16,1);
		LCD_ShowNum(198,58, otherLightSource->shift,3,16,1);
	}

	/* live scrolling update */
	if (fullrender)
	{
		scrollingX = 0;
		DrawRect(m, h, w, 5, BLACK);
	}
	DrawRect(m+indent+(scrollingX*xScale),h, 5, 5, BLACK);  // Delete last green rect
	scrollingX = (scrollingX+1)%(FIVEMINS_PER_DAY-5);
	DrawRect(m+indent+(scrollingX*xScale),h, 5, 5, GREEN);  // draw new green rect
	SetAllDimmersBasedOnTime(scrollingX * 300U);
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
  * @brief  Adjusts time.
  * @param  None
  * @retval None
  */
void Time_Adjust(uint32_t newTime)
{
	/* is the time negative */
	if (newTime & 80000000)
		newTime = 0x00015180 + newTime - 0xFFFFFFFF;

    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Change the current time */
    RTC_SetCounter(newTime);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);

    RefreshGlobalTime();
}

/**
  * @brief  Displays the current time.
  * @param  TimeVar: RTC counter value.
  * @retval None
  */
void Time_Display() {
	unsigned char buff[10];
	uint32_t THH = 0, TMM = 0, TSS = 0;

	/* Compute  hours */
	THH = GlobalTime / 3600;
	/* Compute minutes */
	TMM = (GlobalTime % 3600) / 60;
	/* Compute seconds */
	TSS = (GlobalTime % 3600) % 60;


	DrawRect(0, LCD_H-20, LCD_W, 20, BLACK);
	sprintf(buff, "%0.2d:%0.2d:%0.2d", THH, TMM, TSS);
	BACK_COLOR = BLACK;
	WriteString(5, LCD_H-15, buff, WHITE);
	WriteString(115, LCD_H-15, VERSION_TEXT, WHITE);
}

void Sun_Moon_Dimmers_Init(void)
{
	// Sun @ PB0/ADC12_IN8/TIM3_CH3/TIM1_CH2N
	Sun.PWM.GPIO_TIM = TIM3;
	Sun.PWM.Channel = 3;
	Sun.PWM.RCC_APB1Periph = RCC_APB1Periph_TIM3;
	Sun.PWM.RCC_APB2Periph = RCC_APB2Periph_GPIOB;
	Sun.PWM.GPIO_Pin = GPIO_Pin_0;
	Sun.PWM.GPIO_PORT = GPIOB;
	Sun.PWM.TIM_Period = 12000;

	PWM_Init_Output(&Sun.PWM);
	PWM_Set_Output(&Sun.PWM, 0);

	Sun.freq = 46;
	Sun.amp = 78;
	Sun.phase = 10.7;
	Sun.shift = 29;

	// Moon @ PB1/ADC12_IN9/TIM3_CH4/TIM1_CH3N
	Moon.PWM.GPIO_TIM = TIM3;
	Moon.PWM.Channel = 4;
	Moon.PWM.RCC_APB1Periph = RCC_APB1Periph_TIM3;
	Moon.PWM.RCC_APB2Periph = RCC_APB2Periph_GPIOB;
	Moon.PWM.GPIO_Pin = GPIO_Pin_1;
	Moon.PWM.GPIO_PORT = GPIOB;
	Moon.PWM.TIM_Period = 12000;

	PWM_Init_Output(&Moon.PWM);
	PWM_Set_Output(&Moon.PWM, 0);

	Moon.freq = 52;
	Moon.amp = 54;
	Moon.phase = 1.9;
	Moon.shift = 46;
}

uint16_t Get_DimmerValueBasedOnTime(MOVING_LIGHT_SOURCE* mls, uint32_t time)
{
	return  min(MAX_DIMNESS, (uint16_t)( (sinf(mls->phase + ((float)time/FIVEMINS_PER_DAY/mls->freq))*mls->amp) +mls->amp + mls->shift));
}

void SetAllDimmersBasedOnTime(uint32_t time)
{
	PWM_Set_Output(&Sun.PWM, MAX_DIMNESS -(Get_DimmerValueBasedOnTime(&Sun,time)));
	PWM_Set_Output(&Moon.PWM, MAX_DIMNESS -(Get_DimmerValueBasedOnTime(&Moon,time)));
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
