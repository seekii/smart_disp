/*
 This is the core graphics library for all our displays, providing a common
 set of graphics primitives (points, lines, circles, etc.).  It needs to be
 paired with a hardware-specific library for each display device we carry
 (to handle the lower-level functions).

 Adafruit invests time and resources providing this open source code, please
 support Adafruit & open-source hardware by purchasing products from Adafruit!

 Copyright (c) 2013 Adafruit Industries.  All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 - Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#include <lcd_gfx.h>
#include <lcd_ili9341.h>
#include "glcdfont.c"
#include "stdlib.h"
#include "string.h"


#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif





extern int16_t _width;
extern int16_t _height; // Display w/h as modified by current rotation
//extern uint8_t rotation;

static int16_t cursor_x;
static int16_t cursor_y;
static uint16_t textcolor;
static uint16_t textbgcolor;
static uint8_t textsize;

static uint8_t wrap;   // If set, 'wrap' text at right edge of display
static uint8_t _cp437; // If set, use correct CP437 charset (default is off)
static GFXfont *gfxFont;






void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		_swap_int16_t(x0, y0);
		_swap_int16_t(x1, y1);
	}

	if (x0 > x1) {
		_swap_int16_t(x0, x1);
		_swap_int16_t(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			writePixel(y0, x0, color);
		} else {
			writePixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    il9341_write_fill_rect(x, y, w, h, color);
}

void fillScreen(uint16_t color)
{
    il9341_write_fill_rect(0, 0, _width, _height, color);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {

	if (x0 == x1) {
		if (y0 > y1)
			_swap_int16_t(y0, y1);
		il9341_write_v_line(x0, y0, y1 - y0 + 1, color);
	} else if (y0 == y1) {
		if (x0 > x1)
			_swap_int16_t(x0, x1);
		il9341_write_h_line(x0, y0, x1 - x0 + 1, color);
	} else {
		//startWrite();
		writeLine(x0, y0, x1, y1, color);
		//endWrite();
	}
}

// Draw a circle outline
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	//startWrite();
	writePixel(x0, y0 + r, color);
	writePixel(x0, y0 - r, color);
	writePixel(x0 + r, y0, color);
	writePixel(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		writePixel(x0 + x, y0 + y, color);
		writePixel(x0 - x, y0 + y, color);
		writePixel(x0 + x, y0 - y, color);
		writePixel(x0 - x, y0 - y, color);
		writePixel(x0 + y, y0 + x, color);
		writePixel(x0 - y, y0 + x, color);
		writePixel(x0 + y, y0 - x, color);
		writePixel(x0 - y, y0 - x, color);
	}
	//endWrite();
}

void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
		uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x4) {
			writePixel(x0 + x, y0 + y, color);
			writePixel(x0 + y, y0 + x, color);
		}
		if (cornername & 0x2) {
			writePixel(x0 + x, y0 - y, color);
			writePixel(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8) {
			writePixel(x0 - y, y0 + x, color);
			writePixel(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1) {
			writePixel(x0 - y, y0 - x, color);
			writePixel(x0 - x, y0 - y, color);
		}
	}
}

// Used to do circles and roundrects
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
		int16_t delta, uint16_t color) {

	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if (cornername & 0x1) {
			il9341_write_v_line(x0 + x, y0 - y, 2 * y + 1 + delta, color);
			il9341_write_v_line(x0 + y, y0 - x, 2 * x + 1 + delta, color);
		}
		if (cornername & 0x2) {
			il9341_write_v_line(x0 - x, y0 - y, 2 * y + 1 + delta, color);
			il9341_write_v_line(x0 - y, y0 - x, 2 * x + 1 + delta, color);
		}
	}
}

void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
	//startWrite();
	il9341_write_v_line(x0, y0 - r, 2 * r + 1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
	//endWrite();
}

// Draw a rectangle
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
	il9341_write_h_line(x, y, w, color);
	il9341_write_h_line(x, y + h - 1, w, color);
	il9341_write_v_line(x, y, h, color);
	il9341_write_v_line(x + w - 1, y, h, color);
}

// Draw a rounded rectangle
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,
		uint16_t color) {
	// smarter version
	//startWrite();
	il9341_write_h_line(x + r, y, w - 2 * r, color); // Top
	il9341_write_h_line(x + r, y + h - 1, w - 2 * r, color); // Bottom
	il9341_write_v_line(x, y + r, h - 2 * r, color); // Left
	il9341_write_v_line(x + w - 1, y + r, h - 2 * r, color); // Right
	// draw four corners
	drawCircleHelper(x + r, y + r, r, 1, color);
	drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
	drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
	drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
	//endWrite();
}

// Fill a rounded rectangle
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,
		uint16_t color) {
	// smarter version
	//startWrite();
	il9341_write_fill_rect(x + r, y, w - 2 * r, h, color);

	// draw four corners
	fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
	fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
	//endWrite();
}

// Draw a triangle
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
		int16_t y2, uint16_t color) {
	drawLine(x0, y0, x1, y1, color);
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
		int16_t y2, uint16_t color) {

	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
		_swap_int16_t(y0, y1);
		_swap_int16_t(x0, x1);
	}
	if (y1 > y2) {
		_swap_int16_t(y2, y1);
		_swap_int16_t(x2, x1);
	}
	if (y0 > y1) {
		_swap_int16_t(y0, y1);
		_swap_int16_t(x0, x1);
	}

	//startWrite();
	if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if (x1 < a)
			a = x1;
		else if (x1 > b)
			b = x1;
		if (x2 < a)
			a = x2;
		else if (x2 > b)
			b = x2;
		il9341_write_h_line(a, y0, b - a + 1, color);
		//endWrite();
		return;
	}

	int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
			dx12 = x2 - x1, dy12 = y2 - y1;
	int32_t sa = 0, sb = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if (y1 == y2)
		last = y1;   // Include y1 scanline
	else
		last = y1 - 1; // Skip it

	for (y = y0; y <= last; y++) {
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		 a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		 b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		 */
		if (a > b)
			_swap_int16_t(a, b);
		il9341_write_h_line(a, y, b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++) {
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		 a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		 b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		 */
		if (a > b)
			_swap_int16_t(a, b);
		il9341_write_h_line(a, y, b - a + 1, color);
	}
	//endWrite();
}



/*--------------BITMAP / XBITMAP / GRAYSCALE / RGB BITMAP FUNCTIONS ---------------------*/

// Draw a RAM-resident 1-bit image at the specified (x,y) position,
// using the specified foreground color (unset bits are transparent).
void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

	int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
	uint8_t byte = 0;

	//startWrite();
	for (int16_t j = 0; j < h; j++, y++) {
		for (int16_t i = 0; i < w; i++) {
			if (i & 7)
				byte <<= 1;
			else
				byte = bitmap[j * byteWidth + i / 8];
			if (byte & 0x80)
				writePixel(x + i, y, color);
		}
	}
	//endWrite();
}



// Draw a RAM-resident 16-bit image (RGB 5/6/5) at the specified (x,y)
// position.  For 16-bit display devices; no color reduction performed.

void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) {
	//startWrite();
	for (int16_t j = 0; j < h; j++, y++) {
		for (int16_t i = 0; i < w; i++) {
			writePixel(x + i, y, bitmap[j * w + i]);
		}
	}
	//endWrite();
}

// Draw a RAM-resident 16-bit image (RGB 5/6/5) with a 1-bit mask
// (set bits = opaque, unset bits = clear) at the specified (x,y) pos.
// BOTH buffers (color and mask) must be RAM-resident, no mix-and-match.
// For 16-bit display devices; no color reduction performed.
void drawRGBBitmapMask(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h) {
	int16_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
	uint8_t byte = 0;
	//startWrite();
	for (int16_t j = 0; j < h; j++, y++) {
		for (int16_t i = 0; i < w; i++) {
			if (i & 7)
				byte <<= 1;
			else
				byte = mask[j * bw + i / 8];
			if (byte & 0x80) {
				writePixel(x + i, y, bitmap[j * w + i]);
			}
		}
	}
	//endWrite();
}

// TEXT- AND CHARACTER-HANDLING FUNCTIONS ----------------------------------

// Draw a character
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
		uint16_t bg, uint8_t size) {

	//if (!gfxFont) { // 'Classic' built-in font
	if (1)
	{ // 'Classic' built-in font

		if ((x >= _width) || // Clip right
				(y >= _height) || // Clip bottom
				((x + 6 * size - 1) < 0) || // Clip left
				((y + 8 * size - 1) < 0))   // Clip top
			return;

		if (!_cp437 && (c >= 176))
			c++; // Handle 'classic' charset behavior

		//startWrite();
		for (int8_t i = 0; i < 5; i++) { // Char bitmap = 5 columns
			uint8_t line = (font[c * 5 + i]);
			for (int8_t j = 0; j < 8; j++, line >>= 1) {
				if (line & 1) {
					if (size == 1)
						writePixel(x + i, y + j, color);
					else
						il9341_write_fill_rect(x + i * size, y + j * size, size, size,
								color);
				} else if (bg != color) {
					if (size == 1)
						writePixel(x + i, y + j, bg);
					else
						il9341_write_fill_rect(x + i * size, y + j * size, size, size,
								bg);
				}
			}
		}
		if (bg != color) { // If opaque, draw vertical line for last column
			if (size == 1)
				il9341_write_v_line(x + 5, y, 8, bg);
			else
				il9341_write_fill_rect(x + 5 * size, y, size, 8 * size, bg);
		}
		//endWrite();

	} else { // Custom font

		// Character is assumed previously filtered by write() to eliminate
		// newlines, returns, non-printable characters, etc.  Calling
		// drawChar() directly with 'bad' characters of font may cause mayhem!

		c -= (uint8_t) (gfxFont->first);
		GFXglyph *glyph = &(((GFXglyph *) gfxFont->glyph))[c];
		uint8_t *bitmap = (uint8_t *) gfxFont->bitmap;

		uint16_t bo = (glyph->bitmapOffset);
		uint8_t w = (glyph->width), h = (glyph->height);
		int8_t xo = (glyph->xOffset), yo = (glyph->yOffset);
		uint8_t xx, yy, bits = 0, bit = 0;
		int16_t xo16 = 0, yo16 = 0;

		if (size > 1) {
			xo16 = xo;
			yo16 = yo;
		}

		// Todo: Add character clipping here

		// NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
		// THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
		// has typically been used with the 'classic' font to overwrite old
		// screen contents with new data.  This ONLY works because the
		// characters are a uniform size; it's not a sensible thing to do with
		// proportionally-spaced fonts with glyphs of varying sizes (and that
		// may overlap).  To replace previously-drawn text when using a custom
		// font, use the getTextBounds() function to determine the smallest
		// rectangle encompassing a string, erase the area with fillRect(),
		// then draw new text.  This WILL infortunately 'blink' the text, but
		// is unavoidable.  Drawing 'background' pixels will NOT fix this,
		// only creates a new set of problems.  Have an idea to work around
		// this (a canvas object type for MCUs that can afford the RAM and
		// displays supporting setAddrWindow() and pushColors()), but haven't
		// implemented this yet.

		//startWrite();
		for (yy = 0; yy < h; yy++) {
			for (xx = 0; xx < w; xx++) {
				if (!(bit++ & 7)) {
					bits = (bitmap[bo++]);
				}
				if (bits & 0x80) {
					if (size == 1) {
						writePixel(x + xo + xx, y + yo + yy, color);
					} else {
						il9341_write_fill_rect(x + (xo16 + xx) * size,
								y + (yo16 + yy) * size, size, size, color);
					}
				}
				bits <<= 1;
			}
		}
		//endWrite();

	} // End classic vs custom font
}


void gfx_write_char(uint8_t c, PRINT_DIRECTION direction)
{

    if(!gfxFont)
    { // 'Classic' built-in font

        if(c == '\n')
        {                        // Newline?
            cursor_x = 0;                     // Reset x to zero,
            cursor_y += textsize * 8;          // advance y one line
        }
        else if(c != '\r')
        {                 // Ignore carriage returns
            if(wrap && ((cursor_x + textsize * 6) > _width))
            { // Off right?
                cursor_x = 0;                 // Reset x to zero,
                cursor_y += textsize * 8;      // advance y one line
            }
            drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
            if(direction == DIR_FORWARD)
            {
                cursor_x += textsize * 6;          // Advance x one char
            }
            else
            {
                cursor_x -= textsize * 6;          // Advance x one char
            }
        }

    }
    else
    { // Custom font

        if(c == '\n')
        {
            cursor_x = 0;
            cursor_y += (int16_t) textsize * (uint8_t) (gfxFont->yAdvance);
        }
        else if(c != '\r')
        {
            uint8_t first = (gfxFont->first);
            if((c >= first) && (c <= (uint8_t) (gfxFont->last)))
            {
                GFXglyph *glyph = &(((GFXglyph *) (gfxFont->glyph))[c - first]);
                uint8_t w = (glyph->width), h = (glyph->height);
                if((w > 0) && (h > 0))
                { // Is there an associated bitmap?
                    int16_t xo = (int8_t) (glyph->xOffset); // sic
                    if(wrap && ((cursor_x + textsize * (xo + w)) > _width))
                    {
                        cursor_x = 0;
                        cursor_y += (int16_t) textsize * (uint8_t) (gfxFont->yAdvance);
                    }
                    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
                }
                if(direction == DIR_FORWARD)
                {
                    cursor_x += (uint8_t) (glyph->xAdvance) * (int16_t) textsize;
                }
                else
                {
                    cursor_x -= (uint8_t) (glyph->xAdvance) * (int16_t) textsize;
                }
            }
        }

    }
}


void writeText(char *str, PRINT_DIRECTION direction)
{
    uint16_t len = strlen((char*) str);

    if(direction == DIR_FORWARD)
    {
        for(int16_t i = 0; i < len; i++)
        {
            gfx_write_char(*(str + i), direction);
        }

    }
    else
    {
        for(int16_t i = len; i > 0 ; i--)
        {
            gfx_write_char(*(str + i), direction);
        }
    }

}


void setCursor(int16_t x, int16_t y)
{
    cursor_x = x;
    cursor_y = y;
}


void setTextSize(uint8_t s) {
	textsize = (s > 0) ? s : 1;
}

void setTextColor(uint16_t c) {
	// For 'transparent' background, we'll set the bg
	// to the same as fg instead of using a flag
	textcolor = textbgcolor = c;
}

void setTextColor2(uint16_t c, uint16_t b) {
	textcolor = c;
	textbgcolor = b;
}

void setTextWrap(uint8_t w) {
	wrap = w;
}



// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it.  But a TON of code has been written
// with the erroneous character indices.  By default, the library uses the
// original 'wrong' behavior and old sketches will still work.  Pass 'true'
// to this function to use correct CP437 character values in your code.
void cp437(uint8_t x) {
	_cp437 = x;
}

void setFont(const GFXfont *f) {
	if (f) {            // Font struct pointer passed in?
		if (!gfxFont) { // And no current font struct?
			// Switching from classic to new font behavior.
			// Move cursor pos down 6 pixels so it's on baseline.
			cursor_y += 6;
		}
	} else if (gfxFont) { // NULL passed.  Current font struct defined?
		// Switching from new to classic font behavior.
		// Move cursor pos up 6 pixels so it's at top-left of char.
		cursor_y -= 6;
	}
	gfxFont = (GFXfont *) f;
}

// Broke this out as it's used by both the PROGMEM- and RAM-resident
// getTextBounds() functions.
void getCharBounds(char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy)
{
    if (gfxFont)
    {
        if (c == '\n')
        { // Newline?
            *x = 0;    // Reset x to zero, advance y by one line
            *y += textsize * (uint8_t) (gfxFont->yAdvance);
        }
        else if (c != '\r')
        { // Not a carriage return; is normal char
            uint8_t first = (gfxFont->first), last = (gfxFont->last);
            if ((c >= first) && (c <= last))
            { // Char present in this font?
                GFXglyph *glyph = &(((GFXglyph *) (gfxFont->glyph))[c - first]);
                uint8_t gw = (glyph->width), gh = (glyph->height), xa = (glyph->xAdvance);
                int8_t xo = (glyph->xOffset), yo = (glyph->yOffset);
                if (wrap && ((*x + (((int16_t) xo + gw) * textsize)) > _width))
                {
                    *x = 0; // Reset x to zero, advance y by one line
                    *y += textsize * (uint8_t) (gfxFont->yAdvance);
                }
                int16_t ts = (int16_t) textsize, x1 = *x + xo * ts, y1 = *y + yo * ts, x2 = x1 + gw * ts - 1, y2 = y1
                        + gh * ts - 1;
                if (x1 < *minx)
                    *minx = x1;
                if (y1 < *miny)
                    *miny = y1;
                if (x2 > *maxx)
                    *maxx = x2;
                if (y2 > *maxy)
                    *maxy = y2;
                *x += xa * ts;
            }
        }

    }
    else
    { // Default font

        if (c == '\n')
        {                     // Newline?
            *x = 0;                        // Reset x to zero,
            *y += textsize * 8;             // advance y one line
            // min/max x/y unchaged -- that waits for next 'normal' character
        }
        else if (c != '\r')
        {  // Normal char; ignore carriage returns
            if (wrap && ((*x + textsize * 6) > _width))
            { // Off right?
                *x = 0;                    // Reset x to zero,
                *y += textsize * 8;         // advance y one line
            }
            int x2 = *x + textsize * 6 - 1, // Lower-right pixel of char
            y2 = *y + textsize * 8 - 1;
            if (x2 > *maxx)
                *maxx = x2;      // Track max x, y
            if (y2 > *maxy)
                *maxy = y2;
            if (*x < *minx)
                *minx = *x;      // Track min x, y
            if (*y < *miny)
                *miny = *y;
            *x += textsize * 6;             // Advance x one char
        }
    }
}

// Pass string and a cursor position, returns UL corner and W,H.
void getTextBounds(char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
    uint8_t c; // Current character

    *x1 = x;
    *y1 = y;
    *w = *h = 0;

    int16_t minx = _width, miny = _height, maxx = -1, maxy = -1;

    while ((c = *str++))
        getCharBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);

    if (maxx >= minx)
    {
        *x1 = minx;
        *w = maxx - minx + 1;
    }
    if (maxy >= miny)
    {
        *y1 = miny;
        *h = maxy - miny + 1;
    }
}


int16_t getCursorX(void)
{
    return cursor_x;
}
int16_t getCursorY(void)
{
    return cursor_y;
}


void gfx_write_float(int16_t x, int16_t y, uint16_t color, uint8_t size, float val, uint8_t dec_pl, uint8_t use_sign, PRINT_DIRECTION direction )
{
    char buff[20];
    uint16_t len = 0;

    if (use_sign)
    {
        len = sprintf(buff, "%+.*f",dec_pl, val);
    }
    else
    {
        len = sprintf(buff, "%.*f",dec_pl, val);
    }

    buff[len] = '\0';

    setCursor( x,  y);
    setTextColor(color);
    setTextSize(size);

    writeText(buff,direction);

}

void gfx_write_dec(int16_t x, int16_t y, uint16_t color, uint8_t size, uint32_t val, uint8_t use_sign,PRINT_DIRECTION direction )
{
    char buff[20];
    uint16_t len = 0;

    if (use_sign)
    {
        len = sprintf(buff, "%+05d", val);
    }
    else
    {
        len = sprintf(buff, "%05d", val);
    }

    buff[len] = '\0';

    setCursor( x,  y);
    setTextColor(color);
    setTextSize(size);

    setTextColor2(color, ILI9341_BLACK);

    writeText(buff, direction);
}






