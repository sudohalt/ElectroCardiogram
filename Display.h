/* PCD8544-specific defines: */
#define LCD_COMMAND  0
#define LCD_DATA     1
#define PIN_SCE   7
#define PIN_RESET 6
#define PIN_DC    5
#define PIN_SDIN  4
#define PIN_SCLK  3

/* 84x48 LCD Defines: */
#define LCD_WIDTH   84 // Note: x-coordinates go wide
#define LCD_HEIGHT  48 // Note: y-coordinates go high
#define WHITE       0  // For drawing pixels. A 0 draws white.
#define BLACK       1  // A 1 draws black.
#define HIGH  1
#define LOW  0

typedef unsigned char byte;

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
void drawDigit(int d1, int d2, int d3, int x, int y, int bw);

/* Function: updateDisplay
 * ---------
 * Updates the display with the contents of the displayMap array
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   void
 */
void updateDisplay();

/* Function: clearDisplay
 * ---------
 * Clears the contens of the display, to either all black or all white
 *
 * Parameters:
 *   bool bw: True for black, false for white
 *
 * Returns:
 *   void
 */
void clearDisplay(int bw);

/* Function: setContrast
 * ---------
 * Sets the contrast of the Nokia 5110 LCD
 *
 * Parameters:
 *   byte contrast: Contrast value 0-255, the higher the darker
 *
 * Returns:
 *   void
 */
void setContrast(byte contrast);


/* Function: lcdBegin
 * ---------
 * Initializes the Nokia 5110 display for SPI communication
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   void
 */
void lcdBegin();

/* Function: setPixel

*/
void setPixel(int x, int y, int bw);

/* Dispaly small strings in parallel with wave form */
void setStr(char * dString, int x, int y, int bw);
