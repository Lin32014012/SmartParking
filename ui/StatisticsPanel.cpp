#include "StatisticsPanel.h"
#include <QHeaderView>
#include <QGridLayout>
#include <QFrame>

StatisticsPanel::StatisticsPanel(SpotManager& sm, BillingManager& bm, QWidget* parent)
    : QWidget(parent)
    , spotManager(sm)
    , billingManager(bm)
{
    setupUI();
}

void StatisticsPanel::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);

    QLabel* title = new QLabel("数据统计");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50; padding: 8px 0;");
    mainLayout->addWidget(title);

    QGridLayout* grid = new QGridLayout;
    grid->setSpacing(12);

    auto createCard = [](const QString& title, const QString& value, const QString& color) -> QFrame* {
        QFrame* card = new QFrame;
        card->setStyleSheet(QString(
            "QFrame { background: white; border-radius: 8px; border-left: 4px solid %1; }"
            "QLabel { padding: 12px; }").arg(color));
        card->setMinimumHeight(80);

        QVBoxLayout* layout = new QVBoxLayout(card);
        QLabel* titleLabel = new QLabel(title);
        titleLabel->setStyleSheet("color: #7f8c8d; font-size: 12px;");
        QLabel* valueLabel = new QLabel(value);
        valueLabel->setStyleSheet(QString("color: %1; font-size: 24px; font-weight: bold;").arg(color));
        valueLabel->setObjectName("valueLabel");

        layout->addWidget(titleLabel);
        layout->addWidget(valueLabel);
        layout->addStretch();

        return card;
    };

    QFrame* totalCard = createCard("总车位", "0", "#2c3e50");
    QFrame* occupiedCard = createCard("已占用", "0", "#e74c3c");
    QFrame* availableCard = createCard("空闲", "0", "#2ecc71");
    QFrame* reservedCard = createCard("预留", "0", "#f39c12");
    QFrame* revenueCard = createCard("总营收", "¥0", "#3498db");
    QFrame* billCard = createCard("总账单", "0", "#9b59b6");

    grid->addWidget(totalCard, 0, 0);
    grid->addWidget(occupiedCard, 0, 1);
    grid->addWidget(availableCard, 0, 2);
    grid->addWidget(reservedCard, 1, 0);
    grid->addWidget(revenueCard, 1, 1);
    grid->addWidget(billCard, 1, 2);

    mainLayout->addLayout(grid);

    totalLabel = totalCard->findChild<QLabel*>("valueLabel");
    occupiedLabel = occupiedCard->findChild<QLabel*>("valueLabel");
    availableLabel = availableCard->findChild<QLabel*>("valueLabel");
    reservedLabel = reservedCard->findChild<QLabel*>("valueLabel");
    revenueLabel = revenueCard->findChild<QLabel*>("valueLabel");
    billCountLabel = billCard->findChild<QLabel*>("valueLabel");

    QLabel* historyTitle = new QLabel("计费历史");
    historyTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50; padding: 8px 0 0 0;");
    mainLayout->addWidget(historyTitle);

    billTable = new QTableWidget;
    billTable->setColumnCount(5);
    billTable->setHorizontalHeaderLabels({"车牌号", "类型", "停车时长", "费用", "离场时间"});
    billTable->horizontalHeader()->setStretchLastSection(true);
    billTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    billTable->setStyleSheet(
        "QTableWidget { border: 1px solid #ddd; gridline-color: #eee; }"
        "QHeaderView::section { background-color: #34495e; color: white; padding: 6px; }"
    );
    mainLayout->addWidget(billTable);
}

void StatisticsPanel::refresh() {
    updateOverview();
    updateBillHistory();
}

void StatisticsPanel::updateOverview() {
    totalLabel->setText(QString::number(spotManager.getTotalSpots()));
    occupiedLabel->setText(QString::number(spotManager.getOccupiedCount()));
    availableLabel->setText(QString::number(spotManager.getAvailableCount()));
    reservedLabel->setText(QString::number(spotManager.getReservedCount()));
    revenueLabel->setText(QString("¥%1").arg(billingManager.getTotalRevenue(), 0, 'f', 2));
    billCountLabel->setText(QString::number(billingManager.getBillCount()));
}

void StatisticsPanel::updateBillHistory() {
    billTable->setRowCount(0);

    const auto& history = billingManager.exportHistory();
    for (const auto& record : history) {
        int row = billTable->rowCount();
        billTable->insertRow(row);

        billTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(record.plateNumber)));
        billTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(record.vehicleType)));
        billTable->setItem(row, 2, new QTableWidgetItem(
            QString::number(record.duration, 'f', 1) + " h"));
        billTable->setItem(row, 3, new QTableWidgetItem(
            QString::number(record.amount, 'f', 2) + " 元"));
        billTable->setItem(row, 4, new QTableWidgetItem(
            QString::fromStdString(record.exitTime)));
    }
}
