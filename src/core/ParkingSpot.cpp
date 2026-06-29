#include "core/ParkingSpot.h"
#include "core/Vehicle.h"
#include <sstream>// 用于字符串流拼接 SpotCode
#include <iomanip>// 用于 setw、setfill 格式化

bool Reservation::isExpired() const {//判断预留是否超时
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - reservedAt);
    return duration.count() > TIMEOUT_SECONDS;
}

ParkingSpot::ParkingSpot(int id, const std::string& z, int r, int p)//构造函数
    : spotId(id), zone(z), row(r), position(p), status(SpotStatus::Empty) {}

bool ParkingSpot::isEmpty() const {//空闲
    return status == SpotStatus::Empty;
}

bool ParkingSpot::isReserved() const {//预留
    return status == SpotStatus::Reserved;
}

bool ParkingSpot::isOccupied() const {//占用
    return status == SpotStatus::Occupied;
}

bool ParkingSpot::reserve(const std::string& plate) {//预留车位
    if (status != SpotStatus::Empty) {
        return false;// 只有空闲状态才能预留
    }
    
    reservation = Reservation{plate, std::chrono::system_clock::now()};
    status = SpotStatus::Reserved;
    return true;
}

bool ParkingSpot::occupy(const std::string& plate) {//占用车位
    if (status == SpotStatus::Reserved && reservation.has_value()) {
        if (reservation->plateNumber == plate) {
            occupiedPlate = plate;
            reservation.reset();
            status = SpotStatus::Occupied;
            return true;
        }
    }
    return false;
}

void ParkingSpot::release() {//释放车位
    reservation.reset();
    occupiedPlate.clear();
    status = SpotStatus::Empty;
}

void ParkingSpot::checkReservationTimeout() {//预留超时检查函数，用于自动释放超时未到达的预留车位
    if (status == SpotStatus::Reserved && reservation.has_value()) {
        if (reservation->isExpired()) {
            release();
        }
    }
}

std::string ParkingSpot::getSpotCode() const {//生成车位编码
    std::ostringstream oss;
    oss << zone << "-" << std::setw(3) << std::setfill('0') << spotId;
    return oss.str();
}

int ParkingSpot::getSpotId() const {//车位id
    return spotId;
}

std::string ParkingSpot::getZone() const {//车位区域
    return zone;
}

int ParkingSpot::getRow() const {
    return row;
}

int ParkingSpot::getPosition() const {
    return position;
}

SpotStatus ParkingSpot::getStatus() const {//车位状态
    return status;
}

std::string ParkingSpot::getPlate() const {//获取当前车牌
    if (status == SpotStatus::Reserved && reservation.has_value()) {
        return reservation->plateNumber;// 预留状态：返回预留车牌
    }
    if (status == SpotStatus::Occupied) {
        return occupiedPlate;// 占用状态：返回占用车牌
    }
    return "";// 空闲状态：返回空字符串
}

SmallSpot::SmallSpot(int id, const std::string& z, int r, int p)
    : ParkingSpot(id, z, r, p) {}// 委托给基类构造

bool SmallSpot::canFitVehicle(const Vehicle& vehicle) const {
    return vehicle.getType() == "轿车";// 只能停轿车
}

std::string SmallSpot::getSpotType() const {
    return "小车位";
}

LargeSpot::LargeSpot(int id, const std::string& z, int r, int p)
    : ParkingSpot(id, z, r, p) {}

bool LargeSpot::canFitVehicle(const Vehicle& vehicle) const {
    return vehicle.getType() == "卡车";// 只能停卡车
}

std::string LargeSpot::getSpotType() const {
    return "大车位";
}

MCSpot::MCSpot(int id, const std::string& z, int r, int p)
    : ParkingSpot(id, z, r, p) {}

bool MCSpot::canFitVehicle(const Vehicle& vehicle) const {
    return vehicle.getType() == "摩托车";// 只能停摩托车
}

std::string MCSpot::getSpotType() const {
    return "摩托车车位";
}
