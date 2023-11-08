#ifndef DISPLAY_CONTROL__H
#define DISPLAY_CONTROL__H

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdio.h>
#include "definitions.h"
#include "ili9341.h"
#include "ili9341.h"                    // SYS function prototypes
#include "fonts.h"
#include "display_control.h"
#include "debug_menu.h"


typedef struct Button{
    uint16_t x0;
    uint16_t y0;
    uint16_t width;
    uint16_t height;
}Button_t;


#define START       1
#define STOP        2
#define FORWARD     3
#define BACKWARDS   4
#define DEBUG       5
#define BACK        6


void GPIO_RB15_IntHandler(GPIO_PIN pin, uintptr_t context);
void draw_state_led(void);
void draw_manual_drive_buttons(void);
void stop_button_on(void);
void start_button_on(void);
void stop_led_off();
void start_led_off(); 
void stop_led_on();
void start_led_on();
font_t *font_setup_20x38();
void calculateCoeffs(float* coeff, uint8_t* xdyd, uint16_t* xtyt);
void readXY(uint16_t* x, uint16_t* y);
void sort(uint16_t* arr, int n);
void draw_buttons(void);
uint8_t check_button_hitbox(uint16_t* x, uint16_t* y, float* coeff, bool debug_mode);
float* display_init_coef(float* coeff);
void draw_main_screen(void);

#endif