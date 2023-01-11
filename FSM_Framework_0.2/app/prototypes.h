#ifndef PROTOTYPES_H
#define PROTOTYPES_H

#endif // PROTOTYPES_H

//Usefull functions
void showCurrentState(void);

//State Functions
void S_Init_onExit(void);
void S_Init_onEntry(void);
void S_waitinput_onEntry(void);
void S_checkchange_onEntry(void);
void S_logerror_onEntry(void);
void S_airflow_onEntry(void);
void S_moisturize_onEntry(void);
void S_heat_onEntry(void);
