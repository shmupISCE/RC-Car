#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdio.h>
#include "definitions.h"
#include "ili9341.h"
#include "ili9341.h"                    // SYS function prototypes
#include "fonts.h"
#include "display_control.h"


volatile bool flag = false;

// Callback funckija za eksterni interrupt na pinu RB15 (setovanje flag-a ukoliko se stanje na pinu promeni na logicku 0):
void GPIO_RB15_IntHandler(GPIO_PIN pin, uintptr_t context){
    if(GPIO_PinRead(pin)==0){
        flag = true;
    }
}


// Funkcija za sortiranje niza od najmanje do najvece vrednosti:
void sort(uint16_t* arr, int n){
    int16_t i, key, j;
    
    for(i=1;i<n;i++){
        key = arr[i];
        
        j = i-1;
        
        while(j>=0 && arr[j]>key){
            arr[j+1] = arr[j];
            j=j-1;
        }
        arr[j+1] = key;
    }
}

void readXY(uint16_t* x, uint16_t* y){
    uint16_t data[21];
    
    // Pin RB3 je ulazni pin (Hi-Z):
    GPIO_PinInputEnable(GPIO_PIN_RB3);
    // Pin RB15 je ulazni (Hi-Z) i na njemu je onemoguceno generisanje eksternih interrupt-a:
    GPIO_PinIntDisable(GPIO_PIN_RB15);
    GPIO_RB15_InputEnable();                                
    // Pin RB2 postavlja se kao izlazni pin i na njemu se definise stanje logicke 1 (Vcc):
    GPIO_PinOutputEnable(GPIO_PIN_RB2);
    GPIO_PinSet(GPIO_PIN_RB2);

    for(uint8_t i=0; i<21; i++)                                                 // A/D konverzija se vrsi 21 put, kako bi se na kraju na 21 odbirak primenio median filtar i uklonili potencijalne ekstremne vrednosti usled bouncing-a.
    {
        ADCHS_ChannelConversionStart(ADCHS_CH3);
        while(!ADCHS_ChannelResultIsReady(ADCHS_CH3));
        data[i] = ADCHS_ChannelResultGet(ADCHS_CH3);                            // Ocitavanje izlaza A/D konvertora na ciji se ulaz dovodi signal sa pina RB3.
        CORETIMER_DelayUs(10);
    }
    sort(data, 21);                                                             // Sortiranje odbiraka od najmanjeg do najveceg.
    x[0] = data[10];                                                            // Uzimanje median vrednosti iz niza.

    // Pin RB2 je ulazni pin (Hi-Z):
    GPIO_PinInputEnable(GPIO_PIN_RB2);
    // Pin RB3 postavlja se kao izlazni pin i na njemu se definise stanje logicke 1 (Vcc):
    GPIO_PinOutputEnable(GPIO_PIN_RB3);
    GPIO_PinSet(GPIO_PIN_RB3);
    // Pin RB14 je ulazni pin (Hi-Z):
    GPIO_RB14_InputEnable();
    // Pin RB15 postavlja se kao izlazni pin i na njemu se definise stanje logicke 0 (GND):
    GPIO_RB15_OutputEnable();
    GPIO_RB15_Clear();

    for(uint8_t i=0; i<21; i++)                                                 
    {
        ADCHS_ChannelConversionStart(ADCHS_CH2);
        while(!ADCHS_ChannelResultIsReady(ADCHS_CH2));
        data[i] = ADCHS_ChannelResultGet(ADCHS_CH2);                            // Ocitavanje izlaza A/D konvertora na ciji se ulaz dovodi signal sa pina RB2.
        CORETIMER_DelayUs(10);
    }
    sort(data, 21);
    y[0] = data[10];

    // Standby mod:
    GPIO_RB14_OutputEnable();
    GPIO_RB14_Clear();
    GPIO_PinInputEnable(GPIO_PIN_RB2);
    GPIO_RB15_InputEnable();
    GPIO_PinInputEnable(GPIO_PIN_RB3);
    CORETIMER_DelayMs(1);
    GPIO_PinIntEnable(GPIO_PIN_RB15, GPIO_INTERRUPT_ON_MISMATCH);

}

void calculateCoeffs(float* coeff, uint8_t* xdyd, uint16_t* xtyt){
    coeff[0] = (float)(xdyd[0]*(xtyt[3]-xtyt[5])+xdyd[2]*(xtyt[5]-xtyt[1])+xdyd[4]*(xtyt[1]-xtyt[3]))/(float)(xtyt[0]*(xtyt[3]-xtyt[5])+xtyt[2]*(xtyt[5]-xtyt[1])+xtyt[4]*(xtyt[1]-xtyt[3]));
    coeff[1] = (float)(coeff[0]*(xtyt[4]-xtyt[2])+xdyd[2]-xdyd[4])/(float)(xtyt[3]-xtyt[5]);
    coeff[2] = (float)(xdyd[4]-coeff[0]*xtyt[4]-coeff[1]*xtyt[5]); 
    coeff[3] = (float)(xdyd[1]*(xtyt[3]-xtyt[5])+xdyd[3]*(xtyt[5]-xtyt[1])+xdyd[5]*(xtyt[1]-xtyt[3]))/(float)(xtyt[0]*(xtyt[3]-xtyt[5])+xtyt[2]*(xtyt[5]-xtyt[1])+xtyt[4]*(xtyt[1]-xtyt[3]));
    coeff[4] = (float)(coeff[3]*(xtyt[4]-xtyt[2])+xdyd[3]-xdyd[5])/(float)(xtyt[3]-xtyt[5]);
    coeff[5] = (float)(xdyd[5] - coeff[3]*xtyt[4] - coeff[4]*xtyt[5]);
}
font_t *font_setup_20x38(){
    font_t* test_font = (font_t*)malloc(sizeof(font_t)); 

    test_font->height = 38;
    test_font->width = 20;
    test_font->font = font_lucida_sans_20x38;
    return test_font;
}

void start_led_on(){
    ili9341_draw_filled_circle(150, 190, 9, COLOR_GREEN);
}

void stop_led_on(){
    ili9341_draw_filled_circle(150, 100, 9, COLOR_RED);
}

void start_led_off(){
    ili9341_draw_filled_circle(150, 190, 9, COLOR_WHITE);
}

void stop_led_off(){
    ili9341_draw_filled_circle(150, 100, 9, COLOR_WHITE);
}

void start_button_on(void){
    start_led_on();
    stop_led_off();
}

void stop_button_on(void){
    stop_led_on();
    start_led_off();
}


void draw_manual_drive_buttons(void){
    //Forward button and frame
    //ili9341_draw_filled_rectangle(191, 136, 119, 84, COLOR_LIGHTGREY);
    ili9341_draw_line(200, 150, 300, 150, COLOR_BLACK, 1);
    ili9341_draw_line(300, 150, 250, 210, COLOR_BLACK, 1);
    ili9341_draw_line(250, 210, 200, 150, COLOR_BLACK, 1);
    ili9341_draw_rectangle(190, 135, 120, 85, COLOR_BLACK, 2);
    
    //Backwards button and frame
    ili9341_draw_line(200, 90, 300, 90, COLOR_BLACK, 1);
    ili9341_draw_line(300, 90, 250, 30, COLOR_BLACK, 1);
    ili9341_draw_line(250, 30, 200, 90, COLOR_BLACK, 1);
    ili9341_draw_rectangle(190, 20, 120, 85, COLOR_BLACK, 2);
}

void draw_state_led(void){
    // Power ON state frame
    ili9341_draw_circle(150, 190, 10, COLOR_BLACK, 3);
    //Power OFF state frame
    ili9341_draw_circle(150, 100, 10, COLOR_BLACK, 3);
}

void draw_buttons(void){
    char* start_button  = "START";
    char* stop_button  = "STOP";
    font_t *test_font = font_setup_20x38();
    
    // START BUTTON
    ili9341_draw_filled_rectangle(30, 150, 100, 80, COLOR_GREEN);
    ili9341_write_text(29, 170, start_button, COLOR_BLACK, test_font);

    // STOP BUTTON
    ili9341_draw_filled_rectangle(30, 50, 100, 80, COLOR_RED);
    ili9341_write_text(40, 70, stop_button, COLOR_BLACK, test_font);
    
    //Button frames
    ili9341_draw_rectangle(29, 149, 101, 81, COLOR_BLACK, 1);
    ili9341_draw_rectangle(29, 49, 101, 81,COLOR_BLACK, 1);
}




float *display_init_coef(float *coeff){
    ili9341_init(); 
    ili9341_clear_display(COLOR_BLACK);    
                                          
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


uint8_t check_button_hitbox(uint16_t* x, uint16_t* y, float* coeff, bool debug_mode){
    uint8_t temp =0;
    x[0] = (uint16_t)(coeff[0]*x[0]+coeff[1]*y[0]+coeff[2]);
    y[0] = (uint16_t)(coeff[3]*x[0]+coeff[4]*y[0]+coeff[5]);
    
    
    ////// DEBUG MODE  //////
    if(debug_mode){
        if(x[0] >= 240 && x[0] <= 320){
            if(y[0] >= 0 && y[0] <=80){
                temp = BACK;
            }
        }
    /*
    // STOP button hitbox
        if(x[0] >= 240 && x[0] <= 320){
            if(y[0] >= 81 && y[0] <=160){
                    temp = STOP;
            }
        }
    //  START BUTTON hitbox
        if(x[0] >= 240 && x[0] <= 320){
            if(y[0] >= 161 && y[0] <=240){
                temp = START;
            }
        }
     * */
    }else{
    // Stop button hitbox
    if(x[0] >= 30 && x[0] <= 130){
        if(y[0] >= 50 && y[0] <=130){
            temp = STOP;
            }
        }
    
    // Start button hitbox
    if(x[0] >= 30 && x[0] <= 160){
        if(y[0] >= 150 && y[0] <=230){
            temp = START;
            }
        }
    
    // Forward button hitbox
    if(x[0] >= 193 && x[0] <= 310){
        if(y[0] >= 135 && y[0] <=220){
                temp = FORWARD;
            }
        }
    
    if(x[0] >= 30 && x[0] <= 100){
        if(y[0] >= 5 && y[0] <=40){
            temp = DEBUG;
            }
        }
    
    }

    return temp;
}


void draw_main_screen(void){
    ili9341_clear_display(COLOR_WHITE);
    draw_buttons();
    draw_state_led();
    draw_manual_drive_buttons();
    draw_debug_button();
}