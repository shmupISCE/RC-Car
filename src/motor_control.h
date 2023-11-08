#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "definitions.h"
// Mikro BUS socket 1 
#define DIR         GPIO_PIN_RB8
#define PWM         GPIO_PIN_RD0
#define ENBL        GPIO_PIN_RC1

void enable_motor(void);
void disable_motor(void);
void start_motor(void);
void stop_motor(void);
void motor_dir_forwad(void);
void motor_dir_backwards(void);
void motor_state_toggle(void);
void motor_dir_toggle(void);

void motor_state_set(bool state);

// Default settings
void motor_init(void);


#endif