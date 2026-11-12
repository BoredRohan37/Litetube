#include "Theme.h"
#include <QPixmap>
#include <QPainter>
namespace Theme {
QIcon appIcon() {
    QIcon ic;
    for (int s : {16, 32, 48, 64}) {
        QPixmap pm(s, s); pm.fill(Qt::transparent);
        QPainter p(&pm); p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(QColor("#cc0000")); p.setPen(Qt::NoPen);
        p.drawRoundedRect(QRectF(0, s * 0.18, s, s * 0.64), s * 0.18, s * 0.18);
        p.setBrush(Qt::white);
        QPolygonF tri; tri << QPointF(s * 0.40, s * 0.34) << QPointF(s * 0.40, s * 0.66) << QPointF(s * 0.68, s * 0.5);
        p.drawPolygon(tri);
        ic.addPixmap(pm);
    }
    return ic;
}
}
