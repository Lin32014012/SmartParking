// 核心业务逻辑单元测试（不依赖 Qt，可独立编译运行）
#include "core/Vehicle.h"
#include "core/ParkingSpot.h"
#include "core/ParkingLot.h"
#include "managers/VehicleManager.h"
#include "managers/BillingManager.h"
#include "utils/RatePolicy.h"

#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <iostream>

static int g_total = 0;
static int g_fail = 0;

#define CHECK(cond)                                                       \
    do {                                                                  \
        ++g_total;                                                        \
        if (!(cond)) {                                                    \
            ++g_fail;                                                     \
            std::cout << "  [FAIL] " << #cond                             \
                      << "  (line " << __LINE__ << ")\n";                 \
        }                                                                 \
    } while (0)

#define CHECK_NEAR(a, b, eps)                                             \
    do {                                                                  \
        ++g_total;                                                        \
        if (std::fabs((a) - (b)) > (eps)) {                              \
            ++g_fail;                                                     \
            std::cout << "  [FAIL] " << #a << " ~= " << #b                \
                      << "  got " << (a) << " vs " << (b)                 \
                      << "  (line " << __LINE__ << ")\n";                 \
        }                                                                 \
    } while (0)

static void test_vehicle() {
    Car c("c"); Truck t("t"); Motorcycle m("m");
    CHECK(c.getType() == "轿车");
    CHECK(t.getType() == "卡车");
    CHECK(m.getType() == "摩托车");
    CHECK_NEAR(c.getHourlyRate(), 5.0, 1e-9);
    CHECK_NEAR(t.getHourlyRate(), 10.0, 1e-9);
    CHECK_NEAR(m.getHourlyRate(), 2.5, 1e-9);
}

static void test_rate_policy() {
    // 轿车：日间 5 / 夜间 3
    CHECK_NEAR(RatePolicy<Car>::calculateFee(2.0, false), 10.0, 1e-9);
    CHECK_NEAR(RatePolicy<Car>::calculateFee(2.0, true), 6.0, 1e-9);
    // 卡车：向上取整 * 10
    CHECK_NEAR(RatePolicy<Truck>::calculateFee(2.3), 30.0, 1e-9);
    CHECK_NEAR(RatePolicy<Truck>::calculateFee(3.0), 30.0, 1e-9);
    // 摩托车：2.5 / 小时
    CHECK_NEAR(RatePolicy<Motorcycle>::calculateFee(2.0), 5.0, 1e-9);
}

static void test_spot_state_machine() {
    SmallSpot s(1, "A", 1, 1);
    CHECK(s.isEmpty());
    CHECK(s.reserve("京A"));
    CHECK(s.isReserved());
    CHECK(!s.reserve("京B"));      // 已预留，不可再预留
    CHECK(!s.occupy("京B"));       // 车牌不匹配，不可占用
    CHECK(s.occupy("京A"));
    CHECK(s.isOccupied());
    CHECK(s.getPlate() == "京A");
    s.release();
    CHECK(s.isEmpty());
    CHECK(s.getPlate().empty());
}

static void test_can_fit() {
    SmallSpot ss(1, "A", 1, 1);
    LargeSpot ls(2, "B", 1, 1);
    Car c("c"); Truck t("t");
    CHECK(ss.canFitVehicle(c));
    CHECK(!ss.canFitVehicle(t));
    CHECK(ls.canFitVehicle(t));
    CHECK(!ls.canFitVehicle(c));
}

static void test_parking_lot() {
    ParkingLot lot;
    lot.initializeSpots(2, 0, 0);        // 仅 2 个小车位
    CHECK(lot.getTotalSpots() == 2);
    CHECK(lot.getAvailableCount() == 2);

    Car c1("c1"), c2("c2"), c3("c3");
    CHECK(lot.parkVehicle(c1) != nullptr);
    CHECK(lot.parkVehicle(c2) != nullptr);
    CHECK(lot.parkVehicle(c3) == nullptr);   // 已满
    CHECK(lot.getOccupiedCount() == 2);
    CHECK(lot.getAvailableCount() == 0);

    ParkingLot lot2;
    lot2.initializeSpots(2, 0, 0);
    Truck t("t1");
    CHECK(lot2.parkVehicle(t) == nullptr);   // 卡车停不进小车位
}

static void test_entity_manager() {
    VehicleManager vm;
    CHECK(vm.registerVehicle(std::make_unique<Car>("A")));
    CHECK(!vm.registerVehicle(std::make_unique<Car>("A")));   // 重复车牌
    CHECK(vm.registerVehicle(std::make_unique<Truck>("B")));
    CHECK(vm.getVehicleCount() == 2);
    CHECK(vm.findByPlate("A") != nullptr);
    CHECK(vm.findByPlate("Z") == nullptr);
    CHECK(vm.removeVehicle("A"));
    CHECK(!vm.removeVehicle("A"));
    CHECK(vm.getVehicleCount() == 1);
}

static void test_billing_dispatch() {
    BillingManager bm;
    // 卡车：向上取整计费
    Truck tk("tk");
    tk.setEntryTime(std::chrono::system_clock::now() - std::chrono::minutes(130));
    double dt = tk.calculateParkingDuration();
    CHECK_NEAR(bm.calculateFee(tk), std::ceil(dt) * 10.0, 0.01);
    // 摩托车：线性计费
    Motorcycle mc("mc");
    mc.setEntryTime(std::chrono::system_clock::now() - std::chrono::minutes(60));
    double dm = mc.calculateParkingDuration();
    CHECK_NEAR(bm.calculateFee(mc), dm * 2.5, 0.01);
}

int main() {
    std::cout << "[unit] running core unit tests...\n";
    test_vehicle();
    test_rate_policy();
    test_spot_state_machine();
    test_can_fit();
    test_parking_lot();
    test_entity_manager();
    test_billing_dispatch();

    std::cout << "[unit] " << (g_total - g_fail) << "/" << g_total << " checks passed\n";
    if (g_fail == 0) {
        std::cout << "[unit] ALL PASS\n";
        return 0;
    }
    std::cout << "[unit] " << g_fail << " FAILED\n";
    return 1;
}
