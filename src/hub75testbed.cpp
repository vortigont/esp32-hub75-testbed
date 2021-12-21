#include <Arduino.h>
#include "main.h"
#include <FastLED.h>
#include "EmbUI.h"
#include "uistrings.h"

#ifdef USE_FTP
 #include "ftpSrv.h"
#endif

////////////////////////////////////////////////////////////////////
// Reset Panel
// FM6126 support is still experimental
//
// pinout for ESP38 38pin module
// http://arduinoinfo.mywikis.net/wiki/Esp32#KS0413_keyestudio_ESP32_Core_Board
//

// HUB75E pinout
// R1 | G1
// B1 | GND
// R2 | G2
// B2 | E
//  A | B
//  C | D
// CLK| LAT
// OE | GND

#define R1 25
#define G1 26
#define BL1 27
#define R2 14 // 21 SDA
#define G2 12 // 22 SDL
#define BL2 13
#define CH_A 23
#define CH_B 19
#define CH_C 5
#define CH_D 17
#define CH_E 18 // assign to any available pin if using two panels or 64x64 panels with 1/32 scan
                // shield
#define LAT 4
#define OE 15
#define CLK 16

// End of default setup for RGB Matrix 64x32 panel


///////////////////////////////////////////////////////////////

bool run = false;


void setup(){

  Serial.begin(BAUD_RATE);
  Serial.println(F("Starting ESP32 HUB75 i2s DMA testbed..."));

  // Start EmbUI framework
  embui.begin();


  #ifdef USE_FTP
      ftp_setup(); // запуск ftp-сервера
  #endif
}

void loop(){
  embui.handle();


#ifdef USE_FTP
    ftp_loop(); // цикл обработки событий фтп-сервера
#endif

 if (run)
  wibble();

}
