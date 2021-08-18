#include "main.h"
#include "EmbUI.h"
#include "uistrings.h"   // non-localized text-strings
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Display obejcts
std::unique_ptr<VirtualMatrixPanel> vmatrix = nullptr;
MatrixPanel_I2S_DMA *dma_display = nullptr;

extern bool run;



bool dispStart(){
  run = false;

  HUB75_I2S_CFG mxconfig ={
    .mx_width = embui.paramVariant(FPSTR(V_MX_W)),
    .mx_height = embui.paramVariant(FPSTR(V_MX_H)),
    .chain_length = embui.paramVariant(FPSTR(V_MX_CH))
  };

    mxconfig.driver = (HUB75_I2S_CFG::shift_driver)embui.paramVariant(FPSTR(V_MX_DRV)).as<u8_t>();
    LOG(print, F("Driver: "));
    LOG(println, mxconfig.driver);

    if (embui.paramVariant(FPSTR(V_MX_I2S_HZ)))
      mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_20M;

    mxconfig.double_buff = embui.paramVariant(FPSTR(V_MX_DB));
    mxconfig.latch_blanking = embui.paramVariant(FPSTR(V_MX_LB));
    mxconfig.clkphase = embui.paramVariant(FPSTR(V_MX_ClkPh));
    mxconfig.min_refresh_rate = embui.paramVariant(FPSTR(V_MX_RR));

  // Fix this!
  mxconfig.gpio.e = CH_E;

  dispStop();

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  if (dma_display->begin()){
    LOG(println, F("Display created OK!"));

//  dbuffer leads to crash due to delay() in showDMABuffer()
    dma_display->setBrightness8(255);
    dma_display->fillScreenRGB888(255,64,64);

    vmatrix = std::unique_ptr<VirtualMatrixPanel>(new VirtualMatrixPanel( (*dma_display),
      embui.paramVariant(FPSTR(V_PANE_ROWS)), embui.paramVariant(FPSTR(V_PANE_COLS)),
      embui.paramVariant(FPSTR(V_MX_W)), // Module width
      embui.paramVariant(FPSTR(V_MX_H)), // Module height
      embui.paramVariant(FPSTR(V_PANE_OS)),
      embui.paramVariant(FPSTR(V_PANE_OV)))
    );

    LOG(printf_P, PSTR("vPane X:Y %u:%u\n"), vmatrix->virtualResX, vmatrix->virtualResY);

//    run = true;
    return true;

  } else {
    Serial.println(F("Display create error..."));
    return false;
  }

}


/**
 * Stop DMA transfer and Destroy dispaly obj
 */
void dispStop(){
  run = false;

  if (dma_display){
    vmatrix = nullptr;
    delete dma_display;
    dma_display = nullptr;
  }
}

uint8_t dispBrt(){
    // not yet implemented
//    dma_display ? dma_display->
    return 255;
};

void dispBrt(const uint8_t b){
  if (dma_display)
    dma_display->setBrightness8(b);

  LOG(print, F("Set brightness: "));
  LOG(println, b);
}

void dispClear(){
  if (dma_display)
    dma_display->clearScreen();

  LOG(println, F("Clear screen"));
}
