#pragma once

#include "core/Vehicle.h"
#include "utils/EntityManager.h"
#include "utils/FileManager.h"
#include <memory>
#include <string>

class VehicleManager {
    EntityManager<Vehicle> vehicles;
    
public:
    VehicleManager();
    
    bool registerVehicle(std::unique_ptr<Vehicle> vehicle);
    bool removeVehicle(const std::string& plate);
    Vehicle* findByPlate(const std::string& plate);
    
    size_t getVehicleCount() const;
    
    void displayAllVehicles() const;
    
    std::vector<VehicleRecord> exportVehicles() const;
    
    template<typename Func>
    void forEachVehicle(Func func);
};

template<typename Func>
void VehicleManager::forEachVehicle(Func func) {
    vehicles.forEach(func);
}
