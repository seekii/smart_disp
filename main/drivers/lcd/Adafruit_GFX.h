#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#include "gfxfont.h"
#include "stdint.h"

//Adafruit_GFX(int16_t w, int16_t h); // Constructor

// This MUST be defined by the subclass:
void drawPixel(int16_t x, int16_t y, uint16_t color);

// TRANSACTION API / CORE DRAW API
// These MAY be overridden by the subclass to provide device-specific
// optimized code.  Otherwise 'generic' versions are used.

//void writePixel(int16_t x, int16_t y, uint16_t color);
//void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
//void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
//void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);


// CONTROL API
// These MAY be overridden by the subclass to provide device-specific
// optimized code.  Otherwise 'generic' versions are used.
//void setRotation(uint8_t r);
//void invertDisplay(uint8_t i);

// BASIC DRAW API
// These MAY be overridden by the subclass to provide device-specific
// optimized code.  Otherwise 'generic' versions are used.
void
// It's good to implement those, even if using transaction API


drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color), drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color), fillRect(int16_t x,
		int16_t y, int16_t w, int16_t h, uint16_t color), fillScreen(uint16_t color),
		// Optional and probably not necessary to change
		drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color),
		drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void cp437(uint8_t x);

void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) ;

void getTextBounds(char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
void writex(uint8_t c);

void setTextSize(uint8_t s);
void setCursor(int16_t x, int16_t y);
void setTextColor2(uint16_t c, uint16_t b);
void setTextWrap(uint8_t w);
// These exist only with Adafruit_GFX (no subclass overrides)
//void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color), drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
//		uint16_t color), fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color), fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
//		uint8_t cornername, int16_t delta, uint16_t color), drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2,
//		uint16_t color), fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color), drawRoundRect(
//		int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color), fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
//		int16_t radius, uint16_t color), drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color), drawBitmap(
//		int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg), drawBitmap(int16_t x, int16_t y,
//		uint8_t *bitmap, int16_t w, int16_t h, uint16_t color), drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h,
//		uint16_t color, uint16_t bg), drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color),
//		drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h), drawGrayscaleBitmap(int16_t x, int16_t y,
//				uint8_t *bitmap, int16_t w, int16_t h), drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], const uint8_t mask[],
//				int16_t w, int16_t h), drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask, int16_t w, int16_t h), drawRGBBitmap(
//				int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h), drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w,
//				int16_t h), drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], const uint8_t mask[], int16_t w, int16_t h), drawRGBBitmap(
//				int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h), drawChar(int16_t x, int16_t y, unsigned char c,
//				uint16_t color, uint16_t bg, uint8_t size), setCursor(int16_t x, int16_t y), setTextColor(uint16_t c), setTextColor(uint16_t c,
//				uint16_t bg), setTextSize(uint8_t s), setTextWrap(boolean w), cp437(boolean x = true), setFont(const GFXfont *f = NULL),
//		getTextBounds(char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h), getTextBounds(
//				const __FlashStringHelper *s, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);

//void write(uint8_t);

int16_t height(void);
int16_t width(void);

uint8_t getRotation(void);

// get current cursor position (get rotation safe maximum values, using: width() for x, height() for y)
int16_t getCursorX(void);
int16_t getCursorY(void);

void
charBounds(char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
const int16_t WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
int16_t _width, _height, // Display w/h as modified by current rotation
		cursor_x, cursor_y;
uint16_t textcolor, textbgcolor;
uint8_t textsize, rotation;
uint8_t wrap,   // If set, 'wrap' text at right edge of display
		_cp437; // If set, use correct CP437 charset (default is off)
GFXfont *gfxFont;



#endif // _ADAFRUIT_GFX_H
