// Some project that indicates whether the temperature
// of the water in tubes is hot or low so that there's a need
// to supply more wood into the kettle

#include <msp430f2003.h>
#define LEDOK   BIT6
#define LEDLOW   BIT0
#define BUTTON BIT3
#define SENSPWR BIT4 


volatile int chipTemperature;
volatile unsigned int sensorData, threshold;
volatile _Bool zeroButtonPressed;

void Init(void);
void getTemperatures(void);
void warnTemperatureOK(void);
void warnTemperatureLow(void);
void blinkLEDs(void);


#include "SD16.c"
#include "timer.c"


void main(void) {

    Init();
/*
    while(1) // test consuming power
    {

        P1OUT |= SENSPWR;
        SD16_init();
        __delay_cycles(10000000);
        SD16_off();
        P1OUT &= ~SENSPWR;
        
        P1OUT |= LEDOK;
        __delay_cycles(10000000);
        P1OUT &= ~LEDOK;

        P1OUT |= LEDLOW;
        __delay_cycles(10000000);
        P1OUT &= ~LEDLOW;
    }
*/

    // about 0.5mA at measure, 3mA - each LED
    
    while(1)
    {
        getTemperatures();

        if(zeroButtonPressed)
        {
            zeroButtonPressed = 0;
            threshold = sensorData;
            blinkLEDs();
        };

        if(sensorData > threshold) warnTemperatureOK();
        else warnTemperatureLow();

        deepSleep(2); // 2 sec pause
    }
        
} // main


//**********************************************************************

void Init(void)
{
    WDTCTL = WDTPW + WDTHOLD;
    
    P1OUT = 0;
    P1DIR = (LEDOK + LEDLOW + SENSPWR);

    zeroButtonPressed = 0;

    P1REN = BUTTON; // pull-up resistors on
    P1OUT = BUTTON; // pull-up the buttons
    P1IES = BUTTON; // define interrupt 1->0
    P1IFG = 0;
    P1IE  = BUTTON; // enable interrupts

    threshold = 0;
    
}; //Init()

void getTemperatures(void)
{
    P1OUT |= SENSPWR;
    SD16_init();

    __delay_cycles(5000);
    
    chipTemperature = SD16_getChipTemperature();
    sensorData = SD16_getSensorData();
    
    SD16_off();
    P1OUT &= ~SENSPWR;
    
}; //getTemperatures()

void warnTemperatureOK(void)
{
    P1OUT |= LEDOK;
    __delay_cycles(100000);
    P1OUT &= ~LEDOK;
}; //warnTemperatureOK()

void warnTemperatureLow(void)
{
    P1OUT |= LEDLOW;
    __delay_cycles(100000);
    P1OUT &= ~LEDLOW;
}; //warnTemperatureLow()

void blinkLEDs(void)
{
    P1OUT |= (LEDLOW | LEDOK);
    __delay_cycles(500000);
    P1OUT &= ~(LEDLOW | LEDOK);
    __delay_cycles(500000);
    P1OUT |= (LEDLOW | LEDOK);
    __delay_cycles(100000);
    P1OUT &= ~(LEDLOW | LEDOK);
    __delay_cycles(500000);
    P1OUT |= (LEDLOW | LEDOK);
    __delay_cycles(500000);
    P1OUT &= ~(LEDLOW | LEDOK);
}; //blinkLEDs()

//**********************************************************************

// Buttons interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void P1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) P1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    P1IFG &= ~BUTTON;
    zeroButtonPressed = 1;
} //P1_ISR
