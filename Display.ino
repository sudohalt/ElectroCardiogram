#include <SPI.h>
#include <stdint.h>
#include "Display.h"

/* Pin definitions:
Most of these pins can be moved to any digital or analog pin.
DN(MOSI)and SCLK should be left where they are (SPI pins). The
LED (backlight) pin should remain on a PWM-capable pin. */
const int scePin = 10;   // SCE - Chip select, pin 3 on LCD.
const int rstPin = 8;   // RST - Reset, pin 4 on LCD.
const int dcPin = 9;    // DC - Data/Command, pin 5 on LCD.
const int sdinPin = 11;  // DN(MOSI) - Serial data, pin 6 on LCD.
const int sclkPin = 13;  // SCLK - Serial clock, pin 7 on LCD.
const int blPin = 6;    // LED - Backlight LED, pin 8 on LCD.

static const byte ASCII[][5] = {
  // First 32 characters (0x00-0x19) are ignored. These are
  // non-displayable, control characters.
   {0x00, 0x00, 0x00, 0x00, 0x00} // 0x20  
  ,{0x00, 0x00, 0x5f, 0x00, 0x00} // 0x21 !
  ,{0x00, 0x07, 0x00, 0x07, 0x00} // 0x22 "
  ,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 0x23 #
  ,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 0x24 $
  ,{0x23, 0x13, 0x08, 0x64, 0x62} // 0x25 %
  ,{0x36, 0x49, 0x55, 0x22, 0x50} // 0x26 &
  ,{0x00, 0x05, 0x03, 0x00, 0x00} // 0x27 '
  ,{0x00, 0x1c, 0x22, 0x41, 0x00} // 0x28 (
  ,{0x00, 0x41, 0x22, 0x1c, 0x00} // 0x29 )
  ,{0x14, 0x08, 0x3e, 0x08, 0x14} // 0x2a *
  ,{0x08, 0x08, 0x3e, 0x08, 0x08} // 0x2b +
  ,{0x00, 0x50, 0x30, 0x00, 0x00} // 0x2c ,
  ,{0x08, 0x08, 0x08, 0x08, 0x08} // 0x2d -
  ,{0x00, 0x60, 0x60, 0x00, 0x00} // 0x2e .
  ,{0x20, 0x10, 0x08, 0x04, 0x02} // 0x2f /
  ,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 0x30 0
  ,{0x00, 0x42, 0x7f, 0x40, 0x00} // 0x31 1
  ,{0x42, 0x61, 0x51, 0x49, 0x46} // 0x32 2
  ,{0x21, 0x41, 0x45, 0x4b, 0x31} // 0x33 3
  ,{0x18, 0x14, 0x12, 0x7f, 0x10} // 0x34 4
  ,{0x27, 0x45, 0x45, 0x45, 0x39} // 0x35 5
  ,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 0x36 6
  ,{0x01, 0x71, 0x09, 0x05, 0x03} // 0x37 7
  ,{0x36, 0x49, 0x49, 0x49, 0x36} // 0x38 8
  ,{0x06, 0x49, 0x49, 0x29, 0x1e} // 0x39 9
  ,{0x00, 0x36, 0x36, 0x00, 0x00} // 0x3a :
  ,{0x00, 0x56, 0x36, 0x00, 0x00} // 0x3b ;
  ,{0x08, 0x14, 0x22, 0x41, 0x00} // 0x3c <
  ,{0x14, 0x14, 0x14, 0x14, 0x14} // 0x3d =
  ,{0x00, 0x41, 0x22, 0x14, 0x08} // 0x3e >
  ,{0x02, 0x01, 0x51, 0x09, 0x06} // 0x3f ?
  ,{0x32, 0x49, 0x79, 0x41, 0x3e} // 0x40 @
  ,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 0x41 A
  ,{0x7f, 0x49, 0x49, 0x49, 0x36} // 0x42 B
  ,{0x3e, 0x41, 0x41, 0x41, 0x22} // 0x43 C
  ,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 0x44 D
  ,{0x7f, 0x49, 0x49, 0x49, 0x41} // 0x45 E
  ,{0x7f, 0x09, 0x09, 0x09, 0x01} // 0x46 F
  ,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 0x47 G
  ,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 0x48 H
  ,{0x00, 0x41, 0x7f, 0x41, 0x00} // 0x49 I
  ,{0x20, 0x40, 0x41, 0x3f, 0x01} // 0x4a J
  ,{0x7f, 0x08, 0x14, 0x22, 0x41} // 0x4b K
  ,{0x7f, 0x40, 0x40, 0x40, 0x40} // 0x4c L
  ,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 0x4d M
  ,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 0x4e N
  ,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 0x4f O
  ,{0x7f, 0x09, 0x09, 0x09, 0x06} // 0x50 P
  ,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 0x51 Q
  ,{0x7f, 0x09, 0x19, 0x29, 0x46} // 0x52 R
  ,{0x46, 0x49, 0x49, 0x49, 0x31} // 0x53 S
  ,{0x01, 0x01, 0x7f, 0x01, 0x01} // 0x54 T
  ,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 0x55 U
  ,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 0x56 V
  ,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 0x57 W
  ,{0x63, 0x14, 0x08, 0x14, 0x63} // 0x58 X
  ,{0x07, 0x08, 0x70, 0x08, 0x07} // 0x59 Y
  ,{0x61, 0x51, 0x49, 0x45, 0x43} // 0x5a Z
  ,{0x00, 0x7f, 0x41, 0x41, 0x00} // 0x5b [
  ,{0x02, 0x04, 0x08, 0x10, 0x20} // 0x5c \
  ,{0x00, 0x41, 0x41, 0x7f, 0x00} // 0x5d ]
  ,{0x00, 0x41, 0x41, 0x7f, 0x00} // 0x5d ]
  ,{0x04, 0x02, 0x01, 0x02, 0x04} // 0x5e ^
  ,{0x40, 0x40, 0x40, 0x40, 0x40} // 0x5f _
  ,{0x00, 0x01, 0x02, 0x04, 0x00} // 0x60 `
  ,{0x20, 0x54, 0x54, 0x54, 0x78} // 0x61 a
  ,{0x7f, 0x48, 0x44, 0x44, 0x38} // 0x62 b
  ,{0x38, 0x44, 0x44, 0x44, 0x20} // 0x63 c
  ,{0x38, 0x44, 0x44, 0x48, 0x7f} // 0x64 d
  ,{0x38, 0x54, 0x54, 0x54, 0x18} // 0x65 e
  ,{0x08, 0x7e, 0x09, 0x01, 0x02} // 0x66 f
  ,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 0x67 g
  ,{0x7f, 0x08, 0x04, 0x04, 0x78} // 0x68 h
  ,{0x00, 0x44, 0x7d, 0x40, 0x00} // 0x69 i
  ,{0x20, 0x40, 0x44, 0x3d, 0x00} // 0x6a j 
  ,{0x7f, 0x10, 0x28, 0x44, 0x00} // 0x6b k
  ,{0x00, 0x41, 0x7f, 0x40, 0x00} // 0x6c l
  ,{0x7c, 0x04, 0x18, 0x04, 0x78} // 0x6d m
  ,{0x7c, 0x08, 0x04, 0x04, 0x78} // 0x6e n
  ,{0x38, 0x44, 0x44, 0x44, 0x38} // 0x6f o
  ,{0x7c, 0x14, 0x14, 0x14, 0x08} // 0x70 p
  ,{0x08, 0x14, 0x14, 0x18, 0x7c} // 0x71 q
  ,{0x7c, 0x08, 0x04, 0x04, 0x08} // 0x72 r
  ,{0x48, 0x54, 0x54, 0x54, 0x20} // 0x73 s
  ,{0x04, 0x3f, 0x44, 0x40, 0x20} // 0x74 t
  ,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 0x75 u
  ,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 0x76 v
  ,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 0x77 w
  ,{0x44, 0x28, 0x10, 0x28, 0x44} // 0x78 x
  ,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 0x79 y
  ,{0x44, 0x64, 0x54, 0x4c, 0x44} // 0x7a z
  ,{0x00, 0x08, 0x36, 0x41, 0x00} // 0x7b {
  ,{0x00, 0x00, 0x7f, 0x00, 0x00} // 0x7c |
  ,{0x00, 0x41, 0x36, 0x08, 0x00} // 0x7d }
  ,{0x10, 0x08, 0x08, 0x10, 0x08} // 0x7e ~
  ,{0x78, 0x46, 0x41, 0x46, 0x78} // 0x7f DEL
};

static const byte Digits[][4][18] =
{
 {
    { 0xE0, 0xF0, 0xF8, 0xF4, 0xEE, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0xEE, 0xF4, 0xF8, 0xF0, 0xE0 },
    { 0x1F, 0x3F, 0x7F, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x7F, 0x3F, 0x1F },
    { 0xFC, 0xFE, 0xFF, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0xFF, 0xFE, 0xFC },
    { 0x03, 0x07, 0x0F, 0x17, 0x3B, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x3B, 0x17, 0x0F, 0x07, 0x03 },
  },
  {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF8, 0xF0, 0xE0 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x7F, 0x3F, 0x1F },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0xFF, 0xFE, 0xFC },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x07, 0x03 },
  },
  {
    { 0x00, 0x00, 0x00, 0x04, 0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0xEE, 0xF4, 0xF8, 0xF0, 0xE0 },
    { 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xDF, 0xBF, 0x7F, 0x3F, 0x1F },
    { 0xFC, 0xFE, 0xFF, 0xFE, 0xFD, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00 },
    { 0x03, 0x07, 0x0F, 0x17, 0x3B, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x38, 0x10, 0x00, 0x00, 0x00 },
  },
  {
    { 0x00, 0x00, 0x00, 0x04, 0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0xEE, 0xF4, 0xF8, 0xF0, 0xE0 },
    { 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xDF, 0xBF, 0x7F, 0x3F, 0x1F },
    { 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0xFD, 0xFE, 0xFF, 0xFE, 0xFC },
    { 0x00, 0x00, 0x00, 0x10, 0x38, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x3B, 0x17, 0x0F, 0x07, 0x03 },
  },
  {
    { 0xE0, 0xF0, 0xF8, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF8, 0xF0, 0xE0 },
    { 0x1F, 0x3F, 0x7F, 0xBF, 0xDF, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xDF, 0xBF, 0x7F, 0x3F, 0x1F },
    { 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0xFD, 0xFE, 0xFF, 0xFE, 0xFC },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x07, 0x03 },
  },
  {
    { 0xE0, 0xF0, 0xF8, 0xF4, 0xEE, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00, 0x00 },
    { 0x1F, 0x3F, 0x7F, 0xBF, 0xDF, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0xFD, 0xFE, 0xFF, 0xFE, 0xFC },
    { 0x00, 0x00, 0x00, 0x10, 0x38, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x3B, 0x17, 0x0F, 0x07, 0x03 },
  },
  {
    { 0xE0, 0xF0, 0xF8, 0xF4, 0xEE, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00, 0x00 },
    { 0x1F, 0x3F, 0x7F, 0xBF, 0xDF, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00 },
    { 0xFC, 0xFE, 0xFF, 0xFE, 0xFD, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0xFD, 0xFE, 0xFF, 0xFE, 0xFC },
    { 0x03, 0x07, 0x0F, 0x17, 0x3B, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x3B, 0x17, 0x0F, 0x07, 0x03 },
  },
  {
    { 0x00, 0x00, 0x00, 0x04, 0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0xEE, 0xF4, 0xF8, 0xF0, 0xE0 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x7F, 0x3F, 0x1F },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0xFF, 0xFE, 0xFC },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x07, 0x03 },
  },
  {
    { 0xE0, 0xF0, 0xF8, 0xF4, 0xEE, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0xEE, 0xF4, 0xF8, 0xF0, 0xE0 },
    { 0x1F, 0x3F, 0x7F, 0xBF, 0xDF, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xDF, 0xBF, 0x7F, 0x3F, 0x1F },
    { 0xFC, 0xFE, 0xFF, 0xFE, 0xFD, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0xFD, 0xFE, 0xFF, 0xFE, 0xFC },
    { 0x03, 0x07, 0x0F, 0x17, 0x3B, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x3B, 0x17, 0x0F, 0x07, 0x03 },
  },
  {
    { 0xE0, 0xF0, 0xF8, 0xF4, 0xEE, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0xEE, 0xF4, 0xF8, 0xF0, 0xE0 },
    { 0x1F, 0x3F, 0x7F, 0xBF, 0xDF, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xDF, 0xBF, 0x7F, 0x3F, 0x1F },
    { 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0xFD, 0xFE, 0xFF, 0xFE, 0xFC },
    { 0x00, 0x00, 0x00, 0x10, 0x38, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x3B, 0x17, 0x0F, 0x07, 0x03 },
  }
};
/* The displayMap variable stores a buffer representation of the
pixels on our display. There are 504 total bits in this array,
same as how many pixels there are on a 84 x 48 display.

Each byte in this array covers a 8-pixel vertical block on the
display. Each successive byte covers the next 8-pixel column over
until you reach the right-edge of the display and step down 8 rows.

To update the display, we first have to write to this array, then
call the updateDisplay() function, which sends this whole array
to the PCD8544.

Because the PCD8544 won't let us write individual pixels at a
time, this is how we can make targeted changes to the display.
At start the displayMap contains the begining logo */
byte displayMap[LCD_WIDTH * LCD_HEIGHT / 8] = {
  0xFF, 0xFF, 0xFF, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x87, 0x87, 0x07, 0x07, 0x87, 0x87, 0x87, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0xF0, 0xF0, 0x78,
  0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x08, 0x08, 0xFF,
  0xFC, 0x00, 0x60, 0xF8, 0xFC, 0x6C, 0x4C, 0x4C, 0x4C, 0x70, 0x00, 0xCC, 0xEC, 0x6C, 0x6C, 0xFC,
  0xFC, 0xF0, 0x00, 0x00, 0xFC, 0xFC, 0x1C, 0x0C, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0xFE, 0xFF, 0x0C,
  0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0xF0, 0xF0, 0x78,
  0x18, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  0x03, 0x00, 0x00, 0x03, 0x01, 0x02, 0x00, 0x01, 0x03, 0x07, 0x06, 0x06, 0x07, 0x03, 0x00, 0x03,
  0x03, 0x06, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x02, 0x01, 0x03, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xE0, 0x00, 0x80, 0xF0,
  0xF0, 0xF0, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80,
  0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0xB0, 0xB0, 0x00, 0x00, 0x80, 0x80, 0xE0, 0xC0, 0xA0, 0x80,
  0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80,
  0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x3F, 0x03,
  0x03, 0x0F, 0x0F, 0x03, 0x7F, 0x7F, 0x00, 0x2F, 0x7F, 0x63, 0x61, 0x23, 0x57, 0x3F, 0x00, 0x00,
  0x7F, 0x7F, 0x01, 0x03, 0x7F, 0x00, 0x00, 0x40, 0x41, 0x41, 0x7F, 0x7F, 0x40, 0x40, 0x01, 0x01,
  0x01, 0x7F, 0x7F, 0x61, 0x61, 0x40, 0x00, 0x2F, 0x5F, 0x63, 0x61, 0x23, 0x57, 0x3F, 0x00, 0x00,
  0x7F, 0x7F, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF
};

void setPixel(int x, int y, int bw)
{
  // First, double check that the coordinate is in range.
  if ((x >= 0) && (x < LCD_WIDTH) && (y >= 0) && (y < LCD_HEIGHT)) {
    byte shift = y % 8;
    if (bw) // If black, set the bit.
      displayMap[x + (y/8)*LCD_WIDTH] |= 1<<shift;
    else   // If white clear the bit.
      displayMap[x + (y/8)*LCD_WIDTH] &= ~(1<<shift);
  }
}

/* Function: drawDigit
 * ---------
 * This function draws three large digits on Nokia 5110 display
 *
 * Parameters:
 *   int d1: The hundreds place digit in a three digit number
 *   int d2: The tens place digit in a three digit number
 *   int d3: The ones place digit in a three digit number
 *   int x: x-coordinate to start drawing (top left corner is 0,0)
 *   int y: y-coordinate to start drawing
 *   bool bw: Color to draw true for black, false for white
 *
 * Returns:
 *   void
 */
void drawDigit(int d1, int d2, int d3, int x, int y, int bw) {
  byte loc = 1;
  int nx, ny;
  byte i;
  for(i = 0; i < 4; i++, loc+=8) {
    byte col;
    for(col = 0; col < 18; col++) {
      byte k;
      for (k = 0; k < 8; k++) {
        if (Digits[d1][i][col] & (0x01 << k))
          setPixel(x + col, (y + k + loc), bw);
        else
          setPixel(x + col, (y + k + loc), !bw);
        nx = x + col;
        ny =  (y + k + loc);
      }
    }
  }
  x = nx + 10;
  loc = 1;
  for(i = 0; i < 4; i++, loc+=8) {
    byte col;
    for(col = 0; col < 18; col++) {
      byte k;
      for (k = 0; k < 8; k++) {
        if (Digits[d2][i][col] & (0x01 << k))
          setPixel(x + col, (y + k + loc), bw);
        else
          setPixel(x + col, (y + k + loc), !bw);
        nx = x + col;
        ny =  (y + k + loc);
      }
    }
  }
  loc = 1;
  x = nx + 10;
  for(i = 0; i < 4; i++, loc+=8) {
    byte col;
    for(col = 0; col < 18; col++) {
      byte k;
      for (k = 0; k < 8; k++) {
        if (Digits[d3][i][col] & (0x01 << k))
          setPixel(x + col, (y + k + loc), bw);
        else
          setPixel(x + col, (y + k + loc), !bw);
        nx = x + col;
        ny =  (y + k + loc);
      }
    }
  }
}

// This function will draw an array over the screen. (For now) the
// array must be the same size as the screen, covering the entirety
// of the display.
void setBitmap(char *bitArray)
{
  int i;
  for (i = 0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++)
    displayMap[i] = bitArray[i];
}

// This function clears the entire display either white (0) or
// black (1).
// The screen won't actually clear until you call updateDisplay()!
void clearDisplay(int bw)
{
  int i;
  for (i = 0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++) {
    if (bw)
      displayMap[i] = 0xFF;
    else
      displayMap[i] = 0;
  }
}

// Helpful function to directly command the LCD to go to a
// specific x,y coordinate.
void gotoXY(int x, int y)
{
  LCDWrite(0, 0x80 | x);  // Column.
  LCDWrite(0, 0x40 | y);  // Row.  ?
}

// This will actually draw on the display, whatever is currently
// in the displayMap array.
void updateDisplay()
{
  gotoXY(0, 0);
  int i;
  for (i = 0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++) {
    LCDWrite(LCD_DATA, displayMap[i]);
  }
}

// Set contrast can set the LCD Vop to a value between 0 and 127.
// 40-60 is usually a pretty good range.
void setContrast(byte contrast)
{
  LCDWrite(LCD_COMMAND, 0x21); //Tell LCD that extended commands follow
  LCDWrite(LCD_COMMAND, 0x80 | contrast); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
  LCDWrite(LCD_COMMAND, 0x20); //Set display mode
}

/* There are two ways to do this. Either through direct commands
to the display, or by swapping each bit in the displayMap array.
We'll leave both methods here, comment one or the other out if
you please. */
void invertDisplay()
{
  /* Direct LCD Command option
  LCDWrite(LCD_COMMAND, 0x20); //Tell LCD that extended commands follow
  LCDWrite(LCD_COMMAND, 0x08 | 0x05); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
  LCDWrite(LCD_COMMAND, 0x20); //Set display mode  */

  /* Indirect, swap bits in displayMap option: */
  int i;
  for (i = 0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++) {
    displayMap[i] = ~displayMap[i] & 0xFF;
  }
  updateDisplay();
}

// There are two memory banks in the LCD, data/RAM and commands.
// This function sets the DC pin high or low depending, and then
// sends the data byte
void LCDWrite(byte data_or_command, byte data)
{
  //Tell the LCD that we are writing either to data or a command
  digitalWrite(dcPin, data_or_command);

  //Send the data
  digitalWrite(scePin, LOW);
  SPI.transfer(data); //shiftOut(sdinPin, sclkPin, MSBFIRST, data);
  digitalWrite(scePin, HIGH);
}

//This sends the magical commands to the PCD8544
void lcdBegin(void)
{
  //Configure control pins
  pinMode(scePin, OUTPUT);
  pinMode(rstPin, OUTPUT);
  pinMode(dcPin, OUTPUT);
  pinMode(sdinPin, OUTPUT);
  pinMode(sclkPin, OUTPUT);
  pinMode(blPin, OUTPUT);
  analogWrite(blPin, 255);

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  //Reset the LCD to a known state
  digitalWrite(rstPin, LOW);
  digitalWrite(rstPin, HIGH);

  LCDWrite(LCD_COMMAND, 0x21); //Tell LCD extended commands follow
  LCDWrite(LCD_COMMAND, 0xB0); //Set LCD Vop (Contrast)
  LCDWrite(LCD_COMMAND, 0x04); //Set Temp coefficent
  LCDWrite(LCD_COMMAND, 0x14); //LCD bias mode 1:48 (try 0x13)
  //We must send 0x20 before modifying the display control mode
  LCDWrite(LCD_COMMAND, 0x20);
  LCDWrite(LCD_COMMAND, 0x0C); //Set display control, normal mode.
}

void setChar(char character, int x, int y, int bw)
{
  byte column; // temp byte to store character's column bitmap
  for (int i=0; i<5; i++) // 5 columns (x) per character
  {
    column = ASCII[character - 0x20][i];
    for (int j=0; j<8; j++) // 8 rows (y) per character
    {
      if (column & (0x01 << j)) // test bits to set pixels
        setPixel(x+i, y+j, bw);
      else
        setPixel(x+i, y+j, !bw);
    }
  }
}

void setStr(char * dString, int x, int y, int bw)
{
  while (*dString != 0x00) // loop until null terminator
  {
    setChar(*dString++, x, y, bw);
    x+=5;
    for (int i=y; i<y+8; i++)
    {
      setPixel(x, i, !bw);
    }
    x++;
    if (x > (LCD_WIDTH - 5)) // Enables wrap around
    {
      x = 0;
      y += 8;
    }
  }
}