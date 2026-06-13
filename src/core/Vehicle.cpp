#include "core/Vehicle.h"
#include "core/ParkingSpot.h"

Vehicle::Vehicle(const std::string& plate)
: plateNumber(plate), entryTime(std::chrono::system_clock::now()) {}

void Vehicle::setEntryTime(std::chrono::system_clock::time_point time) {
    entryTime = time;
}

std::string Vehicle::getPlate() const {
    return plateNumber;
}

std::chrono::system_clock::time_point Vehicle::getEntryTime() const {
    return entryTime;
}

double Vehicle::calculateParkingDuration() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - entryTime);
    return duration.count() / 60.0;
}

double Vehicle::calculateFee() const {
    double hours = calculateParkingDuration();
    return hours * getHourlyRate();
}

Car::Car(const std::string& plate) : Vehicle(plate) {}

double Car::getHourlyRate() const {
    return 5.0;
}

std::string Car::getType() const {
    return "轿车";
}

bool Car::canFitInSpot(const ParkingSpot& spot) const {
    return spot.getSpotType() == "小车位";
}

Truck::Truck(const std::string& plate) : Vehicle(plate) {}

double Truck::getHourlyRate() const {
    return 10.0;
}

std::string Truck::getType() const {
    return "卡车";
}

bool Truck::canFitInSpot(const ParkingSpot& spot) const {
    return spot.getSpotType() == "大车位";
}

Motorcycle::Motorcycle(const std::string& plate) : Vehicle(plate) {}

double Motorcycle::getHourlyRate() const {
    return 2.5;
}

std::string Motorcycle::getType() const {
    return "摩托车";
}

bool Motorcycle::canFitInSpot(const ParkingSpot& spot) const {
    return spot.getSpotType() == "摩托车车位";
}
