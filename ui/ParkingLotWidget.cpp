#include "ParkingLotWidget.h"
#include <QMouseEvent>

ParkingLotWidget::ParkingLotWidget(QWidget* parent)
    : QWidget(parent)
    , highlightedSpotId(-1)
    , columns(10)
    , cellSize(60)
    , margin(20)
    , legendHeight(50)
    , emptyColor(46, 204, 113)
    , reservedColor(241, 196, 15)
    , occupiedColor(231, 76, 60)
    , highlightColor(52, 152, 219)
{
    setMinimumSize(400, 300);
    setMouseTracking(true);
}

void ParkingLotWidget::updateSpots(const QVector<ParkingSpot*>& spots) {
    spotVisuals.clear();
    int x = margin;
    int y = margin;
    int idx = 0;

    for (auto* spot : spots) {
        SpotVisual sv;
        sv.spotId = spot->getSpotId();
        sv.code = QString::fromStdString(spot->getSpotCode());
        sv.status = spot->getStatus();
        sv.plate = QString::fromStdString(spot->getPlate());
        sv.rect = QRect(x, y, cellSize - 4, cellSize - 4);

        spotVisuals.append(sv);

        idx++;
        x += cellSize;
        if (idx % columns == 0) {
            x = margin;
            y += cellSize;
        }
    }

    int rows = (spots.size() + columns - 1) / columns;
    setMinimumHeight(y + cellSize + legendHeight + margin);
    update();
}

void ParkingLotWidget::highlightSpot(const QString& code) {
    for (const auto& sv : spotVisuals) {
        if (sv.code == code) {
            highlightedSpotId = sv.spotId;
            update();
            return;
        }
    }
}

void ParkingLotWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor(236, 240, 241));

    painter.setPen(QColor(44, 62, 80));
    QFont titleFont = painter.font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.drawText(QRect(margin, 8, width() - 2 * margin, 30), Qt::AlignCenter, "停车场平面图");

    for (const auto& sv : spotVisuals) {
        drawSpot(painter, sv);
    }

    drawLegend(painter);
}

void ParkingLotWidget::drawSpot(QPainter& painter, const SpotVisual& spot) {
    QColor bgColor;
    switch (spot.status) {
        case SpotStatus::Empty: bgColor = emptyColor; break;
        case SpotStatus::Reserved: bgColor = reservedColor; break;
        case SpotStatus::Occupied: bgColor = occupiedColor; break;
    }

    if (spot.spotId == highlightedSpotId) {
        bgColor = highlightColor;
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(spot.rect, 6, 6);

    painter.setPen(Qt::white);
    QFont codeFont = painter.font();
    codeFont.setPointSize(9);
    codeFont.setBold(true);
    painter.setFont(codeFont);

    QRect textRect = spot.rect.adjusted(0, 0, 0, -10);
    painter.drawText(textRect, Qt::AlignCenter, spot.code);

    if (spot.status == SpotStatus::Occupied && !spot.plate.isEmpty()) {
        QFont plateFont = painter.font();
        plateFont.setPointSize(7);
        plateFont.setBold(false);
        painter.setFont(plateFont);
        QRect plateRect = spot.rect.adjusted(2, spot.rect.height() / 2 - 4, -2, -2);
        painter.drawText(plateRect, Qt::AlignCenter, spot.plate.mid(0, 4));
    } else {
        QFont iconFont = painter.font();
        iconFont.setPointSize(10);
        painter.setFont(iconFont);
        QRect iconRect = spot.rect.adjusted(0, spot.rect.height() / 2 - 8, 0, 0);
        QString icon = (spot.status == SpotStatus::Empty) ? "✓" : "⏳";
        painter.drawText(iconRect, Qt::AlignCenter, icon);
    }
}

void ParkingLotWidget::drawLegend(QPainter& painter) {
    int legendY = height() - legendHeight;
    int legendX = margin;

    painter.setPen(Qt::NoPen);

    painter.setBrush(emptyColor);
    painter.drawRoundedRect(legendX, legendY, 16, 16, 3, 3);
    painter.setPen(QColor(44, 62, 80));
    painter.drawText(legendX + 22, legendY + 13, "空闲");

    legendX += 70;
    painter.setPen(Qt::NoPen);
    painter.setBrush(reservedColor);
    painter.drawRoundedRect(legendX, legendY, 16, 16, 3, 3);
    painter.setPen(QColor(44, 62, 80));
    painter.drawText(legendX + 22, legendY + 13, "预留");

    legendX += 70;
    painter.setPen(Qt::NoPen);
    painter.setBrush(occupiedColor);
    painter.drawRoundedRect(legendX, legendY, 16, 16, 3, 3);
    painter.setPen(QColor(44, 62, 80));
    painter.drawText(legendX + 22, legendY + 13, "占用");
}

void ParkingLotWidget::mousePressEvent(QMouseEvent* event) {
    QPoint pos = event->pos();
    for (const auto& sv : spotVisuals) {
        if (sv.rect.contains(pos)) {
            emit spotClicked(sv.spotId);
            highlightedSpotId = sv.spotId;
            update();
            return;
        }
    }
}

void ParkingLotWidget::recalculateLayout() {
    update();
}
