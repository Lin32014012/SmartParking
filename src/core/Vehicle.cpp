#include "core/Vehicle.h"

Vehicle::Vehicle(const std::string& plate)
: plateNumber(plate), entryTime(std::chrono::system_clock::now()) {}//构造函数 获取当前系统时间作为入场时间

void Vehicle::setEntryTime(std::chrono::system_clock::time_point time) {
    entryTime = time;
}//保存的时间戳还原为time_point

std::string Vehicle::getPlate() const {
    return plateNumber;
}//获取车牌，返回车牌号

std::chrono::system_clock::time_point Vehicle::getEntryTime() const {
    return entryTime;
}

double Vehicle::calculateParkingDuration() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - entryTime);
    return duration.count() / 60.0;
}

double Vehicle::calculateFee() const {
    double hours = calculateParkingDuration();
    return hours * getHourlyRate();//getHourlyRate() 是虚函数，调用时自动分发到子类版本，实现多态计费
}

Car::Car(const std::string& plate) : Vehicle(plate) {}//轿车构造函数

double Car::getHourlyRate() const {
    return 5.0;
}//轿车类调用getHourlyRate,返回5

std::string Car::getType() const {
    return "轿车";
}

Truck::Truck(const std::string& plate) : Vehicle(plate) {}//卡车构造函数

double Truck::getHourlyRate() const {
    return 10.0;
}//卡车类调用getHourlyRate,返回10

std::string Truck::getType() const {
    return "卡车";
}

Motorcycle::Motorcycle(const std::string& plate) : Vehicle(plate) {}//摩托车构造函数

double Motorcycle::getHourlyRate() const {
    return 2.5;
}//摩托车类调用getHourlyRate,返回2.5

std::string Motorcycle::getType() const {
    return "摩托车";
}
