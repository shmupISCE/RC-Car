#ifndef DISPLAY_CONTROL__H
#define DISPLAY_CONTROL__H

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"
#include "ili9341.h"
#include "ili9341.h"                    // SYS function prototypes
#include "fonts.h"
#include "display_control.h"
#include <stdio.h>

void GPIO_RB15_IntHandler(GPIO_PIN pin, uintptr_t context);
void draw_state_led(void);
void draw_manual_drive_buttons(void);
void stop_button_on(void);
void start_button_on(void);
void stop_led_off();
void start_led_off(); 
void stop_led_on();
void start_led_on();
font_t *font_setup();
void calculateCoeffs(float* coeff, uint8_t* xdyd, uint16_t* xtyt);
void readXY(uint16_t* x, uint16_t* y);
void sort(uint16_t* arr, int n);
void draw_buttons(void);
uint8_t check_button_hitbox(uint16_t* x, uint16_t* y, float* coeff);
//float* display_init_coef(float* coeff);

#endif