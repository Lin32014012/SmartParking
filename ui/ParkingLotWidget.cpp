#include "ParkingLotWidget.h"
#include <QMouseEvent>
#include <QMenu>
#include <QResizeEvent>

ParkingLotWidget::ParkingLotWidget(QWidget* parent)
    : QWidget(parent)
    , highlightedSpotId(-1)
    , cellSize(56)
    , gap(4)
    , margin(20)
    , titleHeight(40)
    , legendHeight(40)
    , emptyColor(46, 204, 113)
    , reservedColor(241, 196, 15)
    , occupiedColor(231, 76, 60)
    , highlightColor(52, 152, 219)
{
    setMinimumSize(500, 400);
    setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    zoneColors[0] = QColor(52, 152, 219);
    zoneColors[1] = QColor(155, 89, 182);
    zoneColors[2] = QColor(230, 126, 34);
}

void ParkingLotWidget::updateSpots(const QVector<ParkingSpot*>& spots) {
    spotVisuals.clear();
    for (auto* spot : spots) {
        SpotVisual sv;
        sv.spotId = spot->getSpotId();
        sv.code = QString::fromStdString(spot->getSpotCode());
        sv.status = spot->getStatus();
        sv.plate = QString::fromStdString(spot->getPlate());
        sv.zone = QString::fromStdString(spot->getZone());
        spotVisuals.append(sv);
    }
    recalculateLayout();
}

void ParkingLotWidget::recalculateLayout() {
    if (spotVisuals.isEmpty()) {
        update();
        return;
    }

    int columns = qMax(1, (width() - 2 * margin) / (cellSize + gap));
    int x = margin;
    int y = margin + titleHeight;
    int idx = 0;
    QString lastZone;

    for (auto& sv : spotVisuals) {
        if (sv.zone != lastZone) {
            if (!lastZone.isEmpty()) {
                y += 28;
            }
            lastZone = sv.zone;
        }

        sv.rect = QRect(x, y, cellSize, cellSize);
        idx++;
        x += cellSize + gap;
        if (idx % columns == 0) {
            x = margin;
            y += cellSize + gap;
        }
    }

    int totalHeight = y + cellSize + gap + legendHeight + margin;
    setMinimumHeight(totalHeight);
    update();
}

void ParkingLotWidget::highlightSpot(const QString& code) {
    for (auto& sv : spotVisuals) {
        if (sv.code == code) {
            highlightedSpotId = sv.spotId;
            update();
            return;
        }
    }
}

void ParkingLotWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    recalculateLayout();
}

void ParkingLotWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor(245, 246, 250));

    painter.setPen(QColor(44, 62, 80));
    QFont titleFont = painter.font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.drawText(QRect(margin, 10, width() - 2 * margin, 30),
        Qt::AlignCenter, "停车场平面图  (右键快捷操作)");

    QString lastZone;
    int zoneIdx = 0;
    int lastY = 0;

    for (const auto& sv : spotVisuals) {
        if (sv.zone != lastZone) {
            if (!lastZone.isEmpty()) {
                lastY = sv.rect.y() - 24;
            }
            lastZone = sv.zone;
            zoneIdx = (sv.zone == "A") ? 0 : (sv.zone == "B") ? 1 : 2;
        }
        drawSpot(painter, sv);
        lastY = sv.rect.y() + cellSize;
    }

    for (const auto& sv : spotVisuals) {
        if (sv.zone != lastZone || &sv == &spotVisuals.first()) {
            // Draw zone label once
        }
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
    codeFont.setPointSize(8);
    codeFont.setBold(true);
    painter.setFont(codeFont);

    QRect textRect = spot.rect.adjusted(0, 2, 0, -spot.rect.height() / 2);
    painter.drawText(textRect, Qt::AlignCenter, spot.code);

    if (spot.status == SpotStatus::Occupied && !spot.plate.isEmpty()) {
        QFont plateFont = painter.font();
        plateFont.setPointSize(7);
        plateFont.setBold(false);
        painter.setFont(plateFont);
        QRect plateRect = spot.rect.adjusted(2, spot.rect.height() / 2 - 2, -2, -4);
        painter.drawText(plateRect, Qt::AlignCenter, spot.plate.mid(0, 5));
    } else {
        QFont iconFont = painter.font();
        iconFont.setPointSize(9);
        painter.setFont(iconFont);
        QRect iconRect = spot.rect.adjusted(0, spot.rect.height() / 2 - 4, 0, 0);
        QString icon = (spot.status == SpotStatus::Empty) ? "✓" : "⏳";
        painter.drawText(iconRect, Qt::AlignCenter, icon);
    }
}

void ParkingLotWidget::drawLegend(QPainter& painter) {
    int legendY = height() - legendHeight;
    int legendX = margin;

    painter.setPen(Qt::NoPen);

    painter.setBrush(emptyColor);
    painter.drawRoundedRect(legendX, legendY, 14, 14, 3, 3);
    painter.setPen(QColor(44, 62, 80));
    QFont legendFont = painter.font();
    legendFont.setPointSize(9);
    painter.setFont(legendFont);
    painter.drawText(legendX + 18, legendY + 12, "空闲");

    legendX += 60;
    painter.setPen(Qt::NoPen);
    painter.setBrush(reservedColor);
    painter.drawRoundedRect(legendX, legendY, 14, 14, 3, 3);
    painter.setPen(QColor(44, 62, 80));
    painter.drawText(legendX + 18, legendY + 12, "预留");

    legendX += 60;
    painter.setPen(Qt::NoPen);
    painter.setBrush(occupiedColor);
    painter.drawRoundedRect(legendX, legendY, 14, 14, 3, 3);
    painter.setPen(QColor(44, 62, 80));
    painter.drawText(legendX + 18, legendY + 12, "占用");
}

void ParkingLotWidget::drawZoneLabel(QPainter& painter, const QString& zone, int y) {
    painter.setPen(QColor(44, 62, 80));
    QFont labelFont = painter.font();
    labelFont.setPointSize(10);
    labelFont.setBold(true);
    painter.setFont(labelFont);
    painter.drawText(QRect(margin, y, 200, 20), Qt::AlignLeft, "区域 " + zone);
}

void ParkingLotWidget::mousePressEvent(QMouseEvent* event) {
    QPoint pos = event->pos();

    for (const auto& sv : spotVisuals) {
        if (sv.rect.contains(pos)) {
            if (event->button() == Qt::RightButton) {
                showContextMenu(event->globalPosition().toPoint(), sv);
            } else {
                emit spotClicked(sv.spotId);
                highlightedSpotId = sv.spotId;
                update();
            }
            return;
        }
    }
}

void ParkingLotWidget::showContextMenu(const QPoint& pos, const SpotVisual& spot) {
    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu { background: white; border: 1px solid #ddd; padding: 4px; }"
        "QMenu::item { padding: 6px 20px; }"
        "QMenu::item:selected { background: #3498db; color: white; }"
    );

    if (spot.status == SpotStatus::Empty) {
        QAction* entryAction = menu.addAction("🚗  快捷进场");
        connect(entryAction, &QAction::triggered, this, [this, spot]() {
            emit quickEntry(spot.code);
        });
    } else if (spot.status == SpotStatus::Occupied) {
        QAction* exitAction = menu.addAction("🚙  快捷离场");
        connect(exitAction, &QAction::triggered, this, [this, spot]() {
            emit quickExit(spot.code, spot.plate);
        });
    } else {
        QAction* infoAction = menu.addAction("⏳  预留中...");
        infoAction->setEnabled(false);
    }

    menu.exec(pos);
}
