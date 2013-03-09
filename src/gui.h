/**
  ******************************************************************************
  * @file    gui.h
  * @author  Barry Bingo
  * @brief   Simple GUI
  * Immediate Mode GUI implementation based on tutorials and code
  * found on http://sol.gfxile.net/imgui/
  ******************************************************************************/

#ifndef __GUI_H
#define __GUI_H

#include "RGB.h"

struct UIState
{
	uint8_t mousex;
	uint8_t mousey;
	uint8_t mousedown;

	uint8_t hotitem;
	uint8_t activeitem;  // each widget must have a unique ID
};

extern struct UIState uistate;

void DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, COLOR color);
void DrawString(uint16_t x, uint16_t y, uint8_t *pStr, COLOR textColor, COLOR backColor);

uint8_t RegionHit(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void Imgui_Prepare();
void Imgui_Finish();

#endif
