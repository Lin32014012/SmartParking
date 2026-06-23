#include "VehiclePanel.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QDateTime>
#include <QDialogButtonBox>
#include <QFormLayout>

VehiclePanel::VehiclePanel(ParkingLot& lot, VehicleManager& vm, SpotManager& sm,
                           BillingManager& bm, QWidget* parent)
    : QWidget(parent)
    , parkingLot(lot)
    , vehicleManager(vm)
    , spotManager(sm)
    , billingManager(bm)
{
    setupUI();
    refreshTable();
}

void VehiclePanel::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);

    QHBoxLayout* inputLayout = new QHBoxLayout;

    plateInput = new QLineEdit;
    plateInput->setPlaceholderText("输入车牌号 (如: 京A12345)");
    plateInput->setStyleSheet("QLineEdit { padding: 8px; font-size: 14px; }");

    typeCombo = new QComboBox;
    typeCombo->addItems({"轿车", "卡车", "摩托车"});
    typeCombo->setStyleSheet("QComboBox { padding: 8px; font-size: 14px; }");

    QPushButton* entryBtn = new QPushButton("车辆入场");
    entryBtn->setStyleSheet(
        "QPushButton { background-color: #2ecc71; color: white; padding: 8px 20px; "
        "font-size: 14px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #27ae60; }"
    );

    QPushButton* exitBtn = new QPushButton("车辆离场");
    exitBtn->setStyleSheet(
        "QPushButton { background-color: #e67e22; color: white; padding: 8px 20px; "
        "font-size: 14px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #d35400; }"
    );

    QPushButton* searchBtn = new QPushButton("查询");
    searchBtn->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; padding: 8px 20px; "
        "font-size: 14px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #2980b9; }"
    );

    inputLayout->addWidget(plateInput, 3);
    inputLayout->addWidget(typeCombo, 1);
    inputLayout->addWidget(entryBtn);
    inputLayout->addWidget(exitBtn);
    inputLayout->addWidget(searchBtn);

    mainLayout->addLayout(inputLayout);

    vehicleTable = new QTableWidget;
    vehicleTable->setColumnCount(5);
    vehicleTable->setHorizontalHeaderLabels({"车牌号", "类型", "入场时间", "停车时长", "当前费用"});
    vehicleTable->horizontalHeader()->setStretchLastSection(true);
    vehicleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    vehicleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    vehicleTable->setStyleSheet(
        "QTableWidget { border: 1px solid #ddd; gridline-color: #eee; }"
        "QHeaderView::section { background-color: #34495e; color: white; padding: 6px; }"
    );

    mainLayout->addWidget(vehicleTable);

    connect(entryBtn, &QPushButton::clicked, this, &VehiclePanel::onEntry);
    connect(exitBtn, &QPushButton::clicked, this, &VehiclePanel::onExit);
    connect(searchBtn, &QPushButton::clicked, this, &VehiclePanel::onSearch);
}

void VehiclePanel::onEntry() {
    QString plate = plateInput->text().trimmed();
    if (plate.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入车牌号");
        return;
    }

    if (vehicleManager.findByPlate(plate.toStdString()) != nullptr) {
        QMessageBox::warning(this, "错误", "该车辆已在停车场内");
        return;
    }

    int typeIdx = typeCombo->currentIndex();
    std::unique_ptr<Vehicle> vehicle;
    switch (typeIdx) {
        case 0: vehicle = std::make_unique<Car>(plate.toStdString()); break;
        case 1: vehicle = std::make_unique<Truck>(plate.toStdString()); break;
        case 2: vehicle = std::make_unique<Motorcycle>(plate.toStdString()); break;
    }

    ParkingSpot* spot = spotManager.allocateSpot(*vehicle);
    if (spot) {
        spot->occupy(plate.toStdString());
        vehicleManager.registerVehicle(std::move(vehicle));

        QMessageBox::information(this, "成功",
            QString("车辆入场成功！\n\n车牌号: %1\n车位编号: %2\n区域: %3")
                .arg(plate)
                .arg(QString::fromStdString(spot->getSpotCode()))
                .arg(QString::fromStdString(spot->getZone())));

        plateInput->clear();
        refreshTable();
        emit dataChanged();
    } else {
        QMessageBox::warning(this, "失败", "没有可用的" + typeCombo->currentText() + "车位");
    }
}

void VehiclePanel::onExit() {
    QString plate = plateInput->text().trimmed();
    if (plate.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入车牌号");
        return;
    }

    Vehicle* vehicle = vehicleManager.findByPlate(plate.toStdString());
    if (!vehicle) {
        QMessageBox::warning(this, "错误", "未找到该车辆");
        return;
    }

    showBillDialog(plate);
}

void VehiclePanel::showBillDialog(const QString& plate) {
    Vehicle* vehicle = vehicleManager.findByPlate(plate.toStdString());
    if (!vehicle) return;

    BillRecord bill = billingManager.generateBill(*vehicle);

    QMessageBox dialog(this);
    dialog.setWindowTitle("停车账单");
    dialog.setIcon(QMessageBox::Information);
    dialog.setText(QString(
        "车牌号: %1\n"
        "类型: %2\n"
        "停车时长: %3 小时\n"
        "费用: ¥%4")
        .arg(QString::fromStdString(bill.plateNumber))
        .arg(QString::fromStdString(bill.vehicleType))
        .arg(bill.duration, 0, 'f', 1)
        .arg(bill.amount, 0, 'f', 2));

    QPushButton* payBtn = dialog.addButton("确认支付", QMessageBox::AcceptRole);
    QPushButton* cancelBtn = dialog.addButton("取消", QMessageBox::RejectRole);
    dialog.exec();

    if (dialog.clickedButton() == payBtn) {
        billingManager.addHistory(bill);
        ParkingSpot* spot = spotManager.findSpotByPlate(plate.toStdString());
        if (spot) spot->release();
        vehicleManager.removeVehicle(plate.toStdString());

        QMessageBox::information(this, "完成", "支付成功，车辆已离场");
        plateInput->clear();
        refreshTable();
        emit dataChanged();
    }
}

void VehiclePanel::onSearch() {
    QString plate = plateInput->text().trimmed();
    if (plate.isEmpty()) {
        refreshTable();
        return;
    }

    Vehicle* vehicle = vehicleManager.findByPlate(plate.toStdString());
    vehicleTable->setRowCount(0);

    if (vehicle) {
        vehicleTable->setRowCount(1);
        vehicleTable->setItem(0, 0, new QTableWidgetItem(QString::fromStdString(vehicle->getPlate())));
        vehicleTable->setItem(0, 1, new QTableWidgetItem(QString::fromStdString(vehicle->getType())));

        auto entryTime = vehicle->getEntryTime();
        auto epoch = std::chrono::duration_cast<std::chrono::seconds>(entryTime.time_since_epoch()).count();
        QDateTime dt = QDateTime::fromSecsSinceEpoch(epoch);
        vehicleTable->setItem(0, 2, new QTableWidgetItem(dt.toString("MM-dd HH:mm")));

        double duration = vehicle->calculateParkingDuration();
        vehicleTable->setItem(0, 3, new QTableWidgetItem(QString::number(duration, 'f', 1) + " h"));

        double fee = vehicle->calculateFee();
        vehicleTable->setItem(0, 4, new QTableWidgetItem(QString::number(fee, 'f', 2) + " 元"));
    }
}

void VehiclePanel::refreshTable() {
    vehicleTable->setRowCount(0);
    vehicleManager.forEachVehicle([this](const Vehicle& v) {
        int row = vehicleTable->rowCount();
        vehicleTable->insertRow(row);

        vehicleTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(v.getPlate())));
        vehicleTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(v.getType())));

        auto entryTime = v.getEntryTime();
        auto epoch = std::chrono::duration_cast<std::chrono::seconds>(entryTime.time_since_epoch()).count();
        QDateTime dt = QDateTime::fromSecsSinceEpoch(epoch);
        vehicleTable->setItem(row, 2, new QTableWidgetItem(dt.toString("MM-dd HH:mm")));

        double duration = v.calculateParkingDuration();
        vehicleTable->setItem(row, 3, new QTableWidgetItem(QString::number(duration, 'f', 1) + " h"));

        double fee = v.calculateFee();
        vehicleTable->setItem(row, 4, new QTableWidgetItem(QString::number(fee, 'f', 2) + " 元"));
    });
}
