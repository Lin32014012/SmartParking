#include <iostream>
#include <memory>
#include "core/ParkingLot.h"
#include "managers/VehicleManager.h"
#include "managers/SpotManager.h"
#include "managers/BillingManager.h"
#include "ui/ConsoleUI.h"

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "            智能停车管理系统            " << std::endl;
    std::cout << "========================================" << std::endl;

    
    ParkingLot parkingLot;
    VehicleManager vehicleManager;
    SpotManager spotManager(parkingLot);
    BillingManager billingManager;
    
    std::cout << "正在初始化!" << std::endl;
    
    ConsoleUI ui(parkingLot, vehicleManager, spotManager, billingManager);
    ui.run();
    
    return 0;
}
