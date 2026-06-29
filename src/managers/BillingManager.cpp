#include "managers/BillingManager.h"
#include "utils/RatePolicy.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>

namespace {
    // 判断当前结算时刻是否处于夜间时段（22:00-06:00），夜间轿车享受折扣
    bool isNightTime() {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm local{};
#ifdef _WIN32
        localtime_s(&local, &t);
#else
        localtime_r(&t, &local);
#endif
        return local.tm_hour >= 22 || local.tm_hour < 6;
    }
}

BillingManager::BillingManager() {}

double BillingManager::calculateFee(const Vehicle& vehicle) {
    double hours = vehicle.calculateParkingDuration();

    // 按车型分派到对应的计费策略（RatePolicy 模板特化）：
    //   轿车   —— 夜间 3 元/小时，日间 5 元/小时
    //   卡车   —— 不足 1 小时按 1 小时计，10 元/小时
    //   摩托车 —— 2.5 元/小时
    if (dynamic_cast<const Car*>(&vehicle)) {
        return RatePolicy<Car>::calculateFee(hours, isNightTime());
    }
    if (dynamic_cast<const Truck*>(&vehicle)) {
        return RatePolicy<Truck>::calculateFee(hours);
    }
    if (dynamic_cast<const Motorcycle*>(&vehicle)) {
        return RatePolicy<Motorcycle>::calculateFee(hours);
    }
    return vehicle.calculateFee();  // 兜底：未知车型按基础费率
}

BillRecord BillingManager::generateBill(const Vehicle& vehicle) {
    BillRecord record;
    record.plateNumber = vehicle.getPlate();
    record.vehicleType = vehicle.getType();
    record.duration = vehicle.calculateParkingDuration();
    record.amount = calculateFee(vehicle);
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    record.exitTime = oss.str();
    
    return record;
}

void BillingManager::addHistory(const BillRecord& record) {
    history.push_back(record);
}

void BillingManager::displayHistory() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "               账单历史                   " << std::endl;
    std::cout << "==========================================" << std::endl;
    
    if (history.empty()) {
        std::cout << "  无记录" << std::endl;
    } else {
        for (const auto& record : history) {
            std::cout << "  车牌号: " << record.plateNumber << std::endl;
            std::cout << "  类型: " << record.vehicleType << std::endl;
            std::cout << "  停留时间: " << std::fixed << std::setprecision(1) 
                      << record.duration << " hours" << std::endl;
            std::cout << "  金额: $" << std::fixed << std::setprecision(2) 
                      << record.amount << std::endl;
            std::cout << "  退出时间: " << record.exitTime << std::endl;
            std::cout << "  ----------------------------------------" << std::endl;
        }
    }
    
    std::cout << "========================================\n" << std::endl;
}

double BillingManager::getTotalRevenue() const {
    double total = 0;
    for (const auto& record : history) {
        total += record.amount;
    }
    return total;
}

size_t BillingManager::getBillCount() const {
    return history.size();
}

std::vector<BillRecordData> BillingManager::exportHistory() const {
    std::vector<BillRecordData> records;
    for (const auto& h : history) {
        BillRecordData r;
        r.plateNumber = h.plateNumber;
        r.vehicleType = h.vehicleType;
        r.duration = h.duration;
        r.amount = h.amount;
        r.exitTime = h.exitTime;
        records.push_back(r);
    }
    return records;
}

void BillingManager::importHistory(const std::vector<BillRecordData>& records) {
    for (const auto& r : records) {
        BillRecord h;
        h.plateNumber = r.plateNumber;
        h.vehicleType = r.vehicleType;
        h.duration = r.duration;
        h.amount = r.amount;
        h.exitTime = r.exitTime;
        history.push_back(h);
    }
}
