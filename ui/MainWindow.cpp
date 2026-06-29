#include "MainWindow.h"
#include "ParkingLotWidget.h"
#include "VehiclePanel.h"
#include "StatisticsPanel.h"
#include "utils/FileManager.h"

#include <QApplication>
#include <QCoreApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QSplitter>
#include <QFrame>
#include <QFont>
#include <QScrollArea>

namespace {
    // 数据文件固定存放在可执行文件同级目录，避免因启动方式不同（IDE 调试 vs 双击）
    // 导致工作目录变化、数据文件"找不到"的问题。
    std::string dataFilePath() {
        return (QCoreApplication::applicationDirPath() + "/parking_data.txt").toStdString();
    }
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , parkingLot()
    , spotManager(parkingLot)
    , stackedWidget(new QStackedWidget)
    , parkingLotWidget(new ParkingLotWidget)
    , vehiclePanel(new VehiclePanel(parkingLot, vehicleManager, spotManager, billingManager))
    , statisticsPanel(new StatisticsPanel(spotManager, billingManager, vehicleManager))
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
    vehiclePanel->refreshTable();

    setupUI();
    setupSidebar();
    setupCentral();
    refreshStatus();

    connect(refreshTimer, &QTimer::timeout, this, &MainWindow::refreshStatus);
    refreshTimer->start(5000);

    connect(vehiclePanel, &VehiclePanel::dataChanged, this, &MainWindow::refreshStatus);
    connect(parkingLotWidget, &ParkingLotWidget::quickEntry, this, &MainWindow::onQuickEntry);
    connect(parkingLotWidget, &ParkingLotWidget::quickExit, this, &MainWindow::onQuickExit);
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
    mapBtn = new QPushButton("🗺️  车位平面图");
    statsBtn = new QPushButton("📊  数据统计");
    settingsBtn = new QPushButton("⚙️  系统设置");

    layout->addWidget(entryBtn);
    layout->addWidget(exitBtn);
    layout->addWidget(queryBtn);
    layout->addWidget(mapBtn);
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
    connect(mapBtn, &QPushButton::clicked, this, [this]() {
        parkingLotWidget->clearHighlight();
        stackedWidget->setCurrentIndex(0);
        entryBtn->setStyleSheet("");
        exitBtn->setStyleSheet("");
        queryBtn->setStyleSheet("");
        mapBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; }");
        statsBtn->setStyleSheet("");
        settingsBtn->setStyleSheet("");
    });
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
    parkingLotWidget->clearHighlight();
    entryBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; }");
    exitBtn->setStyleSheet("");
    queryBtn->setStyleSheet("");
    mapBtn->setStyleSheet("");
    statsBtn->setStyleSheet("");
    settingsBtn->setStyleSheet("");
}

void MainWindow::onExitClicked() {
    stackedWidget->setCurrentIndex(1);
    parkingLotWidget->clearHighlight();
    entryBtn->setStyleSheet("");
    exitBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; }");
    queryBtn->setStyleSheet("");
    mapBtn->setStyleSheet("");
    statsBtn->setStyleSheet("");
    settingsBtn->setStyleSheet("");
}

void MainWindow::onQueryClicked() {
    stackedWidget->setCurrentIndex(1);
    parkingLotWidget->clearHighlight();
    entryBtn->setStyleSheet("");
    exitBtn->setStyleSheet("");
    queryBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; }");
    mapBtn->setStyleSheet("");
    statsBtn->setStyleSheet("");
    settingsBtn->setStyleSheet("");
}

void MainWindow::onStatsClicked() {
    stackedWidget->setCurrentIndex(2);
    parkingLotWidget->clearHighlight();
    statisticsPanel->refresh();
    entryBtn->setStyleSheet("");
    exitBtn->setStyleSheet("");
    queryBtn->setStyleSheet("");
    mapBtn->setStyleSheet("");
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
    const std::string DATA_FILE = dataFilePath();
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
    const std::string DATA_FILE = dataFilePath();
    FileManager::saveSpots(DATA_FILE, parkingLot.exportSpots());
    FileManager::saveVehicles(DATA_FILE, vehicleManager.exportVehicles());
    FileManager::saveBillHistory(DATA_FILE, billingManager.exportHistory());
}

void MainWindow::onQuickEntry(const QString& spotCode) {
    bool ok;
    QString plate = QInputDialog::getText(this, "快捷进场",
        "车位: " + spotCode + "\n请输入车牌号:", QLineEdit::Normal, "", &ok);

    if (!ok || plate.trimmed().isEmpty()) return;
    plate = plate.trimmed();

    if (vehicleManager.findByPlate(plate.toStdString()) != nullptr) {
        QMessageBox::warning(this, "错误", "该车辆已在停车场内");
        return;
    }

    QStringList types = {"轿车", "卡车", "摩托车"};
    QString type = QInputDialog::getItem(this, "选择车型", "车辆类型:", types, 0, false, &ok);
    if (!ok) return;

    std::unique_ptr<Vehicle> vehicle;
    if (type == "轿车") vehicle = std::make_unique<Car>(plate.toStdString());
    else if (type == "卡车") vehicle = std::make_unique<Truck>(plate.toStdString());
    else vehicle = std::make_unique<Motorcycle>(plate.toStdString());

    ParkingSpot* spot = spotManager.allocateSpot(*vehicle);
    if (spot) {
        spot->occupy(plate.toStdString());
        vehicleManager.registerVehicle(std::move(vehicle));
        QMessageBox::information(this, "成功",
            QString("车辆入场成功！\n\n车牌号: %1\n车位: %2").arg(plate).arg(spotCode));
        refreshStatus();
        vehiclePanel->refreshTable();
        saveData();
    } else {
        QMessageBox::warning(this, "失败", "车位分配失败");
    }
}

void MainWindow::onQuickExit(const QString& spotCode, const QString& plate) {
    Vehicle* vehicle = vehicleManager.findByPlate(plate.toStdString());
    if (!vehicle) {
        QMessageBox::warning(this, "错误", "未找到该车辆");
        return;
    }

    BillRecord bill = billingManager.generateBill(*vehicle);

    QMessageBox dialog(this);
    dialog.setWindowTitle("快捷离场");
    dialog.setIcon(QMessageBox::Information);
    dialog.setText(QString(
        "车位: %1\n"
        "车牌号: %2\n"
        "类型: %3\n"
        "停车时长: %4 小时\n"
        "费用: ¥%5")
        .arg(spotCode)
        .arg(QString::fromStdString(bill.plateNumber))
        .arg(QString::fromStdString(bill.vehicleType))
        .arg(bill.duration, 0, 'f', 1)
        .arg(bill.amount, 0, 'f', 2));

    QPushButton* payBtn = dialog.addButton("确认支付", QMessageBox::AcceptRole);
    dialog.addButton("取消", QMessageBox::RejectRole);
    dialog.exec();

    if (dialog.clickedButton() == payBtn) {
        billingManager.addHistory(bill);
        ParkingSpot* spot = spotManager.findSpotByPlate(plate.toStdString());
        if (spot) spot->release();
        vehicleManager.removeVehicle(plate.toStdString());

        QMessageBox::information(this, "完成", "支付成功，车辆已离场");
        refreshStatus();
        vehiclePanel->refreshTable();
        saveData();
    }
}
