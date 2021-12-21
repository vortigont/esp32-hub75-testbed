#include "main.h"

#include "EmbUI.h"
#include "interface.h"

#include "uistrings.h"   // non-localized text-strings

/**
 * можно нарисовать свой собственный интефейс/обработчики с нуля, либо
 * подключить статический класс с готовыми формами для базовых системных натсроек и дополнить интерфейс.
 * необходимо помнить что существуют системные переменные конфигурации с зарезервированными именами.
 * Список имен системных переменных можно найти в файле "constants.h"
 */
#include "basicui.h"

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Display obejcts
//std::unique_ptr<VirtualMatrixPanel> vmatrix = nullptr;
extern MatrixPanel_I2S_DMA *dma_display;

/**
 * переопределяем метод из фреймворка, регистрирующий необходимы нам в проекте переменные и методы обработки
 * 
 */
void create_parameters(){
    LOG(println, F("UI: Creating application vars"));

   /**
    * регистрируем статические секции для web-интерфейса с системными настройками,
    * сюда входит:
    *  - WiFi-manager
    *  - установка часового пояса/правил перехода сезонного времени
    *  - установка текущей даты/времени вручную
    *  - базовые настройки MQTT
    *  - OTA обновление прошивки и образа файловой системы
    */
    basicui::add_sections();

    /**
     * регистрируем свои переменные
     */
    embui.var_create(FPSTR(V_MX_W), DEFAULT_MX_W);    // Module width
    embui.var_create(FPSTR(V_MX_H), DEFAULT_MX_H);    // Module height
    embui.var_create(FPSTR(V_MX_CH), 1);              // chain
    embui.var_create(FPSTR(V_MX_DRV), 0);              // Driver chip
    embui.var_create(FPSTR(V_MX_DB), 0);              // double buff
    embui.var_create(FPSTR(V_MX_I2S_HZ), 0);             // Clk rate
    embui.var_create(FPSTR(V_MX_ClkPh), 1);           // Clk phase
    embui.var_create(FPSTR(V_MX_LB), 1);              // Latch Blanking
    embui.var_create(FPSTR(V_MX_RR), 100);            // Min Refresh rate
    embui.var_create(FPSTR(V_PANE_OS), 0);	          // vPanel Serpentine
    embui.var_create(FPSTR(V_PANE_OV), 0);	          // vPanel Vertical
    embui.var_create(FPSTR(V_PANE_ROWS), 1);	          // vPanel Serpentine
    embui.var_create(FPSTR(V_PANE_COLS), 1);	          // vPanel Vertical

    /**
     * добавляем свои обрабочки на вывод UI-секций
     * и действий над данными
     */
    embui.section_handle_add(FPSTR(T_MX), block_hub75setup);                // generate "HUB75" setup UI section
    embui.section_handle_add(FPSTR(T_GFX), block_gfx);                      // generate "GFX control" UI section
//    embui.section_handle_add(FPSTR(S_SHAPE), block_gfxctrls);               // gfx patterns controls

    // action handlers
    embui.section_handle_add(FPSTR(A_SET_MATRIX), set_matrix);          // обработка данных из секции "Demo"
    embui.section_handle_add(FPSTR(A_DMA_STOP), engineShutdown);
    embui.section_handle_add(FPSTR(A_DISP_CLEAR), aClear);              // clean screen
    embui.section_handle_add(FPSTR(A_GFX_CTRLS), aGFXctrls);
    // регуляторы
    embui.section_handle_add(FPSTR(V_DISP_BRT), ui_setbrt);           // регулятор яркости

    embui.section_handle_add(b1, a_b1);           // регулятор яркости
    embui.section_handle_add(b2, a_b2);           // регулятор яркости

    //embui.section_handle_add(F("swpal"), sw_pallete);     // смена палитры
    //embui.section_handle_add(F("ptrn"), sw_pattern);      // action - change patters
};

/**
 * Headlile section
 * this is an overriden weak method that builds our WebUI interface from the top
 * ==
 * Головная секция,
 * переопределенный метод фреймфорка, который начинает строить корень нашего Web-интерфейса
 * 
 */
void section_main_frame(Interface *interf, JsonObject *data){
  if (!interf) return;

  interf->json_frame_interface(FPSTR(T_HEADLINE));  // HEADLINE for EmbUI project page

  block_menu(interf, data);                         // Строим UI блок с меню выбора других секций
  interf->json_frame_flush();

  if(!embui.sysData.wifi_sta){                      // если контроллер не подключен к внешней AP, сразу открываем вкладку с настройками WiFi
    LOG(println, F("UI: Opening network setup section"));
    basicui::block_settings_netw(interf, data);
  } else {
    block_hub75setup(interf, data);                   // Строим блок с demo переключателями
  }
};


/**
 * This code builds UI section with menu block on the left
 * 
 */
void block_menu(Interface *interf, JsonObject *data){
    if (!interf) return;
    // создаем меню
    interf->json_section_menu();

    /**
     * пункт меню - "демо"
     */
    interf->option(FPSTR(T_MX), F("HUB75 Engine"));

    interf->option(FPSTR(T_GFX), F("GFX patterns"));

    /**
     * добавляем в меню пункт - настройки,
     * это автоматически даст доступ ко всем связанным секциям с интерфейсом для системных настроек
     * 
     */
    basicui::opt_setup(interf, data);       // пункт меню "настройки"
    interf->json_section_end();
}

/**
 * HUB75 Matrix setting page
 * 
 */
void block_hub75setup(Interface *interf, JsonObject *data){
    if (!interf) return;
    interf->json_frame_interface();

    interf->json_section_main(FPSTR(A_SET_MATRIX), F("HUB75 I2S DMA engine setup"));

    interf->comment(F("I2S DMA engine does not autostart to prevent boot loops. You need to run it manually each time after restart."));

    interf->spacer(F("DMA engine setup"));

    interf->json_section_line();

    // driver chip selector
    interf->select(FPSTR(V_MX_DRV), F("Driver chip"));
    interf->option(0, F("Generic"));
    interf->option(1, F("FM6124(A)"));
    interf->option(3, F("ICN2038S"));
    interf->option(4, F("MBI5124"));
    interf->json_section_end();

    // MX Width
//    interf->range(FPSTR(V_MX_W), 32, 128, 32, F("Matrix Width, px"));
    interf->select(FPSTR(V_MX_W), F("Matrix Width"));
    interf->option(32, F("32 px"));
    interf->option(64, F("64 px"));
    interf->option(128, F("128 px"));
    interf->json_section_end();


    // MX H
    interf->select(FPSTR(V_MX_H), F("Matrix Height"));
    interf->option(32, F("32 - 1/16 scan"));
    interf->option(64, F("64 - 1/32 scan"));
    interf->json_section_end();

    interf->json_section_end();   // Start of line 2

    // Line 3
    interf->json_section_line();
    interf->number(FPSTR(V_MX_CH), F("Chain length"), 1, 1, 10);
    interf->number(FPSTR(V_MX_RR), F("Min Refresh rate, HZ"), 10, 50, 200);
    interf->select(FPSTR(V_MX_I2S_HZ), F("I2S Clk rate"));
    interf->option(0, F("10 MHz"));
    interf->option(1, F("20 MHz"));
    interf->json_section_end();
    interf->json_section_end();   // End of line 2

    // check boxes
    interf->json_section_line();

    interf->checkbox(FPSTR(V_MX_DB), F("Double Buff"));
    interf->checkbox(FPSTR(V_MX_ClkPh), F("Invert CLK phase"));
    interf->range(FPSTR(V_MX_LB), 1, 4, 1, F("Latch blanking, clk"));

    interf->json_section_end();   // End of line 3

    interf->spacer(F("Virtual display setup"));

    interf->json_section_line();
    interf->number(FPSTR(V_PANE_ROWS), F("Pane rows"), 1, 1, 10);
    interf->number(FPSTR(V_PANE_COLS), F("Pane cols"), 1, 1, 10);
    interf->checkbox(FPSTR(V_PANE_OS), F("Serpentine"));	// Serpentine/Zig-zag sequence
    interf->checkbox(FPSTR(V_PANE_OV), F("Topdown"));	    // Vertical sequence
    interf->json_section_end();     // end of line

/*
    interf->button_submit(FPSTR(A_SET_MATRIX), FPSTR(T_DICT[lang][TD::D_SAVE]), "blue");
    interf->select(F("ptrn"), "0", F("Display Pattern"), true, false);
    interf->option("0", F("Sinus wibble"));
    interf->option("1", F("Red"));
    interf->option("2", F("Green"));
    interf->option("3", F("Blue"));
    interf->option("4", F("White"));
    interf->option("5", F("Black"));

    interf->button_submit(F("swpal"), F("Switch pallete"), FPSTR(P_GRAY));

*/


    /*  кнопка отправки данных секции на обработку
     *  первый параметр FPSTR(T_DEMO) определяет какая секция откроется
     *  после обработки отправленных данных
     */ 
    interf->button_submit(FPSTR(A_SET_MATRIX), F("Re/Start I2S DMA"), F("green"));
    interf->button(FPSTR(A_DMA_STOP), F("Stop I2S DMA"), F("red"));
    interf->json_section_end();
    interf->json_frame_flush();
}


/**
 * GFX page
 * 
 */
void block_gfx(Interface *interf, JsonObject *data){
  if (!interf) return;
  interf->json_frame_interface();

  interf->json_section_main(F("gfx"), F("Test board"));

  // brt control
  interf->range(FPSTR(V_DISP_BRT), 0, 255, 1, F("Brightness"), true);
  interf->button(FPSTR(A_DISP_CLEAR), F("Clear Display"));

  interf->spacer(F("Draw controls"));

  // GFX selector
  interf->select(FPSTR(T_GFX), (*data)[FPSTR(T_GFX)].as<uint>(), F("GFX shapes"), true);
  interf->option(0, F("Solid fill"));
  interf->option(1, F("Gradient"));
  interf->option(2, F("Lines"));
  interf->option(3, F("Animation"));
  interf->json_section_end();


  interf->json_section_begin(FPSTR(A_GFX_CTRLS));
  switch ( (*data)[FPSTR(T_GFX)].as<uint>() ){
    case 0: {   // Solid fill
      interf->color(FPSTR(P_color), F("Fill color"));
      break;
    }
    case 1: {   // Gradients
      interf->comment(F("3 Color gradient: R => X axis, G => Y+X axis, B => Y axis"));
      interf->range(p1, 255, 0, 255, 1, F("R max"), false);
      interf->range(p2, 255, 0, 255, 1, F("G max"), false);
      interf->range(p3, 255, 0, 255, 1, F("B max"), false);

      interf->button(b1, F("Flip buff"));
      interf->button(b2, F("Show buff"));
      break;
    }
    case 2: {   // Lines
      interf->comment(F("Draw straight lines"));

      interf->json_section_line();
      interf->color(FPSTR(P_color), F("Line color"));
      interf->checkbox("vh", F("H/V"));	// Horizontal/vertical
      interf->number(p4, F("repeat each n px"), 1, 0, 1024);
      interf->json_section_end();   // End of line

      interf->json_section_line();
      interf->number(p1, F("X coord"), 1, -5, 1024);
      interf->number(p2, F("Y coord"), 1, -5, 1024);
      interf->number(p3, F("length"), 1, 1, 1024);
      interf->json_section_end();   // End of line

      break;
    }
    case 3: {
      interf->spacer(F("Spacer 3"));
      break;
    }
    default: {
      interf->comment("Just a comment");
    }
  }
  interf->hidden("ctrl", (*data)[FPSTR(T_GFX)].as<String>());
  interf->button_submit(FPSTR(A_GFX_CTRLS), F("Draw"));


  interf->json_frame_flush();
}


/**
 * обработчик статуса (периодического опроса контроллера веб-приложением)
 */
void pubCallback(Interface *interf){
    basicui::embuistatus(interf);
}

/**
 * action - brightness control
 */
void ui_setbrt(Interface *interf, JsonObject *data){
  if (!data) return;

  SETPARAM(FPSTR(V_DISP_BRT));
  dispBrt((*data)[FPSTR(V_DISP_BRT)]);
}

// random pattern for wibbler
void sw_pallete(Interface *interf, JsonObject *data){
  if (!data) return;
  switchpallete();
}

/**
 * Create and start I2S DMA Display pane
 */
void set_matrix(Interface *interf, JsonObject *data){
    if (!data) return;

  // save MX data to config
  SETPARAM(FPSTR(V_MX_DRV));
  SETPARAM(FPSTR(V_MX_W));
  SETPARAM(FPSTR(V_MX_H));
  SETPARAM(FPSTR(V_MX_CH));
  SETPARAM(FPSTR(V_MX_DB));
  SETPARAM(FPSTR(V_MX_I2S_HZ));
  SETPARAM(FPSTR(V_MX_ClkPh));
  SETPARAM(FPSTR(V_MX_LB));
  SETPARAM(FPSTR(V_MX_RR));
  SETPARAM(FPSTR(V_PANE_ROWS));
  SETPARAM(FPSTR(V_PANE_COLS));
  SETPARAM(FPSTR(V_PANE_OS));
  SETPARAM(FPSTR(V_PANE_OV));

  // Re/Start the engine
  dispStart();
}

void engineShutdown(Interface *interf, JsonObject *data){
  dispStop();
}

// gfx  - clear screen
inline void aClear(Interface *interf, JsonObject *data){
  dispClear();
}

// UI gfx controls handler
void aGFXctrls(Interface *interf, JsonObject *data){
  if (!interf || !data) return;

  LOG(print, F("Ctrl: "));
  LOG(println, (*data)["ctrl"].as<uint8_t>());

  switch ( (*data)["ctrl"].as<uint8_t>() ){
    case 0: {   // Solid fill
      if ((*data)[FPSTR(P_color)]){
        LOG(print, F("Fill color: "));
        LOG(println, (*data)[FPSTR(P_color)].as<const char*>());

        gfxFill((*data)[FPSTR(P_color)].as<const char*>());
      }
      break;
    }
    case 1: {   // Gradients
        LOG(println, F("Draw Gradient"));
        gfxGradient((*data)[p1].as<uint8_t>(), (*data)[p2].as<uint8_t>(), (*data)[p3].as<uint8_t>());
      break;
    }
    case 2: {   // Lines
        gfxLines((*data)[p1].as<int16_t>(), (*data)[p2].as<int16_t>(), (*data)[p3].as<int16_t>(), (*data)[p4].as<int16_t>(), (*data)["vh"], (*data)[FPSTR(P_color)].as<const char*>());
      break;
    }
    default: {
    }
  }

}


void a_b1(Interface *interf, JsonObject *data){
//  dispFlip();
  dma_display->flipDMABuffer();
}

void a_b2(Interface *interf, JsonObject *data){
//  dispShow();
//  dma_display->showDMABuffer();
}
