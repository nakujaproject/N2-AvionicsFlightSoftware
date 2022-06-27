#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <BasicLinearAlgebra.h>
#include "defs.h"

using namespace BLA;

float q = 0.0001;

float T = 0.1;

// The system dynamics
BLA::Matrix<3, 3> A = {1.0, 0.1, 0.005,
                       0, 1.0, 0.1,
                       0, 0, 1.0};

// Relationship between measurement and states
BLA::Matrix<2, 3> H = {1.0, 0, 0,
                       0, 0, 1.0};

// Initial posteriori estimate error covariance
BLA::Matrix<3, 3> P = {1, 0, 0,
                       0, 1, 0,
                       0, 0, 1};

// Measurement error covariance
BLA::Matrix<2, 2> R = {0.25, 0,
                       0, 0.75};

// Process noise covariance
BLA::Matrix<3, 3> Q = {q, q, q,
                       q, q, q,
                       q, q, q};

// Identity Matrix
BLA::Matrix<3, 3> I = {1, 0, 0,
                       0, 1, 0,
                       0, 0, 1};

BLA::Matrix<3, 1> x_hat = {1500.0,
                           0.0,
                           0.0};

BLA::Matrix<2, 1> Y = {0.0,
                       0.0};

// kalmanUpdate This filteres our altitude and acceleration values
struct FilteredValues kalmanUpdate(float altitude, float acceleration)
{
    struct FilteredValues return_val;

    // Measurement matrix
    BLA::Matrix<2, 1> Z = {altitude,
                           acceleration};
    // Predicted state estimate
    BLA::Matrix<3, 1> x_hat_minus = A * x_hat;
    // Predicted estimate covariance
    BLA::Matrix<3, 3> P_minus = A * P * (~A) + Q;
    // Kalman gain
    BLA::Matrix<2, 2> con = (H * P_minus * (~H) + R);
    BLA::Matrix<3, 2> K = P_minus * (~H) * Invert(con);
    // Measurement residual
    Y = Z - (H * x_hat_minus);
    // Updated state estimate
    x_hat = x_hat_minus + K * Y;
    // Updated estimate covariance
    P = (I - K * H) * P_minus;
    Y = Z - (H * x_hat_minus);

    return_val.displacement = x_hat(0);
    return_val.velocity = x_hat(1);
    return_val.acceleration = x_hat(2);

    return return_val;
}

#endif