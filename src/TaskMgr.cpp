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
      // pos delta OK
      utcnow - getWhen() <= NEG_DELTA &&
      // neg delta OK
      getWhen() - utcnow <= POS_DELTA &&
      // action not executed
      this->last_exec == NEVER){
    // ok, we can call exec action
    (*this->actionPtr)();
    this->last_exec = utcnow; // don't forget to set last_exec time
    return true; // exec was ok
  }
  else {
    return false;
  }
}

/* ******* TaskMgr class methods ****** */
TaskMgr::TaskMgr(){
  evs_p = NULL; // inits root event pointer
}
void TaskMgr::unlink_task(schedEvt *task){ // chain can be size 1 or greater
  schedEvt *curr_ev = this->evs_p;
  schedEvt *prev_event = this->evs_p;
  // /!\ event chain is not linear in term of memory addresses 
  //     no next event                     reached this (us)
  while (curr_ev->getNext() != NULL  && curr_ev != task) {
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
  task->setNext(NULL);
}

/** inserts a task in the chained list
 * task is inserted at the right */
void TaskMgr::insert_task(schedEvt *task, schedEvt *where){
  //        insert here 
  //            v
  // | curr_ev | next_event ? |
  schedEvt *curr_ev = this->evs_p;
  while (curr_ev->getNext() != NULL && curr_ev != where){
    curr_ev = curr_ev->getNext();
  }
  // end reached or reached event where we should insert
  task->setNext(curr_ev->getNext()); // event after the task might be NULL
  curr_ev->setNext(task); // next event is now the task
}

/** sorts the tasks by date of execution at insertion
 * the more recent the first */
bool TaskMgr::addTask(schedEvt *event){
  schedEvt *curr_ev = evs_p;
  schedEvt *prev_ev = evs_p;
  bool twice = false;
  
  if (evs_p == NULL)
    evs_p = event; // case 1: no root
  else if (evs_p->getWhen() > event->getWhen()) { // case 2: younger than root (in this case, no double insert)
    event->setNext(evs_p);
    evs_p = event; // move root along
  }
  else { // case 3: general case. Double insert might happen
    twice = (curr_ev == event);
    while (curr_ev->getNext() != NULL){
      if (curr_ev->getWhen() > event->getWhen())
	break;
      prev_ev = curr_ev;
      curr_ev = curr_ev->getNext();
      twice = (curr_ev == event);
    }
    if (!twice) // nothing inserted twice
      insert_task(event,prev_ev);
    else
      return false; // caller might want to know if add failed
  }
  return true; // everything was OK. Inform the caller.
}


void TaskMgr::execTasks(time_t utcnow){
  schedEvt *curr_ev = this->evs_p;
  schedEvt *prev_ev = curr_ev;

  while (curr_ev != NULL){ // root may be NULL (no tasks left)
    if (curr_ev->execAction(utcnow)){ // true == exec ok
      if (curr_ev == this->evs_p){ // special case: exec ok for root
	this->evs_p = curr_ev->getNext(); // replace root by next or NULL
	curr_ev->setNext(NULL); // dont't call unlink this time, because unlink relies on evs_p
	curr_ev = evs_p; // jump to next event (root)
	prev_ev = curr_ev; // reset prev_ev also
      } else { // exec ok but not for root
	prev_ev = curr_ev;
	curr_ev = curr_ev->getNext(); // jump to next task (or NULL)
	unlink_task(prev_ev); // remove old task from chain (unlink uses evs_p and it must not be null !)
      }
    } else { // no exec, so just jump to next task
      prev_ev = curr_ev;
      curr_ev = curr_ev->getNext();
    }
  }
}
