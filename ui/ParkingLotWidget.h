#pragma once

#include <QWidget>
#include <QPainter>
#include <QVector>
#include <QPoint>
#include "core/ParkingSpot.h"

struct SpotVisual {
    int spotId;
    QString code;
    QRect rect;
    SpotStatus status;
    QString plate;
};

class ParkingLotWidget : public QWidget {
    Q_OBJECT

public:
    explicit ParkingLotWidget(QWidget* parent = nullptr);

    void updateSpots(const QVector<ParkingSpot*>& spots);
    void highlightSpot(const QString& code);

signals:
    void spotClicked(int spotId);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void drawSpot(QPainter& painter, const SpotVisual& spot);
    void drawLegend(QPainter& painter);
    void recalculateLayout();

    QVector<SpotVisual> spotVisuals;
    int highlightedSpotId;
    int columns;
    int cellSize;
    int margin;
    int legendHeight;

    QColor emptyColor;
    QColor reservedColor;
    QColor occupiedColor;
    QColor highlightColor;
};
