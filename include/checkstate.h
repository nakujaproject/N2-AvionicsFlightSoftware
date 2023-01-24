#ifndef CHECKSTATE_H
#define CHECKSTATE_H

//we initialize functions that will be included in checkstate.cpp

int checkInPoweredFlight(float altitude);

int checkForApogee(float velocity, float currentAltitude, float previousAltitude);

int deployChute(float altitude);

int checkGround(float altitude);

int checkState(float currentAltitude, float previousAltitude, float velocity, float acceleration, int state);

#endif