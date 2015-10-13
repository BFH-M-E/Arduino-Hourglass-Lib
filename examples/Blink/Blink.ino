/* Hourglass library example

   The hourglass library uses timer2 to produce timer interrupts at a given
   cycle time. This cycle time can be chosen at the initialization step of
   the library:

   Hourglass.Init ();        : Use standard (SLOW) mode, 10 ms ticks
   Hourglass.Init (SLOW);    : Use SLOW mode,            10 ms ticks
   Hourglass.Init (FAST);    : Use FAST mode,             1 ms ticks

   NOTE: if you try to register an event without having the library
         initialized, the library initializes itself in standard mode (e.g.
         SLOW mode with 10 ms ticks)

   You can register up to ten events, of which each can be:

   Oneshot:       Fires once and then unregisters itself
   Continuous:    Fires continuously with given cycle time

   Do this with:

   // Oneshot call the TurnLedOn function after 100 ticks from now
   Hourglass.RegisterEvent (100, TurnLedOn);
   // or
   Hourglass.RegisterEvent (100, TurnLedOn, 0);

   // Continuously call the TurnLedOn function every 50 ticks. The first of
   // those function calls shall be in 100 ticks
   Hourglass.RegisterEvent (100, TurnLedOn, 50);

   You can check, whether an even was registred properly by checking the
   RegisterEvent function's return value (of type bool):

   if (Hourglass.RegisterEvent (100, TurnLedOn))
     // registred successfully
   else
     // event queue is already full (event was not registred)

   You can reset all currently registred events by a call to:

   Hourglass.UnregisterAll ();

   WARNING: beware that the callbacks are in interrupt context. This means you
            must keep the functions very (!) short (e.g. less than 10
            instructions). You should also define every variable that is
            changed in a callback function as "volatile".

   And now have fun with the Hourglass library for ATMEGA 2560 boards!  */

#include <Hourglass.h>

bool LedState = LOW;
const int LedPin = 13;

void
setup ()
{
  pinMode (LedPin, OUTPUT);

  /* Initialize Hourglass library with slow cycles (time per tick: 10 ms)  */
  Hourglass.Init ();

  /* Register event
     0:          Time (in Ticks, e.g. 0 x 10 ms) to first execution
     ToggleLed:  Function to execute
     50:         Cycle time (in Ticks, e.g. 50 x 10 ms)

     This instruction registers an event that calls the ToggleLed function
     every 500 ms after an initial delay of 0 ms. Therefore this event
     occures over and over again and produces a 1 Hz blinking of the led
     on pin 13  */
  Hourglass.RegisterEvent (0, ToggleLed, 50);
}

void
loop ()
{
  /* Nothing happens in here  */
  while (1)
    {  }
}

void
ToggleLed ()
{
  LedState = !LedState;
  digitalWrite (LedPin, LedState);
}
