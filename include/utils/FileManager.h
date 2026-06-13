#pragma once

#include <string>
#include <vector>
#include <fstream>

struct SpotRecord {
    int spotId;
    std::string zone;
    int row;
    int position;
    std::string spotType;
    std::string status;
    std::string reservedPlate;
};

struct VehicleRecord {
    std::string plateNumber;
    std::string vehicleType;
    long long entryTimestamp;
};

struct BillRecordData {
    std::string plateNumber;
    std::string vehicleType;
    double duration;
    double amount;
    std::string exitTime;
};

class FileManager {
public:
    static bool saveSpots(const std::string& filename, const std::vector<SpotRecord>& spots);
    static std::vector<SpotRecord> loadSpots(const std::string& filename);

    static bool saveVehicles(const std::string& filename, const std::vector<VehicleRecord>& vehicles);
    static std::vector<VehicleRecord> loadVehicles(const std::string& filename);

    static bool saveBillHistory(const std::string& filename, const std::vector<BillRecordData>& records);
    static std::vector<BillRecordData> loadBillHistory(const std::string& filename);

    static bool fileExists(const std::string& filename);
};
