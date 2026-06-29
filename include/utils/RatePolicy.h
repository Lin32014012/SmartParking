#pragma once

#include "core/Vehicle.h"
#include <cmath>

// 计费策略模板：默认按 "时长 × 基础费率" 计算。
// 通过模板特化为不同车型提供差异化计费规则。
template<typename VehicleType>
class RatePolicy {
public:
    static double calculateFee(double hours) {
        return hours * VehicleType::BASE_RATE;
    }
};

// 轿车：夜间（22:00-06:00）享受折扣价 3 元/小时，日间 5 元/小时
template<>
class RatePolicy<Car> {
public:
    static double calculateFee(double hours, bool isNight = false) {
        double rate = isNight ? 3.0 : 5.0;
        return hours * rate;
    }
};

// 卡车：不足 1 小时按 1 小时计（向上取整），10 元/小时
template<>
class RatePolicy<Truck> {
public:
    static double calculateFee(double hours) {
        double billableHours = std::ceil(hours);
        return billableHours * 10.0;
    }
};

// 摩托车：2.5 元/小时
template<>
class RatePolicy<Motorcycle> {
public:
    static double calculateFee(double hours) {
        return hours * 2.5;
    }
};
