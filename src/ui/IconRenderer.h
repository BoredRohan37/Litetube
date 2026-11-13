#pragma once
#include <QColor>
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QStringList>

class QPainter;
class QRectF;

// Every icon in LiteTube is drawn here with QPainter/QPainterPath: no image
// files, no Qt standard icons, no emoji. Geometry is normalised to a 0..1 box
// and scaled to the requested rectangle, so one definition serves all sizes.
namespace Icons {

void    draw(QPainter& p, const QString& name, const QRectF& box,
             const QColor& color, qreal strokeWidth = 0.0);
QPixmap pixmap(const QString& name, int size, const QColor& color, qreal dpr = 1.0);
QIcon   icon(const QString& name, int size, const QColor& color);

QStringList names();          // every implemented icon name
void    clearCache();         // drops the pixmap cache (used by tests)

} // namespace Icons
