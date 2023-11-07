#include "debug_menu.h"
#include "display_control.h"


void draw_debug_button(void){
    ili9341_draw_filled_rectangle(30, 5, 70, 35, COLOR_LIGHTGREY);
}

font_t *font_setup_14x24(){
    font_t* test_font = (font_t*)malloc(sizeof(font_t)); 

    test_font->height = 24;
    test_font->width = 14;
    test_font->font = font_lucida_sans_14x24;
    return test_font;
}

void draw_pin_state_box(void){
    font_t *font = font_setup_14x24();
    char *pwm_pin = "PWM_PIN:";
    char *dir_pin = "DIR_PIN:";
    char *enbl_pin = "EN_PIN:";
    
    bool pwm_read = GPIO_PinRead(GPIO_PIN_RD0);
    bool dir_read = GPIO_PinRead(GPIO_PIN_RB8);
    bool en_read = GPIO_PinRead(GPIO_PIN_RC1);
    
    
    char* pwm_val = (pwm_read == true) ? "1" : "0";
    char* dir_val = (dir_read == true) ? "1" : "0";
    char* en_val = (en_read == true) ? "1" : "0";
    
    ili9341_write_text(11, 15, pwm_pin, COLOR_BLACK, font);
    ili9341_write_text(125, 15, pwm_val, COLOR_BLACK, font);
    
    ili9341_draw_hline(2, 13, 140, COLOR_BLACK, 2);
    
    ili9341_write_text(11, 40, dir_pin, COLOR_BLACK, font);
    ili9341_write_text(125, 40, dir_val, COLOR_BLACK, font);
    
    ili9341_draw_hline(2, 90, 140, COLOR_BLACK, 2);
    ili9341_draw_vline(140, 13, 79, COLOR_BLACK, 2);
    ili9341_draw_vline(2, 13, 79, COLOR_BLACK, 2);
    
    ili9341_write_text(11, 65, enbl_pin, COLOR_BLACK, font);
    ili9341_write_text(120, 65, en_val, COLOR_BLACK, font);
    
    
}

void draw_state_buttons_debug_mode(void){
    char* start_button  = "START";
    char* stop_button  = "STOP";
    char* back_button  = "BACK";

    font_t *test_font = font_setup_14x24();
    
    ili9341_draw_filled_rectangle(240, 0, 80, 80, COLOR_CYAN);
    ili9341_write_text(250, 30, back_button, COLOR_BLACK, test_font);

    // STOP BUTTON
    ili9341_draw_filled_rectangle(240, 80, 80, 80, COLOR_RED);
    ili9341_write_text(250, 120, stop_button, COLOR_BLACK, test_font);
    
    //Back 
    ili9341_draw_filled_rectangle(240, 160, 80, 80, COLOR_GREEN);
    ili9341_write_text(250, 190, start_button, COLOR_BLACK, test_font);
    
    ili9341_draw_vline(239, 0, 240, COLOR_BLACK, 2);
}


unsigned int GetSystemClockFrequency() {
    unsigned int syscfg;
    unsigned long long frequency;

    syscfg = OSCCON;  // Read the oscillator control register

    // Determine the system clock source and perform the necessary calculations
    if ((syscfg & 0x1800) == 0x1800) {
        frequency = 120000000;  // 120 MHz
    } else if ((syscfg & 0x1800) == 0x1000) {
        frequency = 100000000;  // 100 MHz
    } else {
        // Other cases not handled here; you may need to adjust accordingly
        frequency = 0;
    }

    return (unsigned int)frequency;
}