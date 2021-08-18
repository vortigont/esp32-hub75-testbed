// How to use this library with a FM6126 panel, thanks goes to:
// https://github.com/hzeller/rpi-rgb-led-matrix/issues/746

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

/* -------------------------- Display Config Initialisation -------------------- */
// Virtual Panl dimensions
#define VPANEL_W 128
#define VPANEL_H 128

//std::unique_ptr<VirtualMatrixPanel> vmatrix = nullptr;
//MatrixPanel_I2S_DMA *dma_display = nullptr;
//std::unique_ptr<MatrixPanel_I2S_DMA> dma_display = nullptr;

///////////////////////////////////////////////////////////////

bool run = false;


void setup(){

  Serial.begin(BAUD_RATE);
  Serial.println(F("Starting ESP32 HUB75 i2s DMA testbed..."));

  // Start EmbUI framework
  embui.begin();

/*
  HUB75_I2S_CFG mxconfig ={
    .mx_width = 64,
    .mx_height = 64,
    .chain_length = 4,
  };

    mxconfig.driver = HUB75_I2S_CFG::FM6126A;
    mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_20M;
    mxconfig.gpio.e = CH_E;

//  mxconfig.clkphase = true;
//  HUB75_I2S_CFG::i2s_pins _pins={R1, G1, BL1, R2, G2, BL2, CH_A, CH_B, CH_C, CH_D, CH_E, LAT, OE, CLK};
//  HUB75_I2S_CFG mxconfig(64, 64, 4, _pins, HUB75_I2S_CFG::FM6126A);
//  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_20M;

*/
/*
  HUB75_I2S_CFG mxconfig; //64, 64, 4);
  mxconfig.mx_width = 64;
  mxconfig.mx_height = 64;
  mxconfig.chain_length = 4;
  mxconfig.driver = HUB75_I2S_CFG::FM6126A;
  mxconfig.fastmode = true;
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_20M;
  mxconfig.gpio.e = CH_E;
*/
  //dma_display = std::unique_ptr<MatrixPanel_I2S_DMA>(new MatrixPanel_I2S_DMA(mxconfig));

/*
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  dma_display->begin();
  dma_display->setBrightness8(255);
  //dma_display->setLatBlanking(8);

  vmatrix = std::unique_ptr<VirtualMatrixPanel>(new VirtualMatrixPanel((*dma_display), NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y, embui.param(FPSTR(V_MX_OS)).toInt(), embui.param(FPSTR(V_MX_OV)).toInt()) );
*/
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
