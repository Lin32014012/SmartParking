#pragma once

#include <map>
#include <vector>
#include <numeric>

// 通用统计收集器模板：按类别 T 聚合数值 ValueType，
// 支持求和、求平均、按类别计数。用于车型分布等统计场景。
template<typename T, typename ValueType>
class StatisticsCollector {
    std::map<T, std::vector<ValueType>> data;

public:
    void addData(T category, ValueType value) {
        data[category].push_back(value);
    }

    double getAverage(T category) const {
        auto it = data.find(category);
        if (it == data.end() || it->second.empty()) return 0.0;

        double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
        return sum / it->second.size();
    }

    ValueType getTotal(T category) const {
        auto it = data.find(category);
        if (it == data.end()) return ValueType{};

        return std::accumulate(it->second.begin(), it->second.end(), ValueType{});
    }

    std::vector<T> getCategories() const {
        std::vector<T> categories;
        for (const auto& pair : data) {
            categories.push_back(pair.first);
        }
        return categories;
    }

    size_t getCategoryCount(T category) const {
        auto it = data.find(category);
        return (it != data.end()) ? it->second.size() : 0;
    }
};
