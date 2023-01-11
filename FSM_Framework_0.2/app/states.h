#ifndef STATES_H
#define STATES_H

typedef enum {
   S_NO,               ///< Used for initialisation if state is not yet known
   S_START,            /// simulates [*]
   S_INIT,
   S_WAITINPUT,
   S_CHECKCHANGE,
   S_STATUSLIGHT,
   S_LOGERROR,
   S_AIRFLOW,
   S_MOISTURIZE,
   S_HEAT,
   //end
} state_t;

#endif
