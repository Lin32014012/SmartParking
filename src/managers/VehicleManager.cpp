#include "managers/VehicleManager.h"
#include <iostream>
#include <chrono>

VehicleManager::VehicleManager() {}

bool VehicleManager::registerVehicle(std::unique_ptr<Vehicle> vehicle) {
    if (vehicles.findByPlate(vehicle->getPlate()) != nullptr) {
        return false;
    }
    vehicles.add(std::move(vehicle));
    return true;
}

bool VehicleManager::removeVehicle(const std::string& plate) {
    return vehicles.remove(plate);
}

Vehicle* VehicleManager::findByPlate(const std::string& plate) {
    return vehicles.findByPlate(plate);
}

size_t VehicleManager::getVehicleCount() const {
    return vehicles.count();
}

void VehicleManager::displayAllVehicles() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "               当前停放车辆               " << std::endl;
    std::cout << "==========================================" << std::endl;
    
    if (vehicles.count() == 0) {
        std::cout << "  无车辆 " << std::endl;
    } else {
        vehicles.forEach([](const Vehicle& v) {
            std::cout << "  车牌: " << v.getPlate() 
                      << " | 类型: " << v.getType() << std::endl;
        });
    }
    
    std::cout << "========================================\n" << std::endl;
}

std::vector<VehicleRecord> VehicleManager::exportVehicles() const {
    std::vector<VehicleRecord> records;
    vehicles.forEach([&records](const Vehicle& v) {
        VehicleRecord r;
        r.plateNumber = v.getPlate();
        r.vehicleType = v.getType();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            v.getEntryTime().time_since_epoch());
        r.entryTimestamp = duration.count();
        records.push_back(r);
    });
    return records;
}
