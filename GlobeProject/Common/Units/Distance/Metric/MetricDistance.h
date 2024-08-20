#pragma once
#ifndef METRICDISTANCE_H
#define METRICDISTANCE_H

class Meter {
public:
    explicit Meter(double value) : value_(value) {}

    double getValue() const {
        return value_;
    }

    // Conversion to other units can be added here
    double toKilometers() const {
        return value_ / 1000.0;
    }

    double toCentimeters() const {
        return value_ * 100.0;
    }

private:
    double value_;
};

class Kilometer {
public:
    explicit Kilometer(double value) : value_(value) {}

    double getValue() const {
        return value_;
    }

    // Conversion to other units can be added here
    double toMeters() const {
        return value_ * 1000.0;
    }

private:
    double value_;
};

class Centimeter {
public:
    explicit Centimeter(double value) : value_(value) {}

    double getValue() const {
        return value_;
    }

    // Conversion to other units can be added here
    double toMeters() const {
        return value_ / 100.0;
    }

private:
    double value_;
};

#endif // DISTANCEMETRICS_H