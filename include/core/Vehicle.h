#pragma once

#include <string>
#include <chrono>

class ParkingSpot;

class Vehicle {
protected:
    std::string plateNumber;
    std::chrono::system_clock::time_point entryTime;
    
public:
    Vehicle(const std::string& plate);
    virtual ~Vehicle() = default;


    
    virtual double getHourlyRate() const = 0;
    virtual std::string getType() const = 0;
    virtual bool canFitInSpot(const ParkingSpot& spot) const = 0;
    
    void setEntryTime(std::chrono::system_clock::time_point time);
    
    std::string getPlate() const;
    std::chrono::system_clock::time_point getEntryTime() const;
    double calculateParkingDuration() const;
    double calculateFee() const;
};

class Car : public Vehicle {
public:
    Car(const std::string& plate);
    double getHourlyRate() const override;
    std::string getType() const override;
    bool canFitInSpot(const ParkingSpot& spot) const override;
};

class Truck : public Vehicle {
public:
    Truck(const std::string& plate);
    double getHourlyRate() const override;
    std::string getType() const override;
    bool canFitInSpot(const ParkingSpot& spot) const override;
};

class Motorcycle : public Vehicle {
public:
    Motorcycle(const std::string& plate);
    double getHourlyRate() const override;
    std::string getType() const override;
    bool canFitInSpot(const ParkingSpot& spot) const override;
};
