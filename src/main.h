
#include "globals.h"
#define BAUD_RATE       115200  // serial debug port baud rate

//#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>


#ifndef DEFAULT_MX_W
#define DEFAULT_MX_W    64
#endif
#ifndef DEFAULT_MX_H
#define DEFAULT_MX_H    32
#endif


// display declarations
bool dispStart();
void dispStop();
uint8_t dispBrt();
void dispBrt(const uint8_t b);
void dispClear();

// gfx funcs
void gfxFill(const char *str);
void gfxGradient(uint8_t r, uint8_t g, uint8_t b);
void gfxLines(int16_t x, int16_t y, int16_t l, int16_t repeat, bool vh, const char *color);

void switchpallete();
void wibble();          // some weird sinus patter
void run_wibble();
void stop_wibble();