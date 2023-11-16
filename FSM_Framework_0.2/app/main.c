/*!
 * The C program written by G. Buursink en L. Gout is an
 * implementation of a finite state machine (FSM) model of a Plant Module.
 * The FSM is a mathematical model of computation that represents the behavior
 * of a system by specifying the possible states it can be in, the transitions
 * between those states, and the actions that are performed when transitioning
 * between states. In the case of a Plant Module, the states might include
 * "Waiting for Input" "Moisturizing" "Logging Error" and so on.
 * The transitions between states would be determined by the values of the
 * sensors on the plant, such as a decrease in moisture level or a decrease in
 * temperature. The actions performed by the FSM would be the turning on of a
 * heater or fan and any accompanying changes in the display or status light
 * This program provides a precise and rigorous way of modeling the
 * behavior of a Plant Module, which can be useful for understanding how the
 * Plant Module works, testing its performance, and potentially even improving
 * its design.
 */


/// Include external libraries
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

/// Prototypes and Variables
#include "prototypes.h"
#include "variables.h"

/// External Enum
extern char * eventEnumToText[];
extern char * stateEnumToText[];
extern char * lightStateEnumToText[];

/// Add event and state variables
event_t event;
state_t state;



// Functions(simulation) run in subsystems
event_t EF_InitialiseSubsystems(void);

//Subsystem(simulation) functions
//EF_ is used for Event Functions
event_t EF_WAITINPUT(void);
event_t EF_CO2LOW(void);
event_t EF_MOISTURELOW(void);
event_t EF_TOOCOLD(void);

//HAL functions
void ChangeLight(int);
void LogError(void);
void OpenWindow(void);
void Moisturize(void);
void HeatPlant(void);


/// Main
int main(void) {

   /// Define the state machine model
   /// First the state and the pointer to the onEntry and onExit functions
   //           State                              onEntry()                   onExit()
   FSM_AddState(S_START,        &(state_funcs_t){  NULL,                    NULL               });
   FSM_AddState(S_INIT,         &(state_funcs_t){  S_Init_onEntry,          S_Init_onExit      });
   FSM_AddState(S_WAITINPUT,    &(state_funcs_t){  S_waitinput_onEntry,     NULL               });
   FSM_AddState(S_CHECKCHANGE,  &(state_funcs_t){  S_checkchange_onEntry,   NULL               });
   FSM_AddState(S_LOGERROR,     &(state_funcs_t){  S_logerror_onEntry,      NULL               });
   FSM_AddState(S_AIRFLOW,      &(state_funcs_t){  S_airflow_onEntry,       NULL               });
   FSM_AddState(S_MOISTURIZE,   &(state_funcs_t){  S_moisturize_onEntry,    NULL               });
   FSM_AddState(S_HEAT,         &(state_funcs_t){  S_heat_onEntry,          NULL               });




   /// Define the state transistions
   //                                 From            Event                To
   FSM_AddTransition(&(transition_t){ S_START,        E_INIT,              S_INIT           });
   FSM_AddTransition(&(transition_t){ S_INIT,         E_INITSUCCES,        S_WAITINPUT      });
   FSM_AddTransition(&(transition_t){ S_WAITINPUT,    E_INPUTCHANGED,      S_CHECKCHANGE    });
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_NOACTION,          S_WAITINPUT      });
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_OUTSIDEBOUNDS,     S_LOGERROR       });
   FSM_AddTransition(&(transition_t){ S_LOGERROR,     E_ERRORLOGGED,       S_INIT           });
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_CO2LOW,            S_AIRFLOW        });
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_MOISTURELOW,       S_MOISTURIZE     });
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_TOOCOLD,           S_HEAT           });
   FSM_AddTransition(&(transition_t){ S_CHECKCHANGE,  E_RESET,             S_WAITINPUT      });
   FSM_AddTransition(&(transition_t){ S_AIRFLOW,      E_RESET,             S_WAITINPUT      });
   FSM_AddTransition(&(transition_t){ S_MOISTURIZE,   E_RESET,             S_WAITINPUT      });
   FSM_AddTransition(&(transition_t){ S_HEAT,         E_RESET,             S_WAITINPUT      });

   /// Use this test function to test the model
   /// FSM_RevertModel();


   /// Should unexpected events in a state be flushed or not?
   FSM_FlushEnexpectedEvents(true);

   /// Start the state machine
   FSM_RunStateMachine(S_START, E_INIT);


   return 0;
}



/// Init State Exit Function
void S_Init_onExit(void) {
///does nothing
}

/// Init State Entry Function
/// Initialises subsystem using EF_InitialiseSubsystems()
void S_Init_onEntry(void) {
   event_t nextevent;

   /// Simulate the initialisation
   nextevent = EF_InitialiseSubsystems();

   FSM_AddEvent(nextevent);           /// Internal generated event
}

/// WaitInput State Entry Function
/// Wait for a change in the input signals
void S_waitinput_onEntry(void)  {
    ChangeLight(0);
    showCurrentState();

    event_t nextevent;

    nextevent = EF_WAITINPUT();

    FSM_AddEvent(nextevent);
}

/// Check Change State Entry Function
/// Check if change in input signal needs action,
/// if so decide which action needs to be taken
void S_checkchange_onEntry(void) {
    showCurrentState();

    event_t nextevent;


    int function;

    /// Show user information on options
    DSPshow(4,"Insert Changed Situation");
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
            nextevent = E_OUTSIDEBOUNDS;
            FSM_AddEvent(nextevent);
            break;
        default:
            DSPshow(1,"Invalid input!\nPlease try again!");
            break;
    }
}


/// Log Error State Entry Function
/// State for logging an error
void S_logerror_onEntry(void) {
    ChangeLight(2);
    showCurrentState();
    event_t nextevent;

    LogError(); ///Log Error Function

    nextevent = E_ERRORLOGGED;

    FSM_AddEvent(nextevent);
}

/// Airflow State Entry Function
/// State for changing the airflow
void S_airflow_onEntry(void) {
    ChangeLight(1);
    showCurrentState();
    event_t nextevent;

    OpenWindow();

    nextevent = E_RESET;

    FSM_AddEvent(nextevent);
}

/// Moisturize State Entry Function
/// State for changing the moisture level
void S_moisturize_onEntry(void) {
    ChangeLight(1);
    showCurrentState();
    event_t nextevent;

    Moisturize();

    nextevent = E_RESET;

    FSM_AddEvent(nextevent);
}

/// Heat State Entry Function
/// State for changing the temperature
void S_heat_onEntry(void) {
    ChangeLight(1);
    showCurrentState();
    event_t nextevent;

    HeatPlant();

    nextevent = E_RESET;

    FSM_AddEvent(nextevent);
}


///Subsystem Change Light function
void ChangeLight(int d) {
    lightstatus = d;
    DCSdebugSystemInfo("lightstatus variable changed to: %d", d);
}

void LogError(void) {
    DSPshow(4, "Logging Error");
}

void OpenWindow(void) {
    DSPshow(4, "Opening Window");
}

void Moisturize(void) {
    DSPshow(4, "Moisturizing plant");
}

void HeatPlant(void) {
    DSPshow(4, "");
}

/// function to show current state on display and debug
void showCurrentState(void) {
    /// initialize needed variable
    state_t state;
    /// fetch current state from FSM-framework
    state = FSM_GetState();

    /// Show current state to user
    DSPshow(2, "Lightstatus: %s", lightStateEnumToText[lightstatus]);
    DCSdebugSystemInfo("Current State: %s", stateEnumToText[state]);
}

///Subsystem Initialisation function
event_t EF_InitialiseSubsystems(void) {
   state_t state;
   DSPinitialise();
   DSPshowDisplay();
   KYBinitialise();

   state = FSM_GetState();
   DCSdebugSystemInfo("S_Init_onEntry:");
   DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);
   DSPshow(4,"System Initialized No errors");

   return(E_INITSUCCES);
}

event_t EF_WAITINPUT(void) {
    DSPshow(4, "Awaiting Input");
    return E_INPUTCHANGED;
}

event_t EF_CO2LOW(void) {
    char input[10];
    float value;

    /// change co2 value here
    printf("Enter a co2 value(20-25 normal, 10-20 too low, otherwise error): ");
    fgets(input, sizeof(input), stdin); /// get user input
    value = atof(input);/// set value as float

    if ((value < 20) & (value > 10)) {
        return E_CO2LOW;
    }
    else if ((value < 25) & (value > 20)) {
        return E_NOACTION;
    }
    return E_OUTSIDEBOUNDS;
}

event_t EF_MOISTURELOW(void) {
    char input[10];
    float value;

    /// change moisture level here
    printf("Enter a moisture value(20-25 normal, 10-20 too low, otherwise error): ");
    fgets(input, sizeof(input), stdin); /// get user input
    value = atof(input);/// set value as float

    if ((value < 20) & (value > 10)) {
        return E_MOISTURELOW;
    }
    else if ((value < 25) & (value > 20)) {
        return E_NOACTION;
    }
    return E_OUTSIDEBOUNDS;
}

event_t EF_TOOCOLD(void) {
    char input[10];
    float value;

    /// change temperature here
    printf("Enter a temperature value(20-25 normal, 10-20 too low, otherwise error): ");
    fgets(input, sizeof(input), stdin); /// get user input
    value = atof(input);/// set value as float

    if ((value < 20) & (value > 10)) {
        return E_TOOCOLD;
    }
    else if ((value < 25) & (value > 20)) {
        return E_NOACTION;
    }
    return E_OUTSIDEBOUNDS;
}
