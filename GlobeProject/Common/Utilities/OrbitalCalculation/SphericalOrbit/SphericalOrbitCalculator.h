#pragma once
#ifndef SPHEREICALORBITCALCULATOR_H
#define SPHEREICALORBITCALCULATOR_H
#include <iostream>
#include <cmath>
#include <utility>
#include <chrono>
const float M_PI = 3.14159265358979323846f;
const double G = 6.67430e-11; // gravitational constant
const double M = 5.972e24;    // mass of the Earth (in kg)

// Function to calculate orbital speed
inline double orbitalSpeed(double G, double M, double r) {
    return std::sqrt(G * M / r);
}

// Function to calculate orbital period
inline double orbitalPeriod(double G, double M, double r) {
    return 2 * M_PI * std::sqrt(r * r * r / (G * M));
}

// Function to calculate position in orbit at time t in 3D
inline std::tuple<double, double, double> positionInOrbit(double r, double T, double t, double i, double omega) {
    double x = r * (std::cos(omega) * std::cos(2 * M_PI * t / T) - std::sin(omega) * std::sin(2 * M_PI * t / T) * std::cos(i));
    double y = r * (std::sin(omega) * std::cos(2 * M_PI * t / T) + std::cos(omega) * std::sin(2 * M_PI * t / T) * std::cos(i));
    double z = r * (std::sin(2 * M_PI * t / T) * std::sin(i));
    return { x, y, z };
}


inline std::tuple<double, double, double> getPositionInOrbitAtCurrentTime(double radius, double inclination, double omega, double period) {
    auto now = std::chrono::system_clock::now();
    // Get the duration since the epoch
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    double t = fmod(seconds, period);
    return positionInOrbit(radius, period, t, inclination, omega);
    
}

inline std::vector<float> generateSphericalOrbitTrajectory(double radius, double inclination , double omega) {
    double v = orbitalSpeed(G, M, radius);
    double T = orbitalPeriod(G, M, radius);


    //TODO: use fmod(unix epoch, orbitalPeriod) 
    //Then pass that in to the positionInOrbit to get position in real time

    float previousX;
    float previousY;
    float previousZ;
    bool isFirst = true;
    std::vector<float> trajectoryData;
    for (double t = 0; t <= T; t += T / 100) {
        auto [x, y, z] = positionInOrbit(radius, T, t, inclination, omega);

        if (isFirst) {
            previousX = x;
            previousY = y;
            previousZ = z;
            isFirst = false;
        }
        else {
            trajectoryData.push_back(static_cast<float>(previousX));
            trajectoryData.push_back(static_cast<float>(previousY));
            trajectoryData.push_back(static_cast<float>(previousZ));

            trajectoryData.push_back(static_cast<float>(x));
            trajectoryData.push_back(static_cast<float>(y));
            trajectoryData.push_back(static_cast<float>(z));

            previousX = x;
            previousY = y;
            previousZ = z;

        }


    }
    return trajectoryData;
}

#endif