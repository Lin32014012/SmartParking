#include "managers/SpotManager.h"
#include "core/Vehicle.h"

SpotManager::SpotManager(ParkingLot& lot) : parkingLot(lot) {}

void SpotManager::initializeSpots(int smallCount, int largeCount, int mcCount) {
    parkingLot.initializeSpots(smallCount, largeCount, mcCount);
}

ParkingSpot* SpotManager::allocateSpot(const Vehicle& vehicle) {
    return parkingLot.allocateSpot(vehicle);
}

bool SpotManager::releaseSpot(int spotId) {
    return parkingLot.releaseSpot(spotId);
}

void SpotManager::checkTimeouts() {
    parkingLot.checkAllTimeouts();
}

int SpotManager::getAvailableCount() const {
    return parkingLot.getAvailableCount();
}

int SpotManager::getOccupiedCount() const {
    return parkingLot.getOccupiedCount();
}

int SpotManager::getReservedCount() const {
    return parkingLot.getReservedCount();
}

int SpotManager::getTotalSpots() const {
    return parkingLot.getTotalSpots();
}

void SpotManager::displaySpotStatus() const {
    parkingLot.displaySpotStatus();
}

ParkingSpot* SpotManager::findSpotByPlate(const std::string& plate) {
    return parkingLot.findSpotByPlate(plate);
}
