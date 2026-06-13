#pragma once

#include <string>
#include <chrono>
#include <optional>

enum class SpotStatus {
    Empty,
    Reserved,
    Occupied
};

struct Reservation {
    std::string plateNumber;
    std::chrono::system_clock::time_point reservedAt;
    static constexpr int TIMEOUT_SECONDS = 120;
    
    bool isExpired() const;
};

class ParkingSpot {
protected:
    int spotId;
    std::string zone;
    int row;
    int position;
    SpotStatus status;
    std::optional<Reservation> reservation;
    std::string occupiedPlate;
    
public:
    ParkingSpot(int id, const std::string& z, int r, int p);
    virtual ~ParkingSpot() = default;
    
    bool isEmpty() const;
    bool isReserved() const;
    bool isOccupied() const;
    
    bool reserve(const std::string& plate);
    bool occupy(const std::string& plate);
    void release();
    void checkReservationTimeout();
    
    std::string getSpotCode() const;
    int getSpotId() const;
    std::string getZone() const;
    SpotStatus getStatus() const;
    std::string getPlate() const;
    
    virtual bool canFitVehicle(const class Vehicle& vehicle) const = 0;
    virtual std::string getSpotType() const = 0;
};

class SmallSpot : public ParkingSpot {
public:
    SmallSpot(int id, const std::string& z, int r, int p);
    bool canFitVehicle(const Vehicle& vehicle) const override;
    std::string getSpotType() const override;
};

class LargeSpot : public ParkingSpot {
public:
    LargeSpot(int id, const std::string& z, int r, int p);
    bool canFitVehicle(const Vehicle& vehicle) const override;
    std::string getSpotType() const override;
};

class MCSpot : public ParkingSpot {
public:
    MCSpot(int id, const std::string& z, int r, int p);
    bool canFitVehicle(const Vehicle& vehicle) const override;
    std::string getSpotType() const override;
};
