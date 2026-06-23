#pragma once

#include <QWidget>
#include <QPainter>
#include <QVector>
#include <QPoint>
#include <QMenu>
#include "core/ParkingSpot.h"

struct SpotVisual {
    int spotId;
    QString code;
    QRect rect;
    SpotStatus status;
    QString plate;
    QString zone;
};

class ParkingLotWidget : public QWidget {
    Q_OBJECT

public:
    explicit ParkingLotWidget(QWidget* parent = nullptr);

    void updateSpots(const QVector<ParkingSpot*>& spots);
    void highlightSpot(const QString& code);
    void clearHighlight();

signals:
    void spotClicked(int spotId);
    void quickEntry(const QString& spotCode);
    void quickExit(const QString& spotCode, const QString& plate);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void drawSpot(QPainter& painter, const SpotVisual& spot);
    void drawLegend(QPainter& painter);
    void drawZoneLabel(QPainter& painter, const QString& zone, int y);
    void showContextMenu(const QPoint& pos, const SpotVisual& spot);
    void recalculateLayout();

    QVector<SpotVisual> spotVisuals;
    int highlightedSpotId;
    int cellSize;
    int gap;
    int margin;
    int titleHeight;
    int legendHeight;

    QColor emptyColor;
    QColor reservedColor;
    QColor occupiedColor;
    QColor highlightColor;
    QColor zoneColors[3];
};
