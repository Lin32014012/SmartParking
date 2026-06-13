# SmartParking - 智能停车场管理系统

一个基于C++17的智能停车场管理系统，模拟真实停车场的完整业务流程。

## 项目特点

- **面向对象设计**：通过继承和多态实现不同车型和车位类型的差异化处理
- **泛型编程**：使用类模板和模板特化实现通用容器管理器和差异化计费策略
- **三态状态机**：解决车位分配的并发竞态问题
- **STL应用**：运用算法、容器、迭代器和Lambda表达式

## 技术栈

- C++17
- STL (Standard Template Library)
- CMake 构建系统

## 功能模块

1. **车辆管理**：入场登记、离场处理、信息查询
2. **车位管理**：智能分配（含预留机制）、释放、状态查询、超时检测
3. **计费系统**：实时计费、不同车型费率、账单生成
4. **导航引导**：简化版路径指引
5. **数据统计**：收入统计、车位使用率、车型分布

## 项目结构

```
SmartParking/
├── CMakeLists.txt          # CMake构建配置
├── README.md               # 项目说明文档
├── include/                # 头文件目录
│   ├── core/               # 核心数据模型
│   │   ├── Vehicle.h       # 车辆类定义
│   │   ├── ParkingSpot.h   # 车位类定义
│   │   └── ParkingLot.h    # 停车场类定义
│   ├── managers/           # 业务管理器
│   │   ├── VehicleManager.h
│   │   ├── SpotManager.h
│   │   └── BillingManager.h
│   ├── utils/              # 工具类
│   │   ├── EntityManager.h # 泛型容器模板
│   │   ├── Statistics.h    # 统计模板
│   │   ├── Filter.h        # 过滤器模板
│   │   └── RatePolicy.h    # 计费策略模板
│   └── ui/
│       └── ConsoleUI.h     # 控制台界面
├── src/                    # 源文件目录
│   ├── core/
│   ├── managers/
│   ├── utils/
│   └── ui/
├── data/                   # 数据目录
│   └── config.json         # 系统配置
└── main.cpp                # 程序入口
```

## 构建与运行

### 前置要求

- C++17 兼容的编译器 (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+

### 构建步骤

```bash
# 克隆项目
git clone <repository-url>
cd SmartParking

# 创建构建目录
cmake -B build

# 编译
cmake --build build

# 运行
./build/SmartParking  # Linux/Mac
# 或
build\Debug\SmartParking.exe  # Windows
```

### 使用MSVC (Windows)

```bash
# 使用Visual Studio Developer Command Prompt
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

## 使用说明

### 主菜单

```
========================================
  SmartParking 智能停车场管理系统
========================================
  当前状态: 已停车 0/100 | 空闲车位: 100
========================================
  1. 车辆入场
  2. 车辆离场
  3. 查询车辆
  4. 查看车位状态
  5. 数据统计
  6. 系统设置
  0. 退出系统
========================================
```

### 车辆入场流程

1. 选择"车辆入场"
2. 输入车牌号
3. 选择车辆类型（小型车/大型车/摩托车）
4. 系统分配车位并显示导航引导

### 车辆离场流程

1. 选择"车辆离场"
2. 输入车牌号
3. 系统显示停车账单
4. 确认支付后车辆离场

## 核心设计

### 类层次结构

**车辆类**：
- `Vehicle` (抽象基类)
  - `Car` (小型车，费率5元/小时)
  - `Truck` (大型车，费率10元/小时)
  - `Motorcycle` (摩托车，费率2.5元/小时)

**车位类**：
- `ParkingSpot` (抽象基类)
  - `SmallSpot` (小型车位)
  - `LargeSpot` (大型车位)
  - `MCSpot` (摩托车位)

### 三态车位状态机

```
空闲(Empty) → 已预留(Reserved) → 已占用(Occupied) → 空闲(Empty)
```

**预留机制**：
- 分配车位时将状态改为"已预留"
- 记录预留车牌号、预留时间戳
- 超时未到达（2分钟）自动释放车位
- 防止并发分配的竞态问题

### 泛型编程应用

1. **EntityManager<T>**：通用容器管理器模板
2. **StatisticsCollector<T, ValueType>**：通用统计器模板
3. **Filter<T>**：通用过滤器模板
4. **RatePolicy<T>**：计费策略模板特化

## C++知识点体现

- ✅ 类设计、继承、多态、虚函数
- ✅ 模板编程、模板特化
- ✅ STL容器、算法、迭代器
- ✅ 智能指针、RAII
- ✅ Lambda表达式、函数对象
- ✅ 异常处理
- ✅ CMake构建系统

## 简历描述

**项目名称**：SmartParking - 智能停车场管理系统

**技术栈**：C++17 | STL | CMake

**项目描述**：
设计并实现了一个智能停车场管理系统，模拟真实停车场的完整业务流程，包括车辆管理、车位分配、自动计费和数据统计等功能。

**技术亮点**：
- 采用面向对象设计，通过继承和多态实现不同车型和车位类型的差异化处理
- 设计三态车位状态机（空闲→预留→占用），解决并发分配的竞态问题
- 使用类模板和模板特化实现通用容器管理器和差异化计费策略
- 运用STL算法（sort、find_if、accumulate）和Lambda表达式进行数据处理
- 采用RAII和智能指针管理资源，确保内存安全

## 许可证

MIT License
