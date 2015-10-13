#include "arduino.h"
#include "Hourglass.h"

namespace BFH
  {
    Hourglass* Hourglass::IsrVector;

    Hourglass::Hourglass ()
      {
        /* Register this pointer  */
        IsrVector = this;
      }

    CallbackInstruction::CallbackInstruction (unsigned long TimeToNextEvent,
        CallbackFunctionType CallbackFunction, unsigned long ResetValue)
      : TimeToNextEvent(TimeToNextEvent), ResetValue(ResetValue),
        CallbackFunction(CallbackFunction)
      { }

    ISR (TIMER2_COMPA_vect)
      {
        /* Call (C++ style) ISR  */
        Hourglass::IsrVector->Isr ();
      }

    void
    Hourglass::Init (int CycleTime)
      {
        /* Initialize timer2 to produce periodical interrupts  */

        /* Prevent interrupts  */
        cli();

        /* Reset registers  */
        TCCR2A = 0;
        TCCR2B = 0;
        TCNT2  = 0;

        switch (CycleTime)
          {
          case FAST:
            /* 16 MHz / (128 (prescale) * 1000 Hz) - 1  */
            OCR2A = 124;  /* Set 8-bit compare match register  */
              /* Prescale by 128  */
            TCCR2B |= (1 << CS22) | (0 << CS21) | (1 << CS20);
            break;

          case SLOW:
          default:
            /* 16 MHz / (1024 (prescale) * 100 Hz) - 1  */
            OCR2A = 155;  /* Set 8-bit compare match register  */
            /* Prescale by 1024  */
            TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
            break;
          }

        TCCR2A |= (1 << WGM21);   /* Enable CTC mode  */
        TIMSK2 |= (1 << OCIE2A);  /* Enable timer compare interrupt  */

        IsInitialized = true;

        /* Resume interrupts  */
        sei();
      }

    void
    Hourglass::Isr ()
      {
        for (int index = 0; index < cMaxInstructions; index++)
          {
            /* If there is an entry at the current position  */
            if (InstructionList[index])
              {
                /* Do tick (decrease internal counter, call function if
                   necessary...)  */
                bool done = InstructionList[index]->DoTick ();

                /* If the instruction has finished (no further executions
                   required), destroy it  */
                if (done)
                  {
                    /* Reset slot  */
                    delete InstructionList[index];
                    InstructionList[index] = 0;
                  }
              }
          }
      }

    bool
    Hourglass::RegisterEvent (unsigned long TicksFromNow,
        CallbackFunctionType Callback, unsigned long CycleTime)
      {
        /* If not manually initialized by now, use standard values for
          initialization  */
        if (!IsInitialized)
          Init ();

        /* Initialize variable that states: event not registred yet  */
        bool success = false;

        /* Prevent interrupts  */
        cli();

        /* For every slot on the instruction list  */
        for (int index = 0; index < cMaxInstructions; index++)
          {
            /* If the current slot is empty (available)  */
            if (!InstructionList[index])
              {
                /* Add instruction to this slot  */
                InstructionList[index] = new CallbackInstruction (TicksFromNow,
                  Callback, CycleTime);

                /* The event was successfully registred  */
                success = true;

                /* Stop searching for slots after successful registration  */
                break;
              }
          }

        /* Resume interrupts  */
        sei();

        /* Indicate whether the job was registred successfully  */
        return success;
      }

    void
    Hourglass::UnregisterAll ()
      {
        for (int index = 0; index < cMaxInstructions; index++)
          {
            /* Delete all jobs  */
            delete InstructionList[index];
            InstructionList[index] = 0;
          }
      }

    bool
    CallbackInstruction::DoTick ()
      {
        /* Initialize variable that states: the job is not done yet (further
           ticks need to be made)  */
        bool done = false;

        /* Fire event if required  */
        if (TimeToNextEvent == 0)
          {
            /* Check, whether it is a recurring event  */
            if (ResetValue)
              /* If so: reload counter variable  */
              TimeToNextEvent = ResetValue;
            else
              /* Otherwise: the job is done after this call  */
              done = true;

            /* Fire event  */
            CallbackFunction ();
          }

        /* Count down (the value can not be zero right now). Even freshly reset
           counters must be decremented, as they shall fire e.g. at the next
           DoTick call in case that they had a reload value of 1  */
        TimeToNextEvent--;

        /* Indicate whether the job is finished (and can be removed)  */
        return done;
      }
  }

BFH::Hourglass Hourglass;
