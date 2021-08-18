#include "main.h"
#include "EmbUI.h"
#include "uistrings.h"   // non-localized text-strings
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>

extern MatrixPanel_I2S_DMA *dma_display;
extern std::unique_ptr<VirtualMatrixPanel> vmatrix;
extern bool run;


uint16_t time_counter = 0, cycles = 0, fps = 0;
unsigned long fps_timer;


CRGB currentColor;
CRGBPalette16 palettes[] = {HeatColors_p, LavaColors_p, RainbowColors_p, RainbowStripeColors_p, CloudColors_p};
CRGBPalette16 currentPalette = palettes[0];


CRGB ColorFromCurrentPalette(uint8_t index = 0, uint8_t brightness = 255, TBlendType blendType = LINEARBLEND) {
  return ColorFromPalette(currentPalette, index, brightness, blendType);

// us delay
// <rom/ets_sys.h>
// "ets_delay_us(value)"
}


void switchpallete(){
        int a = random(0,sizeof(palettes)/sizeof(palettes[0]));
        currentPalette = palettes[a];
        Serial.printf("Changing pattern to: %d\n", a);
}



// Sinus wibble effect
void wibble(){
   for (int x = 0; x <  vmatrix->virtualResX; ++x) {
        for (int y = 0; y <  vmatrix->virtualResY; ++y) {
            int16_t v = 0;
            uint8_t wibble = sin8(time_counter);
            v += sin16(x * wibble * 3 + time_counter);
            v += cos16(y * (128 - wibble)  + time_counter);
            v += sin16(y * x * cos8(-time_counter) / 8);

            currentColor = ColorFromPalette(currentPalette, (v >> 8) + 127); //, brightness, currentBlendType);
            vmatrix->drawPixelRGB888(x, y, currentColor.r, currentColor.g, currentColor.b);
        }
    }

  ++time_counter;
  ++cycles;
  ++fps;

  if (cycles > 1024) {
      time_counter = 0;
      cycles = 0;
      switchpallete();
  }
  if (fps_timer + 5000 < millis()){
     Serial.printf_P(PSTR("Effect fps: %d\n"), fps/5);
     fps_timer = millis();
     fps = 0;
  }

//  fps_timer = millis();
}

void run_wibble(){
  dma_display->clearScreen();
}

void stop_wibble(){
}

// Solid fill
void gfxFill(const char *str){
    if (!dma_display) return;

    uint8_t r, g, b;
    sscanf(str, "#%2hhx%2hhx%2hhx", &r, &g, &b);
    dma_display->fillScreenRGB888(r, g ,b);
}

// Draw 3 axis gradient
void gfxGradient(uint8_t r, uint8_t g, uint8_t b){
  if (!dma_display) return;

  uint8_t _r=0, _g=0, _b=0;
  uint16_t y = vmatrix->virtualResY;

  do {
    --y;
    _b = y*b/vmatrix->virtualResY;
    uint16_t x = vmatrix->virtualResX;
    do {
      --x;
        _r = x*r/vmatrix->virtualResX;
        _g = g*(x+y) / (vmatrix->virtualResX + vmatrix->virtualResX);
        vmatrix->drawPixelRGB888( x, y, _r, _g, _b);
    } while(x);
  } while(y);
}

// draw lines
void gfxLines(int16_t x, int16_t y, int16_t l, int16_t repeat, bool vh, const char *color){
  if (!dma_display) return;

  uint8_t r, g, b;
  sscanf(color, "#%2hhx%2hhx%2hhx", &r, &g, &b);

  if (vh){
      do {
          vmatrix->drawFastVLine(x, y, l, MatrixPanel_I2S_DMA::color565(r, g, b));
          x+= repeat;
      } while(repeat && x < vmatrix->virtualResX);
  } else {
      do {
          vmatrix->drawFastHLine(x, y, l, MatrixPanel_I2S_DMA::color565(r, g, b));
          y+= repeat;
      } while(repeat && y < vmatrix->virtualResY);
  }
}
