#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const int PWM_pin = 23;   /* GPIO 1 as per WiringPi, GPIO18 as per BCM */

int main (void)
{
  int intensity = 0;            

  if (wiringPiSetup() == -1)
    exit (1) ;

  pinMode (PWM_pin, PWM_OUTPUT) ; /* set PWM pin as output */
  pwmSetClock (2);
  pwmSetRange(100);
  pwmSetMode(PWM_MODE_MS);
    pwmWrite(PWM_pin, 20);
  while (1)
  {
    // for (intensity = 0 ; intensity < 1024 ; ++intensity)
    // {
    //   pwmWrite (PWM_pin, intensity) ;	/* provide PWM value for duty cycle */
    //   delay (1) ;
    // }
    // delay(1);

    // for (intensity = 1023 ; intensity >= 0 ; --intensity)
    // {
    //   pwmWrite (PWM_pin, intensity) ;
    //   delay (1) ;
    // }
    // delay(1);
}
}