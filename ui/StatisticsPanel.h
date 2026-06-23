#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include "managers/SpotManager.h"
#include "managers/BillingManager.h"

class StatisticsPanel : public QWidget {
    Q_OBJECT

public:
    StatisticsPanel(SpotManager& sm, BillingManager& bm, QWidget* parent = nullptr);

    void refresh();

private:
    void setupUI();
    void updateOverview();
    void updateBillHistory();

    SpotManager& spotManager;
    BillingManager& billingManager;

    QLabel* totalLabel;
    QLabel* occupiedLabel;
    QLabel* availableLabel;
    QLabel* reservedLabel;
    QLabel* rateLabel;
    QLabel* revenueLabel;
    QLabel* billCountLabel;
    QTableWidget* billTable;
};
