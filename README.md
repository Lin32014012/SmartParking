# SmartParking - 智能停车场管理系统（Qt 桌面版）

基于 **C++17 + Qt 6** 实现的桌面图形界面停车场管理系统，模拟真实停车场的完整业务流程：车辆进出场、车位可视化管理、差异化计费与数据统计。

> 本项目由控制台版本演进而来，核心业务逻辑（core / managers）与图形界面（Qt Widgets）解耦，是一个面向对象 + 泛型编程 + GUI 工程实践的综合项目。

---

## ✨ 功能特性

- **可视化车位平面图**：使用 `QPainter` 自绘停车场平面图，三种颜色区分车位状态（空闲 / 预留 / 占用），右键车位可直接快捷进场 / 离场。
- **车辆管理**：进场登记（车牌 + 车型）、离场结算、按车牌查询，表格实时展示在场车辆的入场时间、停车时长与当前费用。
- **差异化计费**：通过 `RatePolicy<T>` 模板特化实现不同车型的计费策略——轿车夜间折扣、卡车不足一小时按一小时计、摩托车标准费率。
- **数据统计看板**：总车位 / 占用 / 空闲 / 预留 / 总营收 / 总账单卡片，**在场车型分布**统计，以及计费历史明细表。
- **三态车位状态机 + 超时回收**：空闲 → 预留 → 占用 → 空闲，配合 `QTimer` 每 5 秒检测并自动回收超时（120 秒）未到达的预留车位。
- **并发安全的车位分配**：`ParkingLot` 用 `std::mutex` 保护「查找空位 → 预留 → 占用」临界区，16 线程并发压测下无重复分配。
- **文件持久化**：进出场数据、车位状态、账单历史以分区段文本格式落盘，程序重启自动恢复。
- **单元测试 + 并发压测**：后端逻辑独立于 Qt 编译，通过 CTest 运行单元测试与多线程压力测试。

---

## 🛠 技术栈

| 分类 | 技术 |
|------|------|
| 语言 | C++17 |
| GUI | Qt 6（Core / Gui / Widgets），自定义 `QPainter` 绘制 |
| 构建 | CMake 3.16+（AUTOMOC / AUTORCC / AUTOUIC） |
| 编译器 | MinGW-w64（MSVC 亦可，已处理 UTF-8 字符集） |

---

## 🧱 项目结构

```
SmartParkingQt/
├── CMakeLists.txt              # 构建配置（Qt6 + CMake）
├── main_qt.cpp                 # 程序入口（QApplication）
├── include/                    # 头文件
│   ├── core/                   # 核心数据模型
│   │   ├── Vehicle.h           # 车辆继承体系（Car / Truck / Motorcycle）
│   │   ├── ParkingSpot.h       # 车位继承体系 + 三态状态机
│   │   └── ParkingLot.h        # 停车场聚合根
│   ├── managers/               # 业务管理层
│   │   ├── VehicleManager.h
│   │   ├── SpotManager.h
│   │   └── BillingManager.h
│   └── utils/                  # 泛型工具模板
│       ├── EntityManager.h     # 通用实体容器模板
│       ├── RatePolicy.h        # 计费策略模板（含模板特化）
│       ├── Statistics.h        # 通用统计收集器模板
│       └── FileManager.h       # 文件持久化
├── src/                        # 源文件（core / managers / utils）
├── ui/                         # Qt 图形界面层
│   ├── MainWindow.{h,cpp}      # 主窗口 + 侧边栏导航 + QStackedWidget
│   ├── ParkingLotWidget.{h,cpp}# 自绘车位平面图（QPainter）
│   ├── VehiclePanel.{h,cpp}    # 进出场 / 查询 + 车辆表格
│   └── StatisticsPanel.{h,cpp} # 数据统计看板
├── tests/                      # 测试（不依赖 Qt，CTest 驱动）
│   ├── test_core.cpp           # 核心逻辑单元测试
│   └── test_concurrency.cpp    # 多线程并发压测
└── data/
    └── config.json             # 费率 / 车位配置参考
```

---

## 🚀 构建与运行

### 前置要求

- Qt 6（建议 6.5+），含 Widgets 模块
- CMake 3.16+
- C++17 编译器（MinGW-w64 / MSVC / GCC / Clang）

### 命令行构建（MinGW 示例）

```bash
# 如 Qt 安装路径不同，用 -DCMAKE_PREFIX_PATH 覆盖
cmake -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/mingw_64
cmake --build build

# 运行（数据文件保存在可执行文件同级目录）
./build/SmartParkingQt.exe
```

### 使用 Qt Creator

直接用 Qt Creator 打开根目录的 `CMakeLists.txt`，选择 Qt 6 MinGW 套件，点击运行即可。

---

## 🧪 测试

后端逻辑（core / managers / utils）不依赖 Qt，可独立编译为测试程序，通过 CTest 运行：

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

- **`test_core`**：车辆 / 车位状态机、`RatePolicy` 计费、`EntityManager`、`ParkingLot` 分配等单元测试（44 项断言）。
- **`test_concurrency`**：16 线程并发抢占 100 个车位的压力测试，验证互斥锁保证「无重复分配」。

---

## 🏗 核心设计

### 1. 面向对象：继承与多态

```
Vehicle (抽象基类)                ParkingSpot (抽象基类)
 ├── Car         5 元/小时         ├── SmallSpot   小车位
 ├── Truck      10 元/小时         ├── LargeSpot   大车位
 └── Motorcycle 2.5 元/小时        └── MCSpot      摩托车位
```

`getHourlyRate()` / `getType()` / `canFitVehicle()` 为纯虚函数，运行时多态分派，新增车型 / 车位类型无需改动既有逻辑。

### 2. 泛型编程：三个模板

- **`EntityManager<T>`**：基于 `vector<unique_ptr<T>>` + `unordered_map` 哈希索引（平均 O(1) 按车牌查找）的通用实体容器，被 `VehicleManager` 复用。
- **`RatePolicy<T>`**：计费策略模板，对 `Car` / `Truck` / `Motorcycle` 分别**模板特化**，`BillingManager` 据车型分派计费。
- **`StatisticsCollector<T, ValueType>`**：通用统计收集器，用于统计面板的在场车型分布（数量 / 应收费用聚合）。

### 3. 三态车位状态机

```
空闲(Empty) → 预留(Reserved) → 占用(Occupied) → 空闲(Empty)
```

分配车位时先置为「预留」并记录车牌与时间戳；`MainWindow` 中的 `QTimer` 每 5 秒调用 `checkTimeouts()`，自动回收超过 120 秒仍未占用的预留车位。

并发场景下，`ParkingLot::parkVehicle()` 在单个 `std::mutex` 临界区内完成「查找空位 → 预留 → 占用」，保证多线程同时入场时不会把同一车位分配给两辆车；`tests/test_concurrency.cpp` 以 16 线程并发抢占验证无重复分配。

### 4. Qt 界面：信号槽解耦

- `ParkingLotWidget` 通过 `quickEntry(QString) / quickExit(QString,QString)` 信号把用户操作上抛给 `MainWindow`，与业务层解耦；
- `VehiclePanel` 操作完成后发 `dataChanged` 信号驱动状态刷新；
- `QStackedWidget` 实现「平面图 / 车辆管理 / 数据统计」多页面切换。

---

## 💡 简历描述参考

**项目名称**：SmartParking - 智能停车场管理系统（Qt 桌面版）

**技术栈**：C++17 | Qt 6 (Widgets) | CMake

**项目描述**：
独立设计并实现的桌面 GUI 停车场管理系统，覆盖车辆进出场、车位可视化、差异化计费与数据统计的完整业务闭环。

**技术亮点**：
- 基于 Qt 6 `QPainter` **自绘**车位平面图控件，颜色区分三态并支持右键快捷进出场，业务层与界面层通过**信号槽解耦**；
- 采用**继承 + 多态**实现多车型 / 多车位差异化处理，新增类型对既有代码零侵入；
- 使用**类模板与模板特化**（`EntityManager` / `RatePolicy` / `StatisticsCollector`）实现通用容器、差异化计费与统计聚合，车牌索引基于 `unordered_map` 做到平均 **O(1)** 查找；
- 设计三态车位状态机并配合 `QTimer` 定时回收超时预留；用 `std::mutex` 保护分配临界区，**16 线程并发压测无重复分配**；
- 采用 `unique_ptr` + RAII 管理对象生命周期，结合文件持久化实现数据自动存取；
- 后端逻辑与 Qt 解耦，配套 **CTest 单元测试 + 并发压力测试**。

---

## 许可证

MIT License
