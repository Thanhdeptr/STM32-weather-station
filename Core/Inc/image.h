/*
 * image.h
 *
 *  Created on: 31 ����. 2019 �.
 *      Author: tabur
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>

typedef struct _tImage
{
  uint16_t 					xSize;
  uint16_t 					ySize;
  uint16_t 					bytesPerLine;
  uint8_t					bitsPerPixel;
  const unsigned char* 		pData;
} sImage;

#define GUI_BITMAP			sImage
#define GUI_CONST_STORAGE	const

extern GUI_CONST_STORAGE GUI_BITMAP bmSTLogo;
extern GUI_CONST_STORAGE GUI_BITMAP bmbgbgbgbg;
extern GUI_CONST_STORAGE GUI_BITMAP bmanhmungtho1;
extern GUI_CONST_STORAGE GUI_BITMAP bmanhmungtho4;
extern GUI_CONST_STORAGE GUI_BITMAP bmsunnycloud;
extern GUI_CONST_STORAGE GUI_BITMAP bmcloudy_icon;
extern GUI_CONST_STORAGE GUI_BITMAP bmsnowy_icon;
extern GUI_CONST_STORAGE GUI_BITMAP bmthunder_icon;

#endif /* IMAGE_H_ */
