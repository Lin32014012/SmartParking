// 并发压力测试：多线程同时抢占车位，验证互斥锁保证「无重复分配」。
// 若 ParkingLot::parkVehicle 未加锁，多个线程会把同一个车位分配给不同车牌，
// 本测试就会检测到「重复分配」或「成功数 != 容量」而失败。
#include "core/ParkingLot.h"
#include "core/Vehicle.h"

#include <atomic>
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <utility>
#include <vector>

int main() {
    const int CAPACITY    = 100;   // 100 个小车位
    const int THREADS     = 16;    // 16 个线程并发抢占
    const int PER_THREAD  = 50;    // 每线程尝试 50 次 -> 800 次请求争抢 100 个车位

    ParkingLot lot;
    lot.initializeSpots(CAPACITY, 0, 0);

    std::atomic<int> successCount{0};
    std::mutex resultMutex;
    std::vector<std::pair<int, std::string>> allocations;  // spotId -> plate

    std::vector<std::thread> threads;
    threads.reserve(THREADS);
    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < PER_THREAD; ++i) {
                std::string plate = "T" + std::to_string(t) + "-" + std::to_string(i);
                Car car(plate);
                ParkingSpot* spot = lot.parkVehicle(car);
                if (spot) {
                    successCount.fetch_add(1, std::memory_order_relaxed);
                    std::lock_guard<std::mutex> lg(resultMutex);
                    allocations.emplace_back(spot->getSpotId(), plate);
                }
            }
        });
    }
    for (auto& th : threads) th.join();

    // 校验：每个车位至多被分配一次
    std::set<int> usedSpots;
    bool duplicate = false;
    for (const auto& a : allocations) {
        if (!usedSpots.insert(a.first).second) duplicate = true;
    }

    const int success  = successCount.load();
    const int occupied = lot.getOccupiedCount();

    std::cout << "[concurrency] threads=" << THREADS
              << " requests=" << THREADS * PER_THREAD
              << " success=" << success
              << " occupied=" << occupied
              << " uniqueSpots=" << usedSpots.size() << "\n";

    int failures = 0;
    if (duplicate) {
        std::cout << "  [FAIL] 同一车位被重复分配\n";
        ++failures;
    }
    if (success != CAPACITY) {
        std::cout << "  [FAIL] 成功分配数 " << success
                  << " != 容量 " << CAPACITY << "\n";
        ++failures;
    }
    if (occupied != CAPACITY) {
        std::cout << "  [FAIL] 占用数 " << occupied
                  << " != 容量 " << CAPACITY << "\n";
        ++failures;
    }
    if (static_cast<int>(usedSpots.size()) != success) {
        std::cout << "  [FAIL] 去重后车位数 " << usedSpots.size()
                  << " != 成功数 " << success << "\n";
        ++failures;
    }

    if (failures == 0) {
        std::cout << "[concurrency] ALL PASS: " << THREADS
                  << " 线程并发下无重复分配，资源分配原子且安全\n";
        return 0;
    }
    return 1;
}
