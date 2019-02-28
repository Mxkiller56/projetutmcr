#include "TaskMgr.h"
schedEvt::schedEvt(){
  // setting all pointer to NULL first
  this->next = NULL;
  this->actionPtr = NULL;
  this->when = NEVER;
  // last execution of the task: never
  this->last_exec = NEVER;
}
void schedEvt::begin(time_t when, void (*actionPtr)(void)){
  setWhen(when);
  setAction(actionPtr);
}
schedEvt *schedEvt::getNext(void){return this->next;}
void schedEvt::setWhen(time_t when){
  this->when = when;
  // if you change the execution time, then last_exec should be reset
  // (it's like making a new task re-using the same memory space)
  this->last_exec = NEVER;
}
time_t schedEvt::getWhen(void){return this->when;}
void schedEvt::setAction(void (*actionPtr)(void)){
  // actionPtr
  this->actionPtr = actionPtr;
}
void schedEvt::setNext(schedEvt *evt){
  // next event can be replaced even if it is not null
  // but we shouldn't have a next event with the value
  // of *our* address
  if (evt != this)
    this->next = evt; // relies on static type checking
}
bool schedEvt::execAction(time_t utcnow){
  // 1. check if actionPtr is not null 2. check if time has come to exec, or if (+-)delta is not too important
  if (this->actionPtr != NULL &&
      // we're after event start minus negative delta
      utcnow - NEG_DELTA >= this->getWhen() && 
      // action not executed
      this->last_exec == NEVER &&
      // delta is not too important
      this->getWhen() <= utcnow + POS_DELTA){
    // ok, we can call exec action
    (*this->actionPtr)();
    this->last_exec = utcnow; // don't forget to set last_exec time
    return true; // exec was ok
  }
  else {
    return false;
  }
}
/** unlinks itself from the schedEvent chain pointed by chainptr */
void schedEvt::unlink_myself(schedEvt *chainptr){ // chain can be size 1 or greater
  schedEvt *curr_ev = chainptr;
  schedEvt *prev_event = chainptr;
  // /!\ event chain is not linear in term of memory addresses 
  //     no next event                     reached this (us)
  while (curr_ev->getNext() != NULL  && curr_ev != this) {
    prev_event = curr_ev;
    curr_ev = curr_ev->getNext(); // current event becomes the next event
  }
  // end reached  or eventpr found
  // if chain size is 1:
  // curr_ev->getNext() will return NULL
  // prev_event (aka curr_ev in this case) will still be NULL
  // else if chain size is > 1 and this < chainptr:
  //                 *pop*     (i.e next)
  // | prev_event | curr_ev | curr_ev->getNext() |
  //        ^___________next___________/
  // else if eventpr = chainptr
  //                *pop*
  // | prev_event | curr_ev | (no next)
  //      ^_____NULL__/
  prev_event->setNext(curr_ev->getNext());
  // reset yourself, you are alone now !
  this->setNext(NULL);
}
