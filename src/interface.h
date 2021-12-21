#pragma once

// UI pages
void block_menu(Interface *interf, JsonObject *data);
void block_hub75setup(Interface *interf, JsonObject *data);
void block_gfx(Interface *interf, JsonObject *data);

// UI actions
void set_matrix(Interface *interf, JsonObject *data);
void engineShutdown(Interface *interf, JsonObject *data);
// action - clear screen
void aClear(Interface *interf, JsonObject *data);
void aGFXctrls(Interface *interf, JsonObject *data);

void pubCallback(Interface *interf);
void ui_setbrt(Interface *interf, JsonObject *data);
void sw_pallete(Interface *interf, JsonObject *data);
void sw_pattern(Interface *interf, JsonObject *data);

void a_b1(Interface *interf, JsonObject *data);
void a_b2(Interface *interf, JsonObject *data);
//void remote_action(RA action, ...);
//uint8_t uploadProgress(size_t len, size_t total);
