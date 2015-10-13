#ifndef HOURGLASS_H
#define HOURGLASS_H

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
