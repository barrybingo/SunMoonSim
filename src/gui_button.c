/**
 ******************************************************************************
 * @file    gui_button.c
 * @author  Barry Bingo
 * @brief   Simple button widget
 *
 * Code based on examples from http://sol.gfxile.net/imgui/
 ******************************************************************************/

#include "RGB.h"
#include "gui_button.h"
#include "Bitmaps/button_skin100x30.c"

/**
 * @brief  Simple button IMGUI widget
 * @param  id: Unique ID
 * 		x,y: x and y drawing positions
 * 		w,h: width and height
 * 		text: Text displayed on button
 * 		bitmap: use stock bitmap to skin the button
 * 		fullrender: non zero to fully render the button else just render changes
 * @retval 1: If button has been clicked
 *         0: If button has not been clicked
 */
uint8_t ButtonWidget(uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t* text, uint8_t bitmap, uint8_t fullrender)
{
	static COLOR lastbg = 0;
	COLOR bg;

	// Check whether the button should be hot
	if (RegionHit(x, y, w, h))
	{
		uistate.hotitem = id;
		if (uistate.activeitem == 0 && uistate.mousedown)
			uistate.activeitem = id;
	}

	// Render button
	if (uistate.hotitem == id)
	{
		if (uistate.activeitem == id)
		{
			// Button is both 'hot' and 'active'
			bg = RED;
		} else {
			// Button is merely 'hot'
			bg = NAVY;
		}
	} else {
		// button is not hot, but it may be active
		bg = BLACK;
	}

	if (fullrender)
	{
		if (bitmap)
			LCD_WriteBMPx2(x, y, 30, 100, button_skin100x30_bmp);
		else
			DrawRect(x, y, w, h, ORANGE);
	}

	if (fullrender || lastbg != bg)
	{
		DrawString(x + 10, y + (h / 2) - 10, text, bg, WHITE, 1);

		/* outline */
		DrawOutlineRect(x,y,w,h,bg);
		x++;
		y++;
		w-=2;
		h-=2;
		DrawOutlineRect(x,y,w,h,bg);

		lastbg = bg;
	}

	// If button is hot and active, but mouse button is not
	// down, the user must have clicked the button.
	if (uistate.mousedown == 0 && uistate.hotitem == id
			&& uistate.activeitem == id)
		return 1;

	// Otherwise, no clicky.
	return 0;
}
