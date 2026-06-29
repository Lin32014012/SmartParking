#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include "managers/SpotManager.h"
#include "managers/BillingManager.h"
#include "managers/VehicleManager.h"

class StatisticsPanel : public QWidget {
    Q_OBJECT

public:
    StatisticsPanel(SpotManager& sm, BillingManager& bm, VehicleManager& vm,
                    QWidget* parent = nullptr);

    void refresh();

private:
    void setupUI();
    void updateOverview();
    void updateTypeDistribution();
    void updateBillHistory();

    SpotManager& spotManager;
    BillingManager& billingManager;
    VehicleManager& vehicleManager;

    QLabel* totalLabel;
    QLabel* occupiedLabel;
    QLabel* availableLabel;
    QLabel* reservedLabel;
    QLabel* revenueLabel;
    QLabel* billCountLabel;
    QLabel* typeDistLabel;
    QTableWidget* billTable;
};
