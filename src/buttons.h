#ifndef BUTTONS_H
#define BUTTONS_H

#include "ili9341.h"

typedef struct Button{
    uint16_t x0;
    uint16_t y0;
    uint16_t width;
    uint16_t height;
    uint16_t color;
    bool btn_state;
}Button_t;

void draw_button(Button_t button);

#endif