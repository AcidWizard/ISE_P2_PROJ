/*!
 * Implementation of the finite statemachine for a turnstile
 * as defined by Robert C. Martin in UML Tutorial: Finite State Machines
 * Code uses a pre-defined state handler (fsm_functions)
 * adapted from code created by Hugo Arends, HAN by
 * John van den Hooven
 *
 * Implementation is based on Figure 2 in the document
 *
 * This version of the code is a monolitic implementation
 * All functions and subsystem code is implemented in main.c
 *
 */

#include <stdio.h>
#include <unistd.h>

/// Finite State Machine Library
#include "fsm_functions/fsm.h"

/// Development Console Library
#include "console_functions/keyboard.h"
#include "console_functions/display.h"
#include "console_functions/devConsole.h"

///Prototypes and Variables
#include "prototypes.h"
#include "variables.h"

/// Own variables



extern char * eventEnumToText[];
extern char * stateEnumToText[];

event_t event;
state_t state;


// Functions(simulation) run in subsystems
event_t InitialiseSubsystems(void);

//Subsystem(simulation) functions
//EF_ is used for Event Functions



///Helper function example
void delay_us(uint32_t d);

// Main
int main(void)
{

   /// Define the state machine model
   /// First the state and the pointer to the onEntry and onExit functions
   //           State                           onEntry()                   onExit()
   FSM_AddState(S_START,        &(state_funcs_t){  NULL,                    NULL               });
   FSM_AddState(S_INIT,         &(state_funcs_t){  S_Init_onEntry,          S_Init_onExit      });
   FSM_AddState(S_WAITINPUT,    &(state_funcs_t){  S_waitinput_onEntry,     NULL               });
   FSM_AddState(S_CHECKCHANGE,  &(state_funcs_t){  S_checkchange_onEntry,   NULL               });
   FSM_AddState(S_STATUSLIGHT,  &(state_funcs_t){  S_statuslight_onEntry,   NULL               });
   FSM_AddState(S_LOGERROR,     &(state_funcs_t){  S_logerror_onEntry,      NULL               });
   FSM_AddState(S_AIRFLOW,      &(state_funcs_t){  S_airflow_onEntry,       NULL               });
   FSM_AddState(S_MOISTURIZE,   &(state_funcs_t){  S_moisturize_onEntry,    NULL               });
   FSM_AddState(S_HEAT,         &(state_funcs_t){  S_heat_onEntry,          NULL               });




   /// Second the transitions
   //                                 From            Event                To
   FSM_AddTransition(&(transition_t){ S_START,        E_INIT,              S_INIT           });
   FSM_AddTransition(&(transition_t){ S_INIT,         E_INITSUCCES,        S_WAITINPUT      });
   FSM_AddTransition(&(transition_t){ S_WAITINPUT,    E_INPUTCHANGED,      S_CHECKCHANGE    });
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_NOACTION,          S_WAITINPUT      });
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_OUTSIDEBOUNDS,     S_LOGERROR       });
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_NORMALCHANGE,      S_STATUSLIGHT    });
   FSM_AddTransition(&(transition_t){ S_LOGERROR,     E_ERRORLOGGED,       S_STATUSLIGHT    });
   FSM_AddTransition(&(transition_t){ S_STATUSLIGHT,  E_CO2LOW,            S_AIRFLOW        });
   FSM_AddTransition(&(transition_t){ S_STATUSLIGHT,  E_MOISTURELOW,       S_MOISTURIZE     });
   FSM_AddTransition(&(transition_t){ S_STATUSLIGHT,  E_TOOCOLD,           S_HEAT           });
   FSM_AddTransition(&(transition_t){ S_STATUSLIGHT,  E_RESET,             S_WAITINPUT      });
   FSM_AddTransition(&(transition_t){ S_AIRFLOW,      E_RESET,             S_WAITINPUT      });
   FSM_AddTransition(&(transition_t){ S_MOISTURIZE,   E_RESET,             S_WAITINPUT      });
   FSM_AddTransition(&(transition_t){ S_HEAT,         E_RESET,             S_WAITINPUT      });

   /// Use this test function to test your model
   /// FSM_RevertModel();


   // Should unexpected events in a state be flushed or not?
   FSM_FlushEnexpectedEvents(true);

   /// Start the state machine
   FSM_RunStateMachine(S_START, E_INIT);

   /// Use this test function to test your model
   /// FSM_RevertModel();

   return 0;
}



/// Local function prototypes State related

void S_Init_onExit(void)
{

}

void S_Init_onEntry(void)
{
   event_t nextevent;

   /// Simulate the initialisation
   nextevent = InitialiseSubsystems();// insert function to run

   FSM_AddEvent(nextevent);           /// Internal generated event
}

event_t empty(void) {
    return E_RESET;
}

///Subsystem (simulation) functions
event_t InitialiseSubsystems(void)
{
   state_t state;
   DSPinitialise();
   DSPshowDisplay();
   KYBinitialise();

   state = FSM_GetState();
   DCSdebugSystemInfo("S_Init_onEntry:");
   DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);
   DSPshow(2,"System Initialized No errors");

   return(E_INITSUCCES);
}

void S_waitinput_onEntry(void)  {
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = InitialiseSubsystems();// insert function to run

    FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_checkchange_onEntry(void) {
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = empty();// insert function to run

    FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_statuslight_onEntry(void) {
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = empty();// insert function to run

    FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_logerror_onEntry(void) {
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = empty();// insert function to run

    FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_airflow_onEntry(void) {
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = empty();// insert function to run

    FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_moisturize_onEntry(void) {
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = empty();// insert function to run

    FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_heat_onEntry(void) {
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = empty();// insert function to run

    FSM_AddEvent(nextevent);           /// Internal generated event
}

// simulate delay in microseconds
void delay_us(uint32_t d)
{
   DCSdebugSystemInfo("Delay waiting for %d micro-seconds", d);
   sleep(10000);
}
