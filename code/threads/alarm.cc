// alarm.cc
//	Routines to use a hardware timer device to provide a
//	software alarm clock.  For now, we just provide time-slicing.
//
//	Not completely implemented.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "alarm.h"
#include "main.h"

//----------------------------------------------------------------------
// Alarm::Alarm
//      Initialize a software alarm clock.  Start up a timer device
//
//      "doRandom" -- if true, arrange for the hardware interrupts to 
//		occur at random, instead of fixed, intervals.
//----------------------------------------------------------------------

Alarm::Alarm(bool doRandom)
{
    timer = new Timer(doRandom, this);
}


void 
Alarm::CallBack() 
{
    Interrupt *interrupt = kernel->interrupt;
    MachineStatus status = interrupt->getStatus();
    
    if (status == IdleMode) {
        if (!interrupt->AnyFutureInterrupts()) {
	    timer->Disable();
	}
    } else {
	interrupt->YieldOnReturn();
    }
}
void Alarm::WaitUntil(int x) {
    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
    Thread* t = kernel->currentThread;
    cout << "Alarm::WaitUntil go sleep" << endl;
    _bedroom.PutToBed(t, x);
    kernel->interrupt->SetLevel(oldLevel);
}
bool Bedroom::IsEmpty() {
    return _beds.size() == 0;
}
void Bedroom::PutToBed(Thread*t, int x) {
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    _beds.push_back(Bed(t, _current_interrupt + x));
    t->Sleep(false);
}
bool Bedroom::MorningCall() {
    bool woken = false;
    _current_interrupt ++;
    for(std::list<Bed>::iterator it = _beds.begin(); 
        it != _beds.end(); ) {
        if(_current_interrupt >= it->when) {
            woken = true;
            cout << "Bedroom::MorningCall Thread woken" << endl;
            kernel->scheduler->ReadyToRun(it->sleeper);
            it = _beds.erase(it);
        } else {
            it++;
        }
    }
    return woken;
}

