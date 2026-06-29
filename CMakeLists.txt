cmake_minimum_required(VERSION 3.16)
project(SmartParking VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 默认指向本机 Qt 安装路径；可在命令行用 -DCMAKE_PREFIX_PATH=... 覆盖
if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/mingw_64")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# MSVC 需要显式指定 UTF-8 源码/执行字符集；MinGW/GCC 默认即为 UTF-8。
# 注意必须在 add_executable 之前调用才会对目标生效。
if(MSVC)
    add_compile_options(/utf-8)
endif()

include_directories(include ui)

set(CORE_SOURCES
    src/core/Vehicle.cpp
    src/core/ParkingSpot.cpp
    src/core/ParkingLot.cpp
)

set(MANAGER_SOURCES
    src/managers/VehicleManager.cpp
    src/managers/SpotManager.cpp
    src/managers/BillingManager.cpp
)

set(UTIL_SOURCES
    src/utils/FileManager.cpp
)

set(UI_SOURCES
    ui/MainWindow.cpp
    ui/ParkingLotWidget.cpp
    ui/VehiclePanel.cpp
    ui/StatisticsPanel.cpp
)

add_executable(SmartParkingQt
    main_qt.cpp
    ${CORE_SOURCES}
    ${MANAGER_SOURCES}
    ${UTIL_SOURCES}
    ${UI_SOURCES}
)

target_link_libraries(SmartParkingQt Qt6::Core Qt6::Gui Qt6::Widgets)

set_target_properties(SmartParkingQt PROPERTIES
    WIN32_EXECUTABLE TRUE
)

file(COPY data DESTINATION ${CMAKE_BINARY_DIR})

# ---------------- 测试（CTest）----------------
# 后端逻辑（core/managers/utils）不依赖 Qt，可独立编译为测试可执行文件
enable_testing()
find_package(Threads REQUIRED)

set(BACKEND_SOURCES ${CORE_SOURCES} ${MANAGER_SOURCES} ${UTIL_SOURCES})

add_executable(test_core tests/test_core.cpp ${BACKEND_SOURCES})
set_target_properties(test_core PROPERTIES AUTOMOC OFF)
target_link_libraries(test_core PRIVATE Threads::Threads)
add_test(NAME unit_tests COMMAND test_core)

add_executable(test_concurrency tests/test_concurrency.cpp ${BACKEND_SOURCES})
set_target_properties(test_concurrency PROPERTIES AUTOMOC OFF)
target_link_libraries(test_concurrency PRIVATE Threads::Threads)
add_test(NAME concurrency_test COMMAND test_concurrency)
