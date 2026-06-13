#pragma once

#include <vector>
#include <map>
#include <memory>
#include <string>
#include <algorithm>
#include <functional>

template<typename T>
class EntityManager {
    std::vector<std::unique_ptr<T>> entities;
    std::map<std::string, T*> indexByPlate;
    
public:
    EntityManager() = default;
    
    void add(std::unique_ptr<T> entity) {
        std::string key = entity->getPlate();
        T* ptr = entity.get();
        indexByPlate[key] = ptr;
        entities.push_back(std::move(entity));
    }
    
    T* findByPlate(const std::string& plate) {
        auto it = indexByPlate.find(plate);
        return (it != indexByPlate.end()) ? it->second : nullptr;
    }
    
    bool remove(const std::string& plate) {
        auto it = std::find_if(entities.begin(), entities.end(),
            [&plate](const std::unique_ptr<T>& e) {
                return e->getPlate() == plate;
            });
        
        if (it != entities.end()) {
            indexByPlate.erase(plate);
            entities.erase(it);
            return true;
        }
        return false;
    }
    
    size_t count() const {
        return entities.size();
    }

    template<typename Func>
    void forEach(Func func) {
        for (auto& entity : entities) {
            func(*entity);
        }
    }

    template<typename Func>
    void forEach(Func func) const {
        for (const auto& entity : entities) {
            func(*entity);
        }
    }
};
