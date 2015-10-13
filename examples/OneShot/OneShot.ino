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

const int LedPin = 13;

void
setup ()
{
  pinMode (13, OUTPUT);

  /* Initialize Hourglass library with slow cycles (time per tick: 10 ms)  */
  Hourglass.Init ();

  /* Register event
     100:        Time (in Ticks, e.g. 100 x 10 ms) to (first) execution
     TurnLedOn:  Function to execute

     This instruction registers an event that calls the TurnLedOn function
     after 1 s. After the execution, the event unregisters itself, so the
     user has nothing to clean up  */
  Hourglass.RegisterEvent (100, TurnLedOn);

  /* Register second event (call TurnLedOff function after 1.1 s)

     NOTE: you can register up to ten events  */
  Hourglass.RegisterEvent (110, TurnLedOff);
}

void
loop ()
{
  /* Nothing happens in here  */
  while (1)
    {  }
}

void
TurnLedOn ()
{
  digitalWrite (LedPin, HIGH);
}

void
TurnLedOff ()
{
  digitalWrite (LedPin, LOW);
}
