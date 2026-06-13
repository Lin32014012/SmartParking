#pragma once

#include "core/Vehicle.h"
#include <cmath>

template<typename VehicleType>
class RatePolicy {
public:
    static double calculateFee(double hours) {
        return hours * VehicleType::BASE_RATE;
    }
};

template<>
class RatePolicy<Car> {
public:
    static double calculateFee(double hours, bool isNight = false) {
        double rate = isNight ? 3.0 : 5.0;
        return hours * rate;
    }
};

template<>
class RatePolicy<Truck> {
public:
    static double calculateFee(double hours) {
        double billableHours = std::ceil(hours);
        return billableHours * 10.0;
    }
};

template<>
class RatePolicy<Motorcycle> {
public:
    static double calculateFee(double hours) {
        return hours * 2.5;
    }
};
