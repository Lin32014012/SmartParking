#include "utils/FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool FileManager::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

bool FileManager::saveSpots(const std::string& filename, const std::vector<SpotRecord>& spots) {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    file << "[SPOTS]" << std::endl;
    for (const auto& spot : spots) {
        file << spot.spotId << "|"
             << spot.zone << "|"
             << spot.row << "|"
             << spot.position << "|"
             << spot.spotType << "|"
             << spot.status << "|"
             << spot.reservedPlate << std::endl;
    }

    file.close();
    return true;
}

std::vector<SpotRecord> FileManager::loadSpots(const std::string& filename) {
    std::vector<SpotRecord> spots;
    std::ifstream file(filename);

    if (!file.is_open()) {
        return spots;
    }

    std::string line;
    bool inSection = false;

    while (std::getline(file, line)) {
        if (line == "[SPOTS]") {
            inSection = true;
            continue;
        }
        if (line.rfind("[", 0) == 0) {
            inSection = false;
            continue;
        }

        if (inSection && !line.empty()) {
            std::istringstream iss(line);
            std::string token;
            SpotRecord record;

            std::getline(iss, token, '|');
            record.spotId = std::stoi(token);
            std::getline(iss, record.zone, '|');
            std::getline(iss, token, '|');
            record.row = std::stoi(token);
            std::getline(iss, token, '|');
            record.position = std::stoi(token);
            std::getline(iss, record.spotType, '|');
            std::getline(iss, record.status, '|');
            std::getline(iss, record.reservedPlate, '|');

            spots.push_back(record);
        }
    }

    file.close();
    return spots;
}

bool FileManager::saveVehicles(const std::string& filename, const std::vector<VehicleRecord>& vehicles) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        return false;
    }

    file << "[VEHICLES]" << std::endl;
    for (const auto& v : vehicles) {
        file << v.plateNumber << "|"
             << v.vehicleType << "|"
             << v.entryTimestamp << std::endl;
    }

    file.close();
    return true;
}

std::vector<VehicleRecord> FileManager::loadVehicles(const std::string& filename) {
    std::vector<VehicleRecord> vehicles;
    std::ifstream file(filename);

    if (!file.is_open()) {
        return vehicles;
    }

    std::string line;
    bool inSection = false;

    while (std::getline(file, line)) {
        if (line == "[VEHICLES]") {
            inSection = true;
            continue;
        }
        if (line.rfind("[", 0) == 0) {
            inSection = false;
            continue;
        }

        if (inSection && !line.empty()) {
            std::istringstream iss(line);
            VehicleRecord record;

            std::getline(iss, record.plateNumber, '|');
            std::getline(iss, record.vehicleType, '|');

            std::string token;
            std::getline(iss, token, '|');
            record.entryTimestamp = std::stoll(token);

            vehicles.push_back(record);
        }
    }

    file.close();
    return vehicles;
}

bool FileManager::saveBillHistory(const std::string& filename, const std::vector<BillRecordData>& records) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        return false;
    }

    file << "[BILLING]" << std::endl;
    for (const auto& r : records) {
        file << r.plateNumber << "|"
             << r.vehicleType << "|"
             << r.duration << "|"
             << r.amount << "|"
             << r.exitTime << std::endl;
    }

    file.close();
    return true;
}

std::vector<BillRecordData> FileManager::loadBillHistory(const std::string& filename) {
    std::vector<BillRecordData> records;
    std::ifstream file(filename);

    if (!file.is_open()) {
        return records;
    }

    std::string line;
    bool inSection = false;

    while (std::getline(file, line)) {
        if (line == "[BILLING]") {
            inSection = true;
            continue;
        }
        if (line.rfind("[", 0) == 0) {
            inSection = false;
            continue;
        }

        if (inSection && !line.empty()) {
            std::istringstream iss(line);
            BillRecordData record;

            std::getline(iss, record.plateNumber, '|');
            std::getline(iss, record.vehicleType, '|');

            std::string token;
            std::getline(iss, token, '|');
            record.duration = std::stod(token);
            std::getline(iss, token, '|');
            record.amount = std::stod(token);
            std::getline(iss, record.exitTime, '|');

            records.push_back(record);
        }
    }

    file.close();
    return records;
}
