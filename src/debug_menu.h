#ifndef DEBUG_MENU_H
#define DEBUG_MENU_H

#include "ili9341.h"
#include <stdio.h>
typedef struct DebugBox{
    char* text;
    uint16_t x0;
    uint16_t y0;
    uint16_t width;
    uint16_t height;
}DebugBox_t;

void draw_debug_button(void);
void draw_pin_state_box(void);
void draw_state_buttons_debug_mode(void);
font_t *font_setup_14x24();


#endif 