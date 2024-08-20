#pragma once
#ifndef SPEEDMETRICS_H
#define SPEEDMETRICS_H

class MetersPerSecond {
public:
    explicit MetersPerSecond(double value) : value_(value) {}

    double getValue() const {
        return value_;
    }

    // Conversion to other units can be added here
    double toKilometersPerHour() const {
        return value_ * 3.6;
    }

    double toMilesPerHour() const {
        return value_ * 2.23694;
    }

private:
    double value_;
};

class KilometersPerHour {
public:
    explicit KilometersPerHour(double value) : value_(value) {}

    double getValue() const {
        return value_;
    }

    // Conversion to other units can be added here
    double toMetersPerSecond() const {
        return value_ / 3.6;
    }

    double toMilesPerHour() const {
        return value_ / 1.60934;
    }

private:
    double value_;
};

class MilesPerHour {
public:
    explicit MilesPerHour(double value) : value_(value) {}

    double getValue() const {
        return value_;
    }

    // Conversion to other units can be added here
    double toMetersPerSecond() const {
        return value_ / 2.23694;
    }

    double toKilometersPerHour() const {
        return value_ * 1.60934;
    }

private:
    double value_;
};

#endif // SPEEDMETRICS_H
