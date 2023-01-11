#ifndef EVENTS_H
#define EVENTS_H

typedef enum {
   E_NO,                ///< Used for initialisation of an event variable
   E_INIT,
   E_INITSUCCES,
   E_INITERROR,
   E_INPUTCHANGED,
   E_NOACTION,
   E_OUTSIDEBOUNDS,
   E_ERRORLOGGED,
   E_CO2LOW,
   E_MOISTURELOW,
   E_TOOCOLD,
   E_RESET,
} event_t;

#endif
