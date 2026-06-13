#pragma once

#include <vector>
#include <memory>
#include <functional>

template<typename T>
class Filter {
public:
    using Predicate = std::function<bool(const T&)>;
    
    static std::vector<T*> apply(
        std::vector<std::unique_ptr<T>>& source,
        Predicate pred
    ) {
        std::vector<T*> result;
        for (auto& item : source) {
            if (pred(*item)) {
                result.push_back(item.get());
            }
        }
        return result;
    }
};
