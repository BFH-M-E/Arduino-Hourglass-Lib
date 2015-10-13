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

#ifndef HOURGLASS_H
#define HOURGLASS_H

#ifndef __AVR_ATmega2560__
  #error Sorry, the Hourglass library only works with Arduino Mega2560
#endif

enum HourglassCycleTime
  {
    SLOW,
    FAST,
  };

namespace BFH
  {
    typedef void(*CallbackFunctionType)();

    const int cMaxInstructions = 10;

    class CallbackInstruction
      {
      public:
        CallbackInstruction (unsigned long TimeToNextEvent,
          CallbackFunctionType CallbackFunction, unsigned long ResetValue);

        bool DoTick ();

      private:
        volatile unsigned long TimeToNextEvent;
        volatile unsigned long ResetValue;
        volatile CallbackFunctionType CallbackFunction;
      };

    class Hourglass
      {
      public:
        Hourglass ();
        void Init (int CycleTime = SLOW);

        bool RegisterEvent (unsigned long TicksFromNow,
          CallbackFunctionType Callback, unsigned long CycleTime = 0);

        void UnregisterAll ();

        void Isr ();

        static Hourglass* IsrVector;

      private:
        bool IsInitialized = false;
        CallbackInstruction* InstructionList[cMaxInstructions];
      };
  }

extern BFH::Hourglass Hourglass;

#endif /* end of include guard: HOURGLASS_H */
