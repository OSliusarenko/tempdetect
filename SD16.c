void SD16_init(void);
int SD16_getChipTemperature(void);
int SD16_getSensorTemperature(void);
void SD16_off(void);

void SD16_init(void)
{
  SD16CTL = SD16REFON +SD16SSEL_1;          // 1.2V ref, SMCLK
  SD16CCTL0 = SD16SNGL + SD16IE + SD16UNI;           // Single conv, interrupt, unipolar
  SD16INCTL0 = SD16INTDLY_0;               // Interrupt on 4th sample 
}

int SD16_getSensorTemperature(void)
{
    unsigned int delta;
    int temp;

    SD16INCTL0 |= SD16INCH_7;                  // A7+/- (calibrate)
    SD16CCTL0 |= SD16SC;                      // Start SD16 conversion
    _BIS_SR(LPM0_bits + GIE);               // Enter LPM0
    delta = SD16MEM0;             // Save CH7 results (delta) (clears IFG)
    SD16INCTL0 &= ~SD16INCH_7;          // Disable channel A7+/-
    SD16INCTL0 |= SD16INCH_4;           // Enable channel A4+/-                          
    SD16AE |= SD16AE1;                  // Enable external input on A4+ 
    SD16CCTL0 |= SD16SC;                      // Start SD16 conversion 
    _BIS_SR(LPM0_bits + GIE);               // Enter LPM0    
    temp = SD16MEM0 - 13653 - delta;
    temp /= 55;
    temp -= 270; //250;			// calibrate
    SD16AE &= ~SD16AE1;                 // Disable external input A4+, A4
    SD16INCTL0 &= ~SD16INCH_4;          // Disable channel A4+/-
    
    return temp;
}

int SD16_getChipTemperature(void)
{
    int temp;

    SD16INCTL0 |= SD16INCH_6;           // Enable built-in temp sensor   
    SD16CCTL0 |= SD16SC;                      // Start SD16 conversion

    _BIS_SR(LPM0_bits + GIE);               // Enter LPM0    

    temp = SD16MEM0 - 39361;
    temp *= 2;
    temp /= 29;

    SD16INCTL0 &= ~SD16INCH_6;          // Disable channel A1+/-
    
    return temp;
}

void SD16_off(void)
{
    SD16CTL = 0;          
    SD16INCTL0 = 0;                  
    SD16CCTL0 = 0;           
};

void __attribute__ ((interrupt(SD16_VECTOR))) SD16ISR (void)
{
    _BIC_SR_IRQ(LPM0_bits + GIE);                   // Clear LPM0 bits from 0(SR)
}



