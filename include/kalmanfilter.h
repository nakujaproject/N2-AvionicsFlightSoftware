#ifndef KALMANFILTER_H
#define KALMANFILTER_H

struct FilteredValues kalmanUpdate(float altitude, float acceleration);

#endif