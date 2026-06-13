#include "ui/ConsoleUI.h"
#include "utils/FileManager.h"
#include <iostream>
#include <limits>
#include <iomanip>
#include <chrono>

static const std::string DATA_FILE = "parking_data.txt";

ConsoleUI::ConsoleUI(ParkingLot& lot, VehicleManager& vm, SpotManager& sm, BillingManager& bm)
    : parkingLot(lot), vehicleManager(vm), spotManager(sm), billingManager(bm), running(true) {}

void ConsoleUI::run() {
    spotManager.initializeSpots(60, 30, 10);
    loadSavedData();
    
    while (running) {
        displayMainMenu();
        int choice = getIntInput("选择项目");
        
        switch (choice) {
            case 1: handleVehicleEntry(); break;
            case 2: handleVehicleExit(); break;
            case 3: handleQueryVehicle(); break;
            case 4: handleSpotStatus(); break;
            case 5: handleStatistics(); break;
            case 6: handleSettings(); break;
            case 0: running = false; break;
            default: std::cout << "无效选项，请重试" << std::endl; break;
        }
        system("cls");
    }
    
    saveAllData();
    std::cout << "\n感谢使用SmartParking，再见!" << std::endl;
    std::cout << "按 Enter 退出...";
    std::cin.get();
}

void ConsoleUI::displayMainMenu() {
    spotManager.checkTimeouts();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "             智能停车管理系统             " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "  状态：已占用 " << spotManager.getOccupiedCount() 
              << "/" << spotManager.getTotalSpots() 
              << " | 可用: " << spotManager.getAvailableCount() << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  1. 车辆进入" << std::endl;
    std::cout << "  2. 车辆离开" << std::endl;
    std::cout << "  3. 查询车辆" << std::endl;
    std::cout << "  4. 查看车位状态" << std::endl;
    std::cout << "  5. 统计" << std::endl;
    std::cout << "  6. 设置" << std::endl;
    std::cout << "  0. 退出" << std::endl;
    std::cout << "========================================" << std::endl;
}

void ConsoleUI::displaySystemStatus() {
    std::cout << "\n系统状态:" << std::endl;
    std::cout << "  总计: " << spotManager.getTotalSpots() << std::endl;
    std::cout << "  可用: " << spotManager.getAvailableCount() << std::endl;
    std::cout << "  占用: " << spotManager.getOccupiedCount() << std::endl;
    std::cout << "  预留: " << spotManager.getReservedCount() << std::endl;
}

void ConsoleUI::handleVehicleEntry() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "               车辆进入                   " << std::endl;
    std::cout << "==========================================" << std::endl;
    
    std::string plate = getStringInput("Enter plate number");
    
    if (vehicleManager.findByPlate(plate) != nullptr) {
        std::cout << "错误：车辆已停靠" << std::endl;
        pauseScreen();
        system("cls");
        return;
    }
    
    std::cout << "\n选择车辆类型:" << std::endl;
    std::cout << "  1. 轿车" << std::endl;
    std::cout << "  2. 卡车" << std::endl;
    std::cout << "  3. 摩托车" << std::endl;
    
    int typeChoice = getIntInput("选择");
    
    std::unique_ptr<Vehicle> vehicle;
    switch (typeChoice) {
        case 1: vehicle = std::make_unique<Car>(plate); break;
        case 2: vehicle = std::make_unique<Truck>(plate); break;
        case 3: vehicle = std::make_unique<Motorcycle>(plate); break;
        default:
            std::cout << "无效选择" << std::endl;
            pauseScreen();
            system("cls");
            return;
    }
    
    ParkingSpot* spot = spotManager.allocateSpot(*vehicle);
    if (spot) {
        spot->occupy(plate);
        vehicleManager.registerVehicle(std::move(vehicle));
        std::cout << "\n成功分配了车位!" << std::endl;
        std::cout << "  车位编号: " << spot->getSpotCode() << std::endl;
        displayNavigation(spot->getSpotCode());
        saveAllData();
        std::cout << "  [Saved to " << DATA_FILE << "]" << std::endl;
    } else {
        std::cout << "\n对不起，没有空位 " << vehicle->getType() << " 车位 " << std::endl;
    }
    
    pauseScreen();
}

void ConsoleUI::handleVehicleExit() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "                  汽车出口                " << std::endl;
    std::cout << "==========================================" << std::endl;
    
    std::string plate = getStringInput("输入车牌号码");
    
    Vehicle* vehicle = vehicleManager.findByPlate(plate);
    if (!vehicle) {
        std::cout << "错误：没有找到车辆" << std::endl;
        pauseScreen();
        system("cls");
        return;
    }
    
    BillRecord bill = billingManager.generateBill(*vehicle);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "                  停车法案                " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "  车牌号: " << bill.plateNumber << std::endl;
    std::cout << "  类型: " << bill.vehicleType << std::endl;
    std::cout << "  停留时间: " << std::fixed << std::setprecision(1) 
              << bill.duration << " 小时 " << std::endl;
    std::cout << "  金额: $" << std::fixed << std::setprecision(2) 
              << bill.amount << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "\n确认付款? (1: Yes / 2: No): ";
    int confirm;
    std::cin >> confirm;
    
    if (confirm == 1) {
        billingManager.addHistory(bill);
        ParkingSpot* spot = spotManager.findSpotByPlate(plate);
        if (spot) {
            spot->release();
        }
        vehicleManager.removeVehicle(plate);
        std::cout << "\n付款成功的. 车辆离开." << std::endl;
        std::cout << "  谢谢!" << std::endl;
        saveAllData();
        std::cout << "  [Saved to " << DATA_FILE << "]" << std::endl;
    } else {
        std::cout << "\n取消了." << std::endl;
    }
    
    pauseScreen();
}

void ConsoleUI::handleQueryVehicle() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "                 车辆查询                 " << std::endl;
    std::cout << "==========================================" << std::endl;
    
    std::cout << "  1. 按车牌搜索" << std::endl;
    std::cout << "  2. 查看所有车辆" << std::endl;
    
    int choice = getIntInput("选择");
    
    if (choice == 1) {
        std::string plate = getStringInput("输入车牌号码");
        Vehicle* vehicle = vehicleManager.findByPlate(plate);
        
        if (vehicle) {
            std::cout << "\n发现车辆:" << std::endl;
            std::cout << "  车牌号码: " << vehicle->getPlate() << std::endl;
            std::cout << "  类型: " << vehicle->getType() << std::endl;
            std::cout << "  停留时间: " << std::fixed << std::setprecision(1) 
                      << vehicle->calculateParkingDuration() << " 小时 " << std::endl;
            std::cout << "  目前的费用: $" << std::fixed << std::setprecision(2) 
                      << vehicle->calculateFee() << std::endl;
        } else {
            std::cout << "\n没有发现车辆" << std::endl;
        }
    } else if (choice == 2) {
        vehicleManager.displayAllVehicles();
    }
    
    pauseScreen();
}

void ConsoleUI::handleSpotStatus() {
    spotManager.displaySpotStatus();
    pauseScreen();
}

void ConsoleUI::handleStatistics() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "                  统计数据                " << std::endl;
    std::cout << "==========================================" << std::endl;
    
    std::cout << "  现场使用情况:" << std::endl;
    std::cout << "    总车位: " << spotManager.getTotalSpots() << std::endl;
    std::cout << "    占用: " << spotManager.getOccupiedCount() << std::endl;
    std::cout << "    可用: " << spotManager.getAvailableCount() << std::endl;
    std::cout << "    预约: " << spotManager.getReservedCount() << std::endl;
    
    double occupancyRate = 0;
    if (spotManager.getTotalSpots() > 0) {
        occupancyRate = (double)spotManager.getOccupiedCount() / spotManager.getTotalSpots() * 100;
    }
    std::cout << "    入场率: " << std::fixed << std::setprecision(1) 
              << occupancyRate << "%" << std::endl;
    
    std::cout << "\n  收入:" << std::endl;
    std::cout << "    总账单: " << billingManager.getBillCount() << std::endl;
    std::cout << "    总营收: $" << std::fixed << std::setprecision(2) 
              << billingManager.getTotalRevenue() << std::endl;
    
    std::cout << "\n========================================" << std::endl;
    
    std::cout << "\n  1. 查看计费历史" << std::endl;
    std::cout << "  2. 返回菜单" << std::endl;
    
    int choice = getIntInput("选择");
    if (choice == 1) {
        billingManager.displayHistory();
    }
    
    pauseScreen();
}

void ConsoleUI::handleSettings() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "                   设置                   " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "  当前配置:" << std::endl;
    std::cout << "    轿车收费: $5.0/小时" << std::endl;
    std::cout << "    卡车收费: $10.0/小时" << std::endl;
    std::cout << "    摩托车收费: $2.5/小时" << std::endl;
    std::cout << "    预留超时：120秒" << std::endl;
    std::cout << "========================================" << std::endl;
    
    pauseScreen();
}

void ConsoleUI::displayNavigation(const std::string& spotCode) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  导航 - 车位 #" << spotCode << std::endl;
    std::cout << "========================================" << std::endl;
    
    char zone = spotCode[0];
    std::cout << "进入停车场后:" << std::endl;
    
    switch (zone) {
        case 'A':
            std::cout << "1. 左转进入区域 A" << std::endl;
            std::cout << "2. 沿主通道直行" << std::endl;
            break;
        case 'B':
            std::cout << "1. 直行进入区域 B" << std::endl;
            std::cout << "2. 沿主通道直行" << std::endl;
            break;
        case 'C':
            std::cout << "1. 右转进入区域 C" << std::endl;
            std::cout << "2. 沿主通道直行" << std::endl;
            break;
    }
    
    std::cout << "3. 到达地点 #" << spotCode << std::endl;
    std::cout << "========================================" << std::endl;
}

int ConsoleUI::getIntInput(const std::string& prompt) {
    int value;
    std::cout << prompt << ": ";
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "输入无效，请输入一个数字: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

std::string ConsoleUI::getStringInput(const std::string& prompt) {
    std::string value;
    std::cout << prompt << ": ";
    std::getline(std::cin, value);
    return value;
}

void ConsoleUI::pauseScreen() {
    std::cout << "\n按 Enter 继续...";
    std::cin.get();
}

void ConsoleUI::loadSavedData() {
    if (!FileManager::fileExists(DATA_FILE)) {
        return;
    }

    auto spotRecords = FileManager::loadSpots(DATA_FILE);
    parkingLot.importSpots(spotRecords);

    auto vehicleRecords = FileManager::loadVehicles(DATA_FILE);
    for (const auto& vr : vehicleRecords) {
        std::unique_ptr<Vehicle> v;
        if (vr.vehicleType == "轿车") {
            v = std::make_unique<Car>(vr.plateNumber);
        } else if (vr.vehicleType == "卡车") {
            v = std::make_unique<Truck>(vr.plateNumber);
        } else if (vr.vehicleType == "摩托车") {
            v = std::make_unique<Motorcycle>(vr.plateNumber);
        }
        if (v) {
            auto tp = std::chrono::system_clock::time_point(
                std::chrono::seconds(vr.entryTimestamp));
            v->setEntryTime(tp);
            vehicleManager.registerVehicle(std::move(v));
        }
    }

    auto billRecords = FileManager::loadBillHistory(DATA_FILE);
    billingManager.importHistory(billRecords);

    if (!spotRecords.empty() || !vehicleRecords.empty() || !billRecords.empty()) {
        std::cout << "[Data loaded from " << DATA_FILE << "]" << std::endl;
    }
}

void ConsoleUI::saveAllData() {
    auto spotRecords = parkingLot.exportSpots();
    FileManager::saveSpots(DATA_FILE, spotRecords);

    auto vehicleRecords = vehicleManager.exportVehicles();
    FileManager::saveVehicles(DATA_FILE, vehicleRecords);

    auto billRecords = billingManager.exportHistory();
    FileManager::saveBillHistory(DATA_FILE, billRecords);
}
