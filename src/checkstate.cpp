#include "functions.h"
#include "defs.h"
#include "checkstate.h"

// variable for detected apogee height
float MAX_ALTITUDE = 0;

// This checks that we have started ascent
// compares the current displacement to the set threshold of the ground state displacement
// if found to be above, we have achieved lift off
int checkInPoweredFlight(float altitude)
{
    float displacement = altitude - BASE_ALTITUDE;
    if (displacement > GROUND_STATE_DISPLACEMENT)
    {
        return POWERED_FLIGHT_STATE;
    }
    else
    {
        return PRE_FLIGHT_GROUND_STATE;
    }
}

// This checks that we have reached apogee
// At apogee velocity is zero so we check for velocity less than or equal to zero
// we also check if the current altitude is less than the previous altitude
// this would determine that the rocket has began descent
int checkForApogee(float velocity, float currentAltitude, float temporalMaxAltitude)
{
    if ((temporalMaxAltitude - currentAltitude) > 5)
    {

        ejection();
        return CHUTE_DESCENT_STATE;
    }
    else if (velocity <= 0)
    {
        ejection();
        return CHUTE_DESCENT_STATE;
    }
    else
    {
        return POWERED_FLIGHT_STATE;
    }
}

// This checks that we have reached the ground
// detects landing of the rocket
// TODO: BASE_ALTITUDE might be different from the original base altitude
int checkGround(float altitude)
{
    float displacement = altitude - BASE_ALTITUDE;
    if (displacement < GROUND_STATE_DISPLACEMENT)
    {
        return POST_FLIGHT_GROUND_STATE;
    }
    else
    {
        return CHUTE_DESCENT_STATE;
    }
}

// Updates the state-machine state
// We check if rocket has launched to move from PRE_FLIGHT_GROUND_STATE to POWERED_FLIGHT_STATE
// We check if we have reached apogee to move to BALLISTIC_DESCENT_STATE
// We deploy parachute to move to CHUTE_DESCENT_STATE
// We check if we have reached the ground to move to POST_FLIGHT_GROUND_STATE
int checkState(float currentAltitude, float temporalMaxAltitude, float velocity, float acceleration, int state)
{
    switch (state)
    {
    case PRE_FLIGHT_GROUND_STATE:
        return checkInPoweredFlight(currentAltitude);
    case POWERED_FLIGHT_STATE:
        return checkForApogee(velocity, currentAltitude, temporalMaxAltitude);
    case CHUTE_DESCENT_STATE:
        return checkGround(currentAltitude);
    case POST_FLIGHT_GROUND_STATE:
        return POST_FLIGHT_GROUND_STATE;
    default:
        return checkInPoweredFlight(currentAltitude);
    }
}