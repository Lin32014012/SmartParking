#pragma once

#include "core/ParkingSpot.h"
#include "utils/FileManager.h"
#include <vector>
#include <memory>

class ParkingLot {
    std::vector<std::unique_ptr<ParkingSpot>> spots;
    int totalSpots;
    
public:
    ParkingLot();
    
    void initializeSpots(int smallCount, int largeCount, int mcCount);
    
    ParkingSpot* findAvailableSpot(const class Vehicle& vehicle);
    ParkingSpot* allocateSpot(const Vehicle& vehicle);
    bool releaseSpot(int spotId);
    
    void checkAllTimeouts();
    int getTotalSpots() const;
    int getAvailableCount() const;
    int getOccupiedCount() const;
    int getReservedCount() const;
    void displaySpotStatus() const;
    
    std::vector<ParkingSpot*> getAllSpots() const;
    
    std::vector<SpotRecord> exportSpots() const;
    void importSpots(const std::vector<SpotRecord>& records);
    ParkingSpot* findSpotById(int spotId);
    ParkingSpot* findSpotByPlate(const std::string& plate);
};
