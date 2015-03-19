/*
 * author: Sebastian Wallin
 * description:
 * Example on how to configure the periodical execution of a user
 * defined function (Interrupt service routine) using Timer2. This
 * example will run the function every 1ms.
 *
 * For detailed information on Timer2 configuration see chapter 17 in
 * ATMEGA328 datasheet.
 
 http://popdevelop.com/2010/04/mastering-timer-interrupts-on-the-arduino/
 
 */
 
/* Timer2 reload value, globally available */
unsigned int tcnt2;

/* Setup phase: configure and enable timer2 overflow interrupt */
void setupInterrupt() {
 
   /* First disable the timer overflow interrupt while we're configuring */
  TIMSK2 &= ~(1<<TOIE2);
 
  /* Configure timer2 in normal mode (pure counting, no PWM etc.) */
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
  TCCR2B &= ~(1<<WGM22);
 
  /* Select clock source: internal I/O clock */
  ASSR &= ~(1<<AS2);
 
  /* Disable Compare Match A interrupt enable (only want overflow) */
  TIMSK2 &= ~(1<<OCIE2A);
 
  /* Now configure the prescaler to CPU clock divided by 128 */
  TCCR2B |= (1<<CS22)  | (1<<CS20); // Set bits
  TCCR2B &= ~(1<<CS21);             // Clear bit
 
  /* We need to calculate a proper value to load the timer counter.
   * The following loads the value 131 into the Timer 2 counter register
   * The math behind this is:
   * (CPU frequency) / (prescaler value) = 125000 Hz = 8us.
   * (desired period) / 8us = 125.
   * MAX(uint8) + 1 - 125 = 131;
   */
  /* Save value globally for later reload in ISR */
  tcnt2 = 131; 
 
  /* Finally load end enable the timer */
  TCNT2 = tcnt2;
  TIMSK2 |= (1<<TOIE2);
}
 
/*
 * Install the Interrupt Service Routine (ISR) for Timer2 overflow.
 * This is normally done by writing the address of the ISR in the
 * interrupt vector table but conveniently done by using ISR()  */
ISR(TIMER2_OVF_vect) {
  /* Reload the timer */
  TCNT2 = tcnt2;
  
  interrupt();
}

void interrupt(){
  sevenSeg_display();
}
