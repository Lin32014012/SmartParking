#pragma once

#include "core/ParkingSpot.h"
#include "utils/FileManager.h"
#include <vector>
#include <memory>
#include <mutex>

class ParkingLot {
    std::vector<std::unique_ptr<ParkingSpot>> spots;
    int totalSpots;
    mutable std::mutex allocMutex;   // 保护车位分配/释放的临界区，支持多线程并发分配

public:
    ParkingLot();

    void initializeSpots(int smallCount, int largeCount, int mcCount);

    ParkingSpot* findAvailableSpot(const class Vehicle& vehicle);
    ParkingSpot* allocateSpot(const Vehicle& vehicle);
    // 线程安全：在同一把锁内完成 查找空位→预留→占用，保证并发下不会重复分配同一车位
    ParkingSpot* parkVehicle(const Vehicle& vehicle);
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
