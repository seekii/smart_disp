#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#include "gfxfont.h"
#include "stdint.h"



typedef enum
{
    FONT_SIZE_1 = 1,
    FONT_SIZE_2,
    FONT_SIZE_3,
    FONT_SIZE_4,
    FONT_SIZE_5,
}FONT_SIZE;

typedef enum
{
    DEC_PLACES_0,
    DEC_PLACES_1,
    DEC_PLACES_2,
    DEC_PLACES_3,
    DEC_PLACES_4,
    DEC_PLACES_5
}DECIMAL_PLACES;


typedef enum
{
    WITHOUT_SIGN,
    WITH_SIGN
} SING_USE;


typedef enum
{
    DIR_FORWARD,
    DIR_BACKWARD
} PRINT_DIRECTION;







//Adafruit_GFX(int16_t w, int16_t h); // Constructor

// This MUST be defined by the subclass:
void drawPixel(int16_t x, int16_t y, uint16_t color);

// TRANSACTION API / CORE DRAW API
// These MAY be overridden by the subclass to provide device-specific
// optimized code.  Otherwise 'generic' versions are used.

//void writePixel(int16_t x, int16_t y, uint16_t color);
void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color); //TODO add code
void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);


// CONTROL API
// These MAY be overridden by the subclass to provide device-specific
// optimized code.  Otherwise 'generic' versions are used.
void setRotation(uint8_t r);
void invertDisplay(uint8_t i);

// BASIC DRAW API
// These MAY be overridden by the subclass to provide device-specific
// optimized code.  Otherwise 'generic' versions are used.

// It's good to implement those, even if using transaction API






void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void fillScreen(uint16_t color);

// Optional and probably not necessary to change
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);


// These exist only with Adafruit_GFX (no subclass overrides)
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,uint16_t color);
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2,uint16_t color);
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,int16_t radius, uint16_t color);
void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w,int16_t h);
void drawRGBBitmapMask(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h);
void drawChar(int16_t x, int16_t y, unsigned char c,uint16_t color, uint16_t bg, uint8_t size);
void gfx_write_char(uint8_t c, PRINT_DIRECTION direction);
void writeText(char *str, PRINT_DIRECTION direction);


void setCursor(int16_t x, int16_t y);
void setTextColor(uint16_t c);
void setTextColor2(uint16_t c, uint16_t bg);
void setTextSize(uint8_t s);
void setTextWrap(uint8_t w);
void cp437(uint8_t x);
void setFont(const GFXfont *f);
void getTextBounds(char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);


//void write(uint8_t);

int16_t height(void);
int16_t width(void);

uint8_t getRotation(void);

// get current cursor position (get rotation safe maximum values, using: width() for x, height() for y)
int16_t getCursorX(void);
int16_t getCursorY(void);

void getCharBounds(char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);



/*Custom text out*/


void gfx_write_float(int16_t x, int16_t y, uint16_t color, uint8_t size, float val, uint8_t dec_pl, uint8_t use_sign,PRINT_DIRECTION direction  );
void gfx_write_dec(int16_t x, int16_t y, uint16_t color, uint8_t size, uint32_t val, uint8_t use_sign,PRINT_DIRECTION direction  );

#endif // _ADAFRUIT_GFX_H
