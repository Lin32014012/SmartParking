#include "core/ParkingLot.h"
#include "core/Vehicle.h"
#include <iostream>
#include <algorithm>

ParkingLot::ParkingLot() : totalSpots(0) {}//构造函数，初始化totalSpots为0

void ParkingLot::initializeSpots(int smallCount, int largeCount, int mcCount) {//初始化车位
    spots.clear();// 清空旧数据
    int id = 1;// 车位 ID 从 1 开始
    
    for (int i = 0; i < smallCount; i++) {
        int row = (i / 10) + 1;
        int pos = (i % 10) + 1;
        spots.push_back(std::make_unique<SmallSpot>(id++, "A", row, pos));
    }

    for (int i = 0; i < largeCount; i++) {
        int row = (i / 5) + 1;
        int pos = (i % 5) + 1;
        spots.push_back(std::make_unique<LargeSpot>(id++, "B", row, pos));
    }

    for (int i = 0; i < mcCount; i++) {
        int row = (i / 5) + 1;
        int pos = (i % 5) + 1;
        spots.push_back(std::make_unique<MCSpot>(id++, "C", row, pos));
    }
    
    totalSpots = spots.size();
}

ParkingSpot* ParkingLot::findAvailableSpot(const Vehicle& vehicle) {//查找空闲车位
    for (auto& spot : spots) {
        if (spot->isEmpty() && spot->canFitVehicle(vehicle)) {
            return spot.get();// 返回裸指针（不转移所有权）
        }
    }
    return nullptr;// 没找到
}

ParkingSpot* ParkingLot::allocateSpot(const Vehicle& vehicle) {//分配车位（仅预留）
    std::lock_guard<std::mutex> lock(allocMutex);// 查找空位+预留为原子操作
    ParkingSpot* spot = findAvailableSpot(vehicle);
    if (spot) {
        spot->reserve(vehicle.getPlate());// 设为 Reserved 状态
        return spot;
    }
    return nullptr;
}

ParkingSpot* ParkingLot::parkVehicle(const Vehicle& vehicle) {//线程安全：查找+预留+占用一气呵成
    std::lock_guard<std::mutex> lock(allocMutex);
    ParkingSpot* spot = findAvailableSpot(vehicle);
    if (spot) {
        spot->reserve(vehicle.getPlate());
        spot->occupy(vehicle.getPlate());// 在同一把锁内完成占用，避免并发读写 status 的竞争
        return spot;
    }
    return nullptr;
}

bool ParkingLot::releaseSpot(int spotId) {//释放车位
    std::lock_guard<std::mutex> lock(allocMutex);
    for (auto& spot : spots) {
        if (spot->getSpotId() == spotId) {
            spot->release();// Occupied/Reserved → Empty
            return true;
        }
    }
    return false;
}

void ParkingLot::checkAllTimeouts() {//超时检查
    for (auto& spot : spots) {
        spot->checkReservationTimeout();// 检查是否超过120秒
    }
}

int ParkingLot::getTotalSpots() const {
    return totalSpots;
}
//状态查询方法
int ParkingLot::getAvailableCount() const {//可用
    int count = 0;
    for (const auto& spot : spots) {
        if (spot->isEmpty()) {
            count++;
        }
    }
    return count;
}

int ParkingLot::getOccupiedCount() const {//占用
    int count = 0;
    for (const auto& spot : spots) {
        if (spot->isOccupied()) {
            count++;
        }
    }
    return count;
}

int ParkingLot::getReservedCount() const {//预约
    int count = 0;
    for (const auto& spot : spots) {
        if (spot->isReserved()) {
            count++;
        }
    }
    return count;
}

void ParkingLot::displaySpotStatus() const {//显示车位状态
    std::cout << "\n========================================" << std::endl;
    std::cout << "                  停车位状态              " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "总车位: " << totalSpots << std::endl;
    std::cout << "可用: " << getAvailableCount() << " | ";
    std::cout << "占用: " << getOccupiedCount() << " | ";
    std::cout << "预约: " << getReservedCount() << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::cout << "车位细节:" << std::endl;
    for (const auto& spot : spots) {
        std::string statusStr;
        switch (spot->getStatus()) {
            case SpotStatus::Empty: statusStr = "空"; break;
            case SpotStatus::Reserved: statusStr = "预约"; break;
            case SpotStatus::Occupied: statusStr = "占用"; break;
        }
        std::cout << "  " << spot->getSpotCode() << " [" << spot->getSpotType() << "] " << statusStr << std::endl;
    }
}

std::vector<ParkingSpot*> ParkingLot::getAllSpots() const {//获取所有车位指针
    std::vector<ParkingSpot*> result;
    for (const auto& spot : spots) {
        result.push_back(spot.get());// 裸指针，不转移所有权
    }
    return result;
}

std::vector<SpotRecord> ParkingLot::exportSpots() const {// 导出车位数据
    std::vector<SpotRecord> records;
    for (const auto& spot : spots) {
        SpotRecord r;
        r.spotId = spot->getSpotId();
        r.zone = spot->getZone();
        r.spotType = spot->getSpotType();
        r.row = spot->getRow();
        r.position = spot->getPosition();

        switch (spot->getStatus()) {
            case SpotStatus::Empty: r.status = "空"; break;
            case SpotStatus::Reserved: r.status = "预约"; break;
            case SpotStatus::Occupied: r.status = "占用"; break;
        }

        r.reservedPlate = spot->getPlate();// 保存当前车牌
        records.push_back(r);
    }
    return records;
}

void ParkingLot::importSpots(const std::vector<SpotRecord>& records) {//导入车位数据
    for (const auto& r : records) {
        ParkingSpot* spot = findSpotById(r.spotId);
        if (!spot) continue;

        if (r.status == "预约" && !r.reservedPlate.empty()) {
            spot->reserve(r.reservedPlate);// 恢复预留状态
        } else if (r.status == "占用" && !r.reservedPlate.empty()) {
            spot->reserve(r.reservedPlate);// 先预留
            spot->occupy(r.reservedPlate);// 再占用
        }
    }
}

ParkingSpot* ParkingLot::findSpotById(int spotId) {//查找车位
    for (auto& spot : spots) {
        if (spot->getSpotId() == spotId) {
            return spot.get();
        }
    }
    return nullptr;
}

ParkingSpot* ParkingLot::findSpotByPlate(const std::string& plate) {
    for (auto& spot : spots) {
        if (spot->getPlate() == plate) {
            return spot.get();
        }
    }
    return nullptr;
}
