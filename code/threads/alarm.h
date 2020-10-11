#include "copyright.h"
#include "utility.h"
#include "callback.h"
#include "timer.h"
#include <list>
#include "thread.h"
class Bedroom {
    public:
        Bedroom():_current_interrupt(0) {};
        void PutToBed(Thread *t, int x);
    bool MorningCall();
    bool IsEmpty();
    private:
        class Bed {
            public:
                Bed(Thread* t, int x):
                    sleeper(t), when(x) {};
                Thread* sleeper;
                int when;
        };
    
    int _current_interrupt;
    std::list<Bed> _beds;
};
// The following class defines a software alarm clock. 
class Alarm : public CallBackObj {
  public:
    Alarm(bool doRandomYield);	// Initialize the timer, and callback 
                // to "toCall" every time slice.
    ~Alarm() { delete timer; }
    
    void WaitUntil(int x);	// suspend execution until time > now + x
  private:
    Timer *timer;		// the hardware timer device
    Bedroom _bedroom;
    void CallBack();		// called when the hardware
                // timer generates an interrupt
};