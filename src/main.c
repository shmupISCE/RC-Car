#include <stddef.h>
#include <stdbool.h>
#include "definitions.h"
#include "display_control.h"

#define DIR GPIO_PIN_RB8
#define PWM GPIO_PIN_RD0
#define ENBL GPIO_PIN_RC1

bool motor_enabled = false;

/* LEGACY CODE FOR BUTTON*/
volatile bool button_flag = false;


void GPIO_RA4_IntHandle(GPIO_PIN pin, uintptr_t context) {
    button_flag = true;
}

#define STOP 0
#define START 1

volatile bool flag = false;
// Callback funckija za eksterni interrupt na pinu RB15 (setovanje flag-a ukoliko se stanje na pinu promeni na logicku 0):
void GPIO_RB15_IntHandler(GPIO_PIN pin, uintptr_t context){
    if(GPIO_PinRead(pin)==0){
        flag = true;
    }
}

float *display_init_coef(float *coeff){
        ili9341_init(); 
    ili9341_clear_display(COLOR_WHITE);    
                                          
    // Mozda i ne treba proveriti pin config
    GPIO_RB14_Clear();
    GPIO_RB14_OutputEnable();

    
    GPIO_PinInterruptCallbackRegister(GPIO_PIN_RB15, GPIO_RB15_IntHandler, 0);  // Registrovanje GPIO_RB15_IntHandler funkcije kao callback funkcije za eksterni interrupt na pinu RB15.
    GPIO_PinIntEnable(GPIO_PIN_RB15, GPIO_INTERRUPT_ON_MISMATCH);               // Omogucavanje generisanja interrupt-a na pinu RB15 za svaku promenu stanja na pinu (ON MISMATCH).   
    
    uint8_t cnt=0;
    uint8_t xdyd[6]={32, 120, 160, 24, 32, 24};                                 // Poznate koordinate dispeleja tri tacke na displeju u kojima se vrsi kalibracija (P1, P2 i P3).
    uint16_t xtyt[6];
    uint16_t xt[1], yt[1];
    
    while(cnt<3){                                                               // Postupak ocitavanja pritiska touch panela u zadatoj tacki ponavlja se tri puta za kalibracione tacke P1, P2 i P3.
        ili9341_draw_filled_circle(xdyd[2*cnt],xdyd[2*cnt+1],7,COLOR_RED);      // Prikaz tacke Pi, i = 1,2,3.
        if(flag){                                                               // Ukoliko se desio interrupt na RB15:    
            flag = false;                                                       // Resetovanje flag-a.
            readXY(xt, yt);                                                     // Ocitavanje koordinata touch panela.
            printf("Tacka %u\r\n", cnt);
            xtyt[2*cnt] = xt[0];                                                // Cuvanje koordinata touch panela za tacke P1, P2 i P3 u nizu xtyt ---> {xP1, yP1, xP2, yP2, xP3, yP3}.
            xtyt[2*cnt+1] = yt[0];
            
            cnt++;
            CORETIMER_DelayMs(2000);
        }   
    }
    
    ili9341_clear_display(COLOR_WHITE);
    calculateCoeffs(coeff, xdyd, xtyt);
    
    return coeff;
}

int main(void) {
    /* Initialize all modules */
    SYS_Initialize(NULL);

    float coef[6];
    display_init_coef(coef);

    /* RA4 pin as an input for the button */
    GPIO_PinInputEnable(GPIO_PIN_RA4);  // Ne verujem Harmony-u 

    /* Configure the RA4 pin for interrupt on mismatch */
    GPIO_PinIntEnable(GPIO_PIN_RA4, GPIO_INTERRUPT_ON_MISMATCH);
    GPIO_PinInterruptCallbackRegister(GPIO_PIN_RA4, GPIO_RA4_IntHandle, 0);

    uint16_t x[1];
    uint16_t y[1];
    uint8_t button_pressed = STOP;

    draw_buttons();
    draw_state_led();
    draw_manual_drive_buttons();
    stop_button_on();
    
    GPIO_PinSet(ENBL);
    GPIO_PinWrite(DIR, 1);

    
    while (true) {
        readXY(x, y);
        button_pressed = check_button_hitbox(x, y, coef);
            switch (button_pressed) {
            case 1: // START button is pressed
                motor_enabled = true; // Set the START button state
                break;
            case 2: // STOP button is pressed or not pressed
                motor_enabled = false;
                break;
        }
    GPIO_PinWrite(PWM, motor_enabled);
    }
    return EXIT_FAILURE;
}
