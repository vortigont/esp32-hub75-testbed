#pragma once

// Set of flash-strings that might be reused multiple times within the code

// General
static const char T_HEADLINE[] PROGMEM = "HUB75 testbed";    // имя проекта


// Our variable names
static const char V_MX_W[] PROGMEM = "mx_w";
static const char V_MX_H[] PROGMEM = "mx_h";
static const char V_MX_CH[] PROGMEM = "mx_chain";
static const char V_MX_DRV[] = "mx_drv";
static const char V_MX_DB[] = "mx_db";                  // double buff
static const char V_MX_ClkPh[] = "mx_cph";              // Clk phase
static const char V_MX_I2S_HZ[] = "mx_hz";              // Clk HZ
static const char V_MX_LB[] = "mx_LB";                  // Latch Blanking
static const char V_MX_RR[] = "mx_RR";                  // Min Refresh rate
static const char V_PANE_OS[] = "vp_os";	            // V-Pane order - Serpentine
static const char V_PANE_OV[] = "vp_ov";	            // V-Pane order - Vertical
static const char V_PANE_ROWS[] = "vp_r";	            // V-Pane Width
static const char V_PANE_COLS[] = "vp_c";               // V-Pane Height
static const char V_DISP_BRT[] = "brt";                 // Brightness

// UI blocks
static const char T_GFX[] PROGMEM = "t1";           // генерация UI-секции "gfx"
static const char T_MX[] PROGMEM = "t2";            // генерация UI-секции "matrix"

static const char S_SHAPE[] PROGMEM = "shape";          // GFX shape selector

// UI action handlers
static const char A_SET_MATRIX[] PROGMEM = "a_setmx";       //  Matrix settings set 
static const char A_DMA_STOP[] PROGMEM = "a_dmastop";       // Stop i2s DMA transfer
static const char A_DISP_CLEAR[] PROGMEM = "a_dcl";         // clear display
static const char A_GFX_CTRLS[] PROGMEM = "a_gfxctrls";         // clear display

// params
static const char p1[] = "p1";
static const char p2[] = "p2";
static const char p3[] = "p3";
static const char p4[] = "p4";
