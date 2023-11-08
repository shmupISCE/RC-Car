#include "motor_control.h"
#include "definitions.h"
static bool enbl_motor_flag;
static bool start_motor_flag;
static bool dir_motor_flag;


void enable_motor(void){
    GPIO_PinWrite(ENBL, 1);
    enbl_motor_flag = true;
}

void disable_motor(void){
    GPIO_PinWrite(ENBL, 0);
    enbl_motor_flag = false;
}

void start_motor(void){
    GPIO_PinWrite(PWM, 1);
    start_motor_flag = true;
}

void stop_motor(void){
    GPIO_PinWrite(PWM, 0);
    start_motor_flag = false;
}

void motor_dir_forwad(void){
    GPIO_PinWrite(DIR, 1);
    dir_motor_flag = true;
}
void motor_dir_backwards(void){
    GPIO_PinWrite(DIR, 0);
    dir_motor_flag = false;
}

void motor_state_toggle(void){
    GPIO_PinToggle(PWM);
}
void motor_dir_toggle(void){
    GPIO_PinToggle(DIR);
}

// Default settings
void motor_init(void){
    enable_motor();
    motor_dir_forwad();
    stop_motor();
}

void motor_state_set(bool state){
    GPIO_PinWrite(PWM, state);
}