#pragma once

#include "core/Vehicle.h"
#include "utils/FileManager.h"
#include <string>
#include <vector>

struct BillRecord {
    std::string plateNumber;
    std::string vehicleType;
    double duration;
    double amount;
    std::string exitTime;
};

class BillingManager {
    std::vector<BillRecord> history;
    
public:
    BillingManager();
    
    double calculateFee(const Vehicle& vehicle);
    BillRecord generateBill(const Vehicle& vehicle);
    
    void addHistory(const BillRecord& record);
    void displayHistory() const;
    
    double getTotalRevenue() const;
    size_t getBillCount() const;
    
    std::vector<BillRecordData> exportHistory() const;
    void importHistory(const std::vector<BillRecordData>& records);
};
