/**
 ******************************************************************************
 * @file    gui.c
 * @author  Barry Bingo
 * @brief   Simple GUI
 * Immediate Mode GUI implementation based on tutorials and code
 * found on http://sol.gfxile.net/imgui/
 ******************************************************************************/

#include "stdint.h"
#include "ili932x.h"
#include "gui.h"

struct UIState uistate = { 0, 0, 0, 0, 0 };

/**
  * @brief  Filled rectangle
  * @param  x,y: x and y drawing positions
  * 		w,h: width and height
  * 		colour: Fill color
  * @retval None
  */
void DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, COLOR color)
{
	LCD_Fill(x, y, x + w, y + h, color);
}

/**
  * @brief  Draw text to LCD
  * @param  x,y: x and y drawing positions
  * 		pcStr: text to write
  * 		textColour: text pen colour
  * 		backColor: backgound pen colour
  * @retval None
  */
void DrawString(uint16_t x, uint16_t y, uint8_t *pStr, COLOR textColor, COLOR backColor)
{
	uint16_t bc = BACK_COLOR;
	BACK_COLOR = backColor;
	WriteString(x, y, pStr, textColor);
	BACK_COLOR = bc;
}



/**
  * @brief  Check whether current mouse position is within a rectangle
  * @param  x,y: x and y of region top left corner
  * 		w,h: width and height of bounding region
  * @retval 1: HIT
  * 	    0: MISS
  */
uint8_t RegionHit(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	if (uistate.mousex < x || uistate.mousey < y || uistate.mousex >= x + w
			|| uistate.mousey >= y + h)
		return 0;
	return 1;
}

/**
  * @brief  Prepare for IMGUI code
  * @param  None
  * @retval None
  */
void Imgui_Prepare()
{
	uistate.hotitem = 0;
}

/**
  * @brief  Finish up after IMGUI code
  * @param  None
  * @retval None
  */
void Imgui_Finish() {
	if (uistate.mousedown == 0) {
		uistate.activeitem = 0;
	} else {
		if (uistate.activeitem == 0)
			uistate.activeitem = -1;
	}
}

