#include "buttons.h"

void draw_button(Button_t *button){
    ili9341_draw_filled_rectangle(button->x0,
                                  button->y0,
                                  button->width,
                                  button->height,
                                  button->color);
}