
/*

  This stuff could all be put into a library, but we wanted to
  leave it all in one sketch to keep it as transparent as possible.

  Hardware: (Note most of these pins can be swapped)
    Graphic LCD Pin ---------- Arduino Pin
       1-VCC       ----------------  5V
       2-GND       ----------------  GND
       3-SCE       ----------------  7
       4-RST       ----------------  6
       5-D/C       ----------------  5
       6-DN(MOSI)  ----------------  11
       7-SCLK      ----------------  13
       8-LED       - 330 Ohm res --  9
   The SCLK, DN(MOSI), must remain where they are, but the other
   pins can be swapped. The LED pin should remain a PWM-capable
   pin. Don't forget to stick a current-limiting resistor in line
   between the LCD's LED pin and Arduino pin 9!
*/

#include "Display.h"
#define swap(a, b) { int16_t t = a; a = b; b = t; }

void setup()
{
  digitalWrite(6, HIGH);
  Serial.begin(9600);

  lcdBegin(); // This will setup our pins, and initialize the LCD
  setContrast(55); // Pretty good value, play around with it

  updateDisplay(); // with displayMap untouched, SFE logo
  delay(2000);

  //lcdFunTime(); // Runs a 30-second demo of graphics function;
}


char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

// Bresenham's algorithm - thx wikpedia
void drawLine(int16_t x0, int16_t y0,
			    int16_t x1, int16_t y1,
			    int color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
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

  for (; x0<=x1; x0++) {
    if (steep) {
      setPixel(y0, x0, color);
    } else {
      setPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void draw_wave(int wave[], int heart_rate)
{
  int i, x, avg, prev, maxs;
  prev = 0;
  int even = 0;
  int prevavg = 0;
  for (i = 0, x = 0; x < 84; i++, x++) {
    //int maxs = wave[i] ? wave[i] > prev : prev;
    //int waveval = wave[i]/2 - 5 ? wave[i]/2 > 5 : wave[i]/2;
    if (wave[i] > 0) {
        avg = wave[i];
        setPixel(x, 30 - wave[i], 1);
        /*
        if (x != 0) drawLine(x,36 -avg, x-2, 36 -prevavg, 1);
        if (prevavg > 0) {
          if (x != 0) drawLine(x, 30 - avg, x - 2, 30 - prevavg, 1);
          else drawLine(x,  30 - avg,x, 30 - prevavg, 1);
        } else {
          if (x != 0) drawLine(x, 30 - avg, x - 2,30 + prevavg, 1);
          else drawLine(x,  30 - avg,x, 30 + prevavg, 1);        
        }
        */
    } else {
        avg = abs(wave[i]);
        setPixel(x, 30 + abs(wave[i]), 1);
        /*
        if (prevavg > 0) {
          if (x != 0) drawLine(x, 30 + avg, x - 2, 30 - prevavg, 1);
          else drawLine(x,  30 + avg,x, 30 - prevavg, 1);
        } else {
          if (x != 0) drawLine(x, 30 + avg, x - 2,30 +prevavg, 1);
          else drawLine(x,  30 + avg,x, 30 + prevavg, 1);        
        }
        */
    }
    prevavg = abs(wave[i]);
    char buf[5];
    if (heart_rate > 20 && heart_rate < 110)
      sprintf(buf, "%d", heart_rate);
    else if (heart_rate > 110 || heart_rate < 12) 
      sprintf(buf, "NN");
    else 
      sprintf(buf, "%d", 0);
    setStr(buf, 1, 1, 1);
    updateDisplay();
  }
}

// Loop turns the display into a local serial monitor echo.
// Type to the Arduino from the serial monitor, and it'll echo
// what you type on the display. Type ~ to clear the display.
void loop()
{
  clearDisplay(0);
  static int cursorX = 0;
  static int cursorY = 24;
  static int currentDigit = 0;
  int i;
  int d1;
  int d2;
  int d3; 
  float avg = 0.0;
  int cnt = 0;
  int comp_cnt = 0;
  int impulse;
  int gatherData = 5000;
  int read_comp = 0;
  int wave_form[168];
  int wave_dummy0[] = {
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0,0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };
  int wave_dummy1[] = {
    0,  0,  0,  0,
    5, 10, 15, 20, 15, 10, 5, 0, -5, -6, 0, 0, 
   -5, 0,   0,  0,  0,  0, 0, 0,  0,  0,
   5, 10, 15, 20, 15, 10, 5, 0, -5, -6, 0, 0, 
       0,  0,  0,  0,  0,  0, 0, 0,  0,   0, 0, 0,
    5, 10, 15, 20, 15, 10, 5, 0, -5, -6, 0, 0,
   -5, 0,   0,  0,  0,  0, 0, 0,  0,  0,0,  
   5, 10, 15, 20, 15, 10, 5, 0, -5, -6, 0
  };
  int wave_dummy2[] = {
         5, 10, 15, 18, 20, 18, 15, 10, 5, 0, -5, -6, -5, 0,
     5, 10, 15, 18, 20, 18, 15, 10, 5, 0, -5, -6, -5, 0, 
     5, 10, 15, 18, 20, 18, 15, 10, 5, 0, -5, -6, -5, 0, 
     5, 10, 15, 18, 20, 18, 15, 10, 5, 0, -5, -6, -5, 0, 
     5, 10, 15,18, 20, 18,15, 10, 5, 0, -5, -6, -5, 0, 
     5, 10, 15, 18, 20, 18, 15, 10, 5, 0, -5, -6, -5, 0
  };
   int wave_dummy3[] = {
     5, 10, 15, 20, 15, 10, 5, 0, -5, -6, -5, 0,
     5, 10, 15, 20, 15, 10, 5, 0, -5, -6, -5, 0, 
     5, 10, 15, 20, 15, 10, 5, 0, -5, -6, -5, 0, 
     5, 10, 15, 20, 15, 10, 5, 0, -5, -6, -5, 0, 
     5, 10, 15, 20, 15, 10, 5, 0, -5, -6, -5, 0, 
     5, 10, 15, 20, 15, 10, 5, 0, -5, -6, -5, 0, 
     5, 10, 15, 20, 15, 10, 5, 0, -5, -6, -5, 0
  };
  int comp_hi = 0;
  int prev_comp = LOW;
  
  for(i = 0; i < 168; i++) {
      wave_form[i] = analogRead(A1);
      if (analogRead(A0) >= 300 && prev_comp != HIGH) {
        comp_hi += 1;
        prev_comp = HIGH;
      } else if (analogRead(A0) >= 300) {
        prev_comp = HIGH;
      } else {
        prev_comp = LOW;
      }
      delay(30);
  }
  int heart_rate = comp_hi * 12;
  if (heart_rate < 30) {
   draw_wave(wave_dummy0, heart_rate);
  }
  if (heart_rate < 70 && heart_rate > 30) {
    draw_wave(wave_dummy1, heart_rate);
  } else if (heart_rate > 70 && heart_rate < 100) {
    draw_wave(wave_dummy1, heart_rate);
  } else if (heart_rate > 100) {
    draw_wave(wave_dummy3, heart_rate);
  }
  //delay(3000);
//  while (gatherData && 0) {
//    read_comp = digitalRead(26); 
//    if (read_comp && !prev_comp)    
//      comp_cnt++;
//    }
//    gatherData--;
//    delay(1);
//  } 
//  clearDisplay();
//  currentDigit = comp_cnt;
//  d1 = currentDigit/100;
//  d2 = currentDigit % 100 / 10;
//  d3 = currentDigit % 10;
//  drawDigit(d1, d2, d3, 10, 10, true);
//  updateDisplay();
}
