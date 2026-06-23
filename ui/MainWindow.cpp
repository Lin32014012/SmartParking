#include "MainWindow.h"
#include "ParkingLotWidget.h"
#include "VehiclePanel.h"
#include "StatisticsPanel.h"
#include "utils/FileManager.h"

#include <QApplication>
#include <QMessageBox>
#include <QSplitter>
#include <QFrame>
#include <QFont>
#include <QScrollArea>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , parkingLot()
    , spotManager(parkingLot)
    , stackedWidget(new QStackedWidget)
    , parkingLotWidget(new ParkingLotWidget)
    , vehiclePanel(new VehiclePanel(parkingLot, vehicleManager, spotManager, billingManager))
    , statisticsPanel(new StatisticsPanel(spotManager, billingManager))
    , statusLabel(new QLabel)
    , occupiedLabel(new QLabel)
    , availableLabel(new QLabel)
    , refreshTimer(new QTimer(this))
{
    setWindowTitle("SmartParking - 智能停车管理系统");
    resize(1200, 750);
    setMinimumSize(900, 600);

    spotManager.initializeSpots(60, 30, 10);
    loadData();

    setupUI();
    setupSidebar();
    setupCentral();
    refreshStatus();

    connect(refreshTimer, &QTimer::timeout, this, &MainWindow::refreshStatus);
    refreshTimer->start(5000);

    connect(vehiclePanel, &VehiclePanel::dataChanged, this, &MainWindow::refreshStatus);
}

MainWindow::~MainWindow() {
    saveData();
}

void MainWindow::setupUI() {
    QWidget* central = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setCentralWidget(central);
}

void MainWindow::setupSidebar() {
    QWidget* sidebar = new QWidget;
    sidebar->setFixedWidth(200);
    sidebar->setStyleSheet(
        "QWidget { background-color: #2c3e50; }"
        "QPushButton {"
        "  color: white;"
        "  border: none;"
        "  padding: 12px 16px;"
        "  text-align: left;"
        "  font-size: 14px;"
        "  border-radius: 4px;"
        "  margin: 2px 8px;"
        "}"
        "QPushButton:hover { background-color: #34495e; }"
        "QPushButton:pressed { background-color: #1a252f; }"
    );

    QVBoxLayout* layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(0, 16, 0, 16);
    layout->setSpacing(4);

    QLabel* logo = new QLabel("SmartParking");
    logo->setAlignment(Qt::AlignCenter);
    logo->setStyleSheet("color: white; font-size: 18px; font-weight: bold; padding: 16px 0 24px 0;");
    layout->addWidget(logo);

    entryBtn = new QPushButton("🚗  车辆入场");
    exitBtn = new QPushButton("🚙  车辆离场");
    queryBtn = new QPushButton("🔍  车辆查询");
    statsBtn = new QPushButton("📊  数据统计");
    settingsBtn = new QPushButton("⚙️  系统设置");

    layout->addWidget(entryBtn);
    layout->addWidget(exitBtn);
    layout->addWidget(queryBtn);
    layout->addWidget(statsBtn);
    layout->addWidget(settingsBtn);
    layout->addStretch();

    QFrame* line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #34495e;");
    layout->addWidget(line);

    statusLabel->setStyleSheet("color: #95a5a6; font-size: 11px; padding: 8px;");
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);

    occupiedLabel->setStyleSheet("color: #e74c3c; font-size: 11px; padding: 0 8px 4px 8px;");
    occupiedLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(occupiedLabel);

    availableLabel->setStyleSheet("color: #2ecc71; font-size: 11px; padding: 0 8px 8px 8px;");
    availableLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(availableLabel);

    centralWidget()->layout()->addWidget(sidebar);

    connect(entryBtn, &QPushButton::clicked, this, &MainWindow::onEntryClicked);
    connect(exitBtn, &QPushButton::clicked, this, &MainWindow::onExitClicked);
    connect(queryBtn, &QPushButton::clicked, this, &MainWindow::onQueryClicked);
    connect(statsBtn, &QPushButton::clicked, this, &MainWindow::onStatsClicked);
    connect(settingsBtn, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);
}

void MainWindow::setupCentral() {
    stackedWidget->addWidget(parkingLotWidget);
    stackedWidget->addWidget(vehiclePanel);
    stackedWidget->addWidget(statisticsPanel);
    stackedWidget->setCurrentIndex(0);

    centralWidget()->layout()->addWidget(stackedWidget);
}

void MainWindow::refreshStatus() {
    spotManager.checkTimeouts();
    int total = spotManager.getTotalSpots();
    int occupied = spotManager.getOccupiedCount();
    int available = spotManager.getAvailableCount();
    int reserved = spotManager.getReservedCount();

    statusLabel->setText(QString("状态: %1/%2 已占用").arg(occupied).arg(total));
    occupiedLabel->setText(QString("占用: %1 | 预留: %2").arg(occupied).arg(reserved));
    availableLabel->setText(QString("空闲: %1").arg(available));

    std::vector<ParkingSpot*> allSpotsVec = parkingLot.getAllSpots();
    QVector<ParkingSpot*> allSpots(allSpotsVec.begin(), allSpotsVec.end());
    parkingLotWidget->updateSpots(allSpots);
}

void MainWindow::onEntryClicked() {
    stackedWidget->setCurrentIndex(1);
    entryBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; }");
    exitBtn->setStyleSheet("");
    queryBtn->setStyleSheet("");
    statsBtn->setStyleSheet("");
    settingsBtn->setStyleSheet("");
}

void MainWindow::onExitClicked() {
    stackedWidget->setCurrentIndex(1);
    entryBtn->setStyleSheet("");
    exitBtn->setStyleSheet("QPushButton { background-color: #e67e22; color: white; }");
    queryBtn->setStyleSheet("");
    statsBtn->setStyleSheet("");
    settingsBtn->setStyleSheet("");
}

void MainWindow::onQueryClicked() {
    stackedWidget->setCurrentIndex(1);
    entryBtn->setStyleSheet("");
    exitBtn->setStyleSheet("");
    queryBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; }");
    statsBtn->setStyleSheet("");
    settingsBtn->setStyleSheet("");
}

void MainWindow::onStatsClicked() {
    stackedWidget->setCurrentIndex(2);
    statisticsPanel->refresh();
    entryBtn->setStyleSheet("");
    exitBtn->setStyleSheet("");
    queryBtn->setStyleSheet("");
    statsBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; }");
    settingsBtn->setStyleSheet("");
}

void MainWindow::onSettingsClicked() {
    QMessageBox::information(this, "系统设置",
        "费率配置:\n"
        "  轿车: 5.0元/小时\n"
        "  卡车: 10.0元/小时\n"
        "  摩托车: 2.5元/小时\n\n"
        "预留超时: 120秒\n"
        "车位配置: 小型60 + 大型30 + 摩托10");
}

void MainWindow::loadData() {
    static const std::string DATA_FILE = "parking_data.txt";
    if (!FileManager::fileExists(DATA_FILE)) return;

    auto spotRecords = FileManager::loadSpots(DATA_FILE);
    parkingLot.importSpots(spotRecords);

    auto vehicleRecords = FileManager::loadVehicles(DATA_FILE);
    for (const auto& vr : vehicleRecords) {
        std::unique_ptr<Vehicle> v;
        if (vr.vehicleType == "轿车") v = std::make_unique<Car>(vr.plateNumber);
        else if (vr.vehicleType == "卡车") v = std::make_unique<Truck>(vr.plateNumber);
        else if (vr.vehicleType == "摩托车") v = std::make_unique<Motorcycle>(vr.plateNumber);
        if (v) {
            auto tp = std::chrono::system_clock::time_point(std::chrono::seconds(vr.entryTimestamp));
            v->setEntryTime(tp);
            vehicleManager.registerVehicle(std::move(v));
        }
    }

    auto billRecords = FileManager::loadBillHistory(DATA_FILE);
    billingManager.importHistory(billRecords);
}

void MainWindow::saveData() {
    static const std::string DATA_FILE = "parking_data.txt";
    FileManager::saveSpots(DATA_FILE, parkingLot.exportSpots());
    FileManager::saveVehicles(DATA_FILE, vehicleManager.exportVehicles());
    FileManager::saveBillHistory(DATA_FILE, billingManager.exportHistory());
}
