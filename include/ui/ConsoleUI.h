#pragma once

#include "core/ParkingLot.h"
#include "managers/VehicleManager.h"
#include "managers/SpotManager.h"
#include "managers/BillingManager.h"

class ConsoleUI {
    ParkingLot& parkingLot;
    VehicleManager& vehicleManager;
    SpotManager& spotManager;
    BillingManager& billingManager;
    bool running; 
    
public:
    ConsoleUI(ParkingLot& lot, VehicleManager& vm, SpotManager& sm, BillingManager& bm);
    
    void run(); 
    
private:
    void displayMainMenu();
    void displaySystemStatus();
    
    void handleVehicleEntry();
    void handleVehicleExit();
    void handleQueryVehicle();
    void handleSpotStatus();
    void handleStatistics();
    void handleSettings();
    
    void displayNavigation(const std::string& spotCode);
    
    void loadSavedData();
    void saveAllData();
    
    int getIntInput(const std::string& prompt);
    std::string getStringInput(const std::string& prompt);
    void pauseScreen();
};
