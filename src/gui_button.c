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

/**
 * @brief  Simple button IMGUI widget
 * @param  id: Unique ID
 * 		x,y: x and y drawing positions
 * 		w,h: width and height
 * 		text: Text displayed on button
 * @retval 1: If button has been clicked
 *         0: If button has not been clicked
 */
uint8_t ButtonWidget(uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
		uint8_t* text)
{
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
		bg = BLUE;
	}

	DrawRect(x, y, w, h, bg);
	DrawString(x + 20, y + (h / 2) - 10, text, WHITE, bg);

	// If button is hot and active, but mouse button is not
	// down, the user must have clicked the button.
	if (uistate.mousedown == 0 && uistate.hotitem == id
			&& uistate.activeitem == id)
		return 1;

	// Otherwise, no clicky.
	return 0;
}

