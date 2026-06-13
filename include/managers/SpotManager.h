#pragma once

#include "core/ParkingSpot.h"
#include "core/ParkingLot.h"
#include <memory>

class SpotManager {
    ParkingLot& parkingLot;
    
public:
    SpotManager(ParkingLot& lot);
    
    void initializeSpots(int smallCount, int largeCount, int mcCount);
    
    ParkingSpot* allocateSpot(const class Vehicle& vehicle);
    bool releaseSpot(int spotId);
    
    void checkTimeouts();
    
    int getAvailableCount() const;
    int getOccupiedCount() const;
    int getReservedCount() const;
    int getTotalSpots() const;
    
    void displaySpotStatus() const;
    
    ParkingSpot* findSpotByPlate(const std::string& plate);
};
