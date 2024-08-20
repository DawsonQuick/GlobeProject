#pragma once
#ifndef TIME_H
#define TIME_H

class Second {
public:
    explicit Second(double value) : value_(value) {}

    double getValue() const {
        return value_;
    }

    double toMinutes() const {
        return value_ / 60.0;
    }

    double toHours() const {
        return value_ / 3600.0;
    }

private:
    double value_;
};

class Minute {
public:
    explicit Minute(double value) : value_(value) {}

    double getValue() const {
        return value_;
    }

    double toSeconds() const {
        return value_ * 60.0;
    }

    double toHours() const {
        return value_ / 60.0;
    }

private:
    double value_;
};

class Hour {
public:
    explicit Hour(double value) : value_(value) {}

    double getValue() const {
        return value_;
    }

    double toSeconds() const {
        return value_ * 3600.0;
    }

    double toMinutes() const {
        return value_ * 60.0;
    }

private:
    double value_;
};

#endif // TIME_H
