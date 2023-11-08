#include <stddef.h>
#include <stdbool.h>
#include "definitions.h"
#include "display_control.h"
#include "motor_control.h"
#include "debug_menu.h"


//Git test
bool DEBUG_MODE = false;
bool motor_enabled = false;
bool start_btn_flag = false;    //Check state of Start button

int main(void) {
    /* Initialize all modules */
    SYS_Initialize(NULL);

    float coef[6];
    uint16_t x[1];
    uint16_t y[1];
    uint8_t button_pressed = 2;
    
    display_init_coef(coef);
    motor_init();
    draw_main_screen();

    while (true) {
        readXY(x, y);
        button_pressed = check_button_hitbox(x, y, coef, DEBUG_MODE);

        switch (button_pressed) {
            case START: // START button is pressed
                motor_enabled = true; // Set the START button state
                start_btn_flag = true;
                start_button_on();
                break;
                
            case STOP: // STOP button is pressed or not pressed
                motor_enabled = false;
                start_btn_flag = false;
                stop_button_on();
                break;
                
             case FORWARD:
                motor_enabled = true;
                start_button_on();// Motor turns on while Button 3 is pressed
                break;
                
             case DEBUG:
                 ili9341_clear_display(COLOR_LIGHTGREY);
                 draw_pin_state_box();
                 draw_state_buttons_debug_mode();
                 DEBUG_MODE = true;
                 break;
             case BACK:
                DEBUG_MODE = false;
                draw_main_screen();
                break;
                
            default:
                if(start_btn_flag == 1){
                    motor_enabled = true;
                }else{
                    motor_enabled = false;
                    start_btn_flag = false;
                    if(!DEBUG_MODE){
                    stop_button_on();
                    }
                }
                break;
        }
        
        motor_state_set(motor_enabled);
    }
    return EXIT_FAILURE;
}
