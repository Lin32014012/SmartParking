#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTimer>
#include <QVector>

#include "core/ParkingLot.h"
#include "managers/VehicleManager.h"
#include "managers/SpotManager.h"
#include "managers/BillingManager.h"

class ParkingLotWidget;
class VehiclePanel;
class StatisticsPanel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onEntryClicked();
    void onExitClicked();
    void onQueryClicked();
    void onStatsClicked();
    void onSettingsClicked();
    void refreshStatus();

private:
    void setupUI();
    void setupSidebar();
    void setupCentral();
    void loadData();
    void saveData();

    ParkingLot parkingLot;
    VehicleManager vehicleManager;
    SpotManager spotManager;
    BillingManager billingManager;

    QStackedWidget* stackedWidget;
    ParkingLotWidget* parkingLotWidget;
    VehiclePanel* vehiclePanel;
    StatisticsPanel* statisticsPanel;

    QLabel* statusLabel;
    QLabel* occupiedLabel;
    QLabel* availableLabel;
    QTimer* refreshTimer;

    QPushButton* entryBtn;
    QPushButton* exitBtn;
    QPushButton* queryBtn;
    QPushButton* statsBtn;
    QPushButton* settingsBtn;
};
