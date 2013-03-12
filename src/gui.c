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
void DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, COLOR color)
{
	LCD_Fill(x, y, x + w, y + h, color);
}

/**
  * @brief  Outline rectangle - no fill
  * @param  x,y: x and y drawing positions
  * 		w,h: width and height
  * 		colour: Pen colour
  * @retval None
  */
void DrawOutlineRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, COLOR color)
{
	DrawLine(x,y,x+w,y,color);
	DrawLine(x,y+h,x+w,y+h,color);
	DrawLine(x,y,x,y+h,color);
	DrawLine(x+w,y,x+w,y+h,color);
}

/**
  * @brief  Draw text to LCD
  * @param  x,y: x and y drawing positions
  * 		pcStr: text to write
  * 		textColour: text pen colour
  * 		backColor: backgound pen colour - ignored if transparent is non zero
  * 		transparent: nonzero for transparent background, zero otherwise
  * @retval None
  */
void DrawString(uint16_t x, uint16_t y, uint8_t *pStr, COLOR textColor, COLOR backColor, uint8_t transparent)
{
	uint16_t bc = BACK_COLOR;
	BACK_COLOR = backColor;
	WriteString(x, y, pStr, textColor, transparent);
	BACK_COLOR = bc;
}


void DrawPoint(uint16_t x, uint16_t y, COLOR color)
{
	POINT_COLOR = color;
	LCD_DrawPoint(x, y);
}

void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, COLOR color)
{
	POINT_COLOR = color;
	LCD_DrawLine(x1, y1, x2, y2);
}


/**
  * @brief  Check whether current mouse position is within a rectangle
  * @param  x,y: x and y of region top left corner
  * 		w,h: width and height of bounding region
  * @retval 1: HIT
  * 	    0: MISS
  */
uint8_t RegionHit(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
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


/**
  * @brief  Simple widget return true is screen has been 'clicked'
  * @param  None
  * @retval None
  */
uint8_t ScreenClicked(uint8_t id)
{
	// Check whether the button should be hot
	if (uistate.activeitem == 0 && uistate.mousedown)
		uistate.activeitem = id;

	// If screen is hot and active, but mouse button is not
	// down, the user must have clicked the screen.
	if (uistate.mousedown == 0 && uistate.activeitem == id)
		return 1;

	return 0;
}

