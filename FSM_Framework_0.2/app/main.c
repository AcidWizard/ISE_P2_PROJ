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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
event_t EF_WAITINPUT(void);
event_t EF_CO2LOW(void);
event_t EF_MOISTURELOW(void);
event_t EF_TOOCOLD(void);
event_t EF_OUTSIDEBOUNDS(void);

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
   FSM_AddState(S_LOGERROR,     &(state_funcs_t){  S_logerror_onEntry,      NULL               });
   FSM_AddState(S_AIRFLOW,      &(state_funcs_t){  S_airflow_onEntry,       NULL               });
   FSM_AddState(S_MOISTURIZE,   &(state_funcs_t){  S_moisturize_onEntry,    NULL               });
   FSM_AddState(S_HEAT,         &(state_funcs_t){  S_heat_onEntry,          NULL               });




   /// Second the transitions
   //                                 From            Event                To
   FSM_AddTransition(&(transition_t){ S_START,        E_INIT,              S_INIT           });     //done
   FSM_AddTransition(&(transition_t){ S_INIT,         E_INITSUCCES,        S_WAITINPUT      });     //done
   FSM_AddTransition(&(transition_t){ S_WAITINPUT,    E_INPUTCHANGED,      S_CHECKCHANGE    });     //done
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_NOACTION,          S_WAITINPUT      });     //done
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_OUTSIDEBOUNDS,     S_LOGERROR       });     //done
   FSM_AddTransition(&(transition_t){ S_LOGERROR,     E_ERRORLOGGED,       S_WAITINPUT      });
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_CO2LOW,            S_AIRFLOW        });     //done
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_MOISTURELOW,       S_MOISTURIZE     });     //done
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_TOOCOLD,           S_HEAT           });     //done
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_RESET,             S_WAITINPUT      });     //done
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
    showCurrentState();

    event_t nextevent;

    /// Simulate the initialisation
    nextevent = EF_WAITINPUT();// insert function to run

    FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_checkchange_onEntry(void) {
    showCurrentState();

    event_t nextevent;


    int function;

    /// Show user information
    DSPshow(2,"Insert Changed Situation");
    function = DCSsimulationSystemInputChar("\n"
                                            "Press N for no change\n"
                                            "Press C for changed CO2 level\n"
                                            "Press M for changed moisture level\n"
                                            "Press T for changed temperature level\n"
                                            "Press E to trigger error\n",
                                            "N" "C" "M" "T" "E");

    /// Process the user response and transition to the next state
    /// depending on user input.
    switch (function) {
        case 'N':
            nextevent = E_NOACTION;
            FSM_AddEvent(nextevent);
            break;
        case 'C':
            nextevent = EF_CO2LOW();
            FSM_AddEvent(nextevent);
            break;
        case 'M':
            nextevent = EF_MOISTURELOW();
            FSM_AddEvent(nextevent);
            break;
        case 'T':
            nextevent = EF_TOOCOLD();
            FSM_AddEvent(nextevent);
            break;
        case 'E':
            nextevent = EF_OUTSIDEBOUNDS();
            FSM_AddEvent(nextevent);
            break;
        default:
            DSPshow(1,"Invalid input!\nPlease try again!");
            break;
    }
}


void S_statuslight_onEntry(void) {
    showCurrentState();
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = empty();// insert function to run

    FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_logerror_onEntry(void) {
    showCurrentState();
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = empty();// insert function to run

    FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_airflow_onEntry(void) {
    showCurrentState();
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = empty();// insert function to run

    FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_moisturize_onEntry(void) {
    showCurrentState();
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = empty();// insert function to run

    FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_heat_onEntry(void) {
    showCurrentState();
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

void showCurrentState(void) {
    /// initialize needed variable
    state_t state;

    /// fetch current state from FSM-framework
    state = FSM_GetState();

    /// Show current state to user
    DCSdebugSystemInfo("Current State: %s", stateEnumToText[state]);
}

event_t EF_WAITINPUT(void) {
    DSPshow(2,"Awaiting Input");
    return E_INPUTCHANGED;
}

event_t EF_CO2LOW(void) {
    char input[10];
    float value;
    /// change Incline here
    printf("Enter a temperature value(20-25 normal, 10-20 too low, otherwise error): ");
    fgets(input, sizeof(input), stdin); /// get user input
    value = atof(input);

    if (value < 20 & value > 10) {
        return E_CO2LOW;
    }
    else if (value < 25 & value > 20) {
        return E_NOACTION;
    }
    else return E_OUTSIDEBOUNDS;
}

event_t EF_MOISTURELOW(void) {
    char input[10];
    float value;
    /// change Incline here
    printf("Enter a temperature value(20-25 normal, 10-20 too low, otherwise error): ");
    fgets(input, sizeof(input), stdin); /// get user input
    value = atof(input);

    if (value < 20 & value > 10) {
        return E_MOISTURELOW;
    }
    else if (value < 25 & value > 20) {
        return E_NOACTION;
    }
    else return E_OUTSIDEBOUNDS;
}

event_t EF_TOOCOLD(void) {
    char input[10];
    float value;
    /// change Incline here
    printf("Enter a temperature value(20-25 normal, 10-20 too low, otherwise error): ");
    fgets(input, sizeof(input), stdin); /// get user input
    value = atof(input);

    if (value < 20 & value > 10) {
        return E_TOOCOLD;
    }
    else if (value < 25 & value > 20) {
        return E_NOACTION;
    }
    else return E_OUTSIDEBOUNDS;
}

event_t EF_OUTSIDEBOUNDS(void) {
    return E_OUTSIDEBOUNDS;
}
