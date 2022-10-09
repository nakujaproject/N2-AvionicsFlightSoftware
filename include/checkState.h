#ifndef CHECKSTATE_H
#define CHECKSTATE_H

int checkInPoweredFlight(float altitude);

int checkForBurnOut(float acceleration);

int checkForApogee(float velocity, float currentAltitude, float previousAltitude);

int deployChute(float altitude);

int checkGround(float altitude);

int checkState(float currentAltitude, float previousAltitude, float velocity, float acceleration, int state);

#endif
