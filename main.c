// Some project that indicates whether the temperature
// of the water in tubes is hot or low so that there's a need
// to supply more wood into the kettle

#include <msp430f2003.h>
#define LEDOK   BIT6
#define LEDLOW   BIT0


volatile int chipTemperature, sensorTemperature;

void Init(void);
void getTemperatures(void);
void warnTemperatureOK(void);
void warnTemperatureLow(void);


#include "SD16.c"
#include "timer.c"


void main(void) {
    
    Init();
    
    while(1)
    {
        getTemperatures();

        if(chipTemperature > 250) warnTemperatureOK();
        else warnTemperatureLow();

        deepSleep(1); // 5 sec pause
    }
        
} // main


//**********************************************************************

void Init(void)
{
    WDTCTL = WDTPW + WDTHOLD;
    
    P1OUT = 0;
    P1DIR = (LEDOK + LEDLOW);
}; //Init()

void getTemperatures(void)
{
    SD16_init();
    
    chipTemperature = SD16_getChipTemperature();
//    sensorTemperature = SD16_getSensorTemperature();
    
    SD16_off();
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
    __delay_cycles(500000);
    P1OUT &= ~LEDLOW;
}; //warnTemperatureLow()

//**********************************************************************
