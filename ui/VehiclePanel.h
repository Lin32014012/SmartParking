#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QDialog>

#include "core/ParkingLot.h"
#include "managers/VehicleManager.h"
#include "managers/SpotManager.h"
#include "managers/BillingManager.h"

class VehiclePanel : public QWidget {
    Q_OBJECT

public:
    VehiclePanel(ParkingLot& lot, VehicleManager& vm, SpotManager& sm, 
                 BillingManager& bm, QWidget* parent = nullptr);
    void refreshTable();

signals:
    void dataChanged();

private slots:
    void onEntry();
    void onExit();
    void onSearch();

private:
    void setupUI();
    void showBillDialog(const QString& plate);

    ParkingLot& parkingLot;
    VehicleManager& vehicleManager;
    SpotManager& spotManager;
    BillingManager& billingManager;

    QLineEdit* plateInput;
    QComboBox* typeCombo;
    QTableWidget* vehicleTable;
};
