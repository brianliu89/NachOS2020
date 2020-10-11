void Alarm::CallBack() {
    Interrupt *interrupt = kernel->interrupt;
    MachineStatus status = interrupt->getStatus();
    bool woken = _bedroom.MorningCall();
    
    if (status == IdleMode && !woken && _bedroom.IsEmpty()) {// is it time to quit?
        if (!interrupt->AnyFutureInterrupts()) {
        timer->Disable();	// turn off the timer
    }
    } else {			// there's someone to preempt
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