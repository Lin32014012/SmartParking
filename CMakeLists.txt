cmake_minimum_required(VERSION 3.16)
project(SmartParking VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

set(SOURCES
    src/core/Vehicle.cpp
    src/core/ParkingSpot.cpp
    src/core/ParkingLot.cpp
    src/managers/VehicleManager.cpp
    src/managers/SpotManager.cpp
    src/managers/BillingManager.cpp
    src/utils/FileManager.cpp
    src/ui/ConsoleUI.cpp
    main.cpp
)

add_executable(SmartParking ${SOURCES})
set_target_properties(SmartParking PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/..")

file(COPY data DESTINATION ${CMAKE_BINARY_DIR})

add_compile_options(/utf-8)
