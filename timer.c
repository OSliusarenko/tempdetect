volatile unsigned long deepSleepCnt, deepSleep5sec, deepSleepCnt5sec;
void deepSleep(unsigned int sec_intervals);

void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
{
    if(++deepSleepCnt==4) // approx 1 seconds interval
    {
        deepSleepCnt = 0;
        if(++deepSleepCnt5sec==deepSleep5sec)
        {
            _BIC_SR_IRQ(LPM0_bits + GIE); // Clear LPM0 bits from 0(SR)
        };
    };
}

void deepSleep(unsigned int sec_intervals)
{
    // SMCLK as WDT+ source is not good since consumes much
    // power but since the quartz is abscent...
    BCSCTL2 |= DIVS_3;                        // SMCLK/8
    WDTCTL = WDT_MDLY_32;                   // WDT SMCLK 32ms interval timer

    deepSleepCnt = 0;
    deepSleepCnt5sec = 0;
    deepSleep5sec = sec_intervals;

    IE1 = WDTIE;                             // Enable WDT interrupt
    _BIS_SR(LPM0_bits + GIE);               // Enter LPM0
    WDTCTL = WDTPW | WDTHOLD;
}

