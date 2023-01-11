char * stateEnumToText[] =
{
   "S_NO", ///< Used for initialisation if state is not yet known KEEP NO START AND INIT
   "S_START",       /// < simulates [*]
   "S_INIT",
   "S_WAITINPUT",
   "S_CHECKCHANGE",
   "S_LOGERROR",
   "S_AIRFLOW",
   "S_MOISTURIZE",
   "S_HEAT"
};

char * lightStateEnumToText[] =
{
   "GREEN",
   "ORANGE",
   "RED"
};
