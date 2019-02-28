#ifndef TASKMGR_H
#define TASKMGR_H
#ifdef TESTING
#include "Arduino_testing.h"
#endif
#define NEVER 0
// action must be executed *after* the time set
// but you can exec it a little bit before if you wish,
// by modifying this variable
#define NEG_DELTA (0L)
// 10 minutes
#define POS_DELTA (600L)

/** Scheduler for tasks */

/** Event with scheduled action
 * can be used as a chained list of events
 * no sorting is done in terms of supposed event 
 * time of execution
 * Warning: only one chain can exist at a time
 * multiple chains aren't supported
 * Warning again: tasks will be executed once
 * and once only (one-shot).
 */
// TODO: - move unlink_myself to TaskMgr
//       - add insert method
//       - sort by date
//       - autoclean (delete executed tasks)
class schedEvt {
 public:
  schedEvt();
  void begin(time_t when, void (*actionPtr)(void)); // arduino-style
  void unlink_myself(schedEvt *chainptr); // remove this from chain (chainptr)
  schedEvt *getNext();
  void setNext(schedEvt *); // if you want to disassociate, just pass NULL. But you should use unlink.
  void setAction(void (*actionPtr)(void));
  bool execAction(time_t utcnow); // will not exec if +delta over 10 minutes from utcnow
  void setWhen(time_t when); // warning: resets last_exec
  time_t getWhen(void);
 private:
  time_t last_exec;
  schedEvt *next; // no next event by default
  void (*actionPtr)(void); // no action fun by default. No parameters.
  time_t when;
};

// should manage chain data
class TaskMgr {
 public:
  execTasks();
  addTask(schedEvt *);
 private:
  schedEvt *evs_p; // pointer to the beginning of the chained list
  unlinkTask(schedEvt *);
  insertTask(schedEvt *);
};

#endif
