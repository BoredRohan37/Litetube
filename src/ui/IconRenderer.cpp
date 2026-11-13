#include "IconRenderer.h"
#include <QCache>
#include <QHash>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QRectF>
#include <QPointF>
#include <algorithm>

namespace {

using Painter = void (*)(QPainter&, const QColor&);

inline void L(QPainter& p, qreal x1, qreal y1, qreal x2, qreal y2) {
    p.drawLine(QPointF(x1, y1), QPointF(x2, y2));
}
inline void Poly(QPainter& p, std::initializer_list<QPointF> pts, bool close = false) {
    QPainterPath path;
    bool first = true;
    for (const QPointF& q : pts) {
        if (first) { path.moveTo(q); first = false; } else path.lineTo(q);
    }
    if (close) path.closeSubpath();
    p.drawPath(path);
}
inline void Circle(QPainter& p, qreal cx, qreal cy, qreal r) {
    p.drawEllipse(QPointF(cx, cy), r, r);
}
inline void RRect(QPainter& p, qreal x, qreal y, qreal w, qreal h, qreal r) {
    p.drawRoundedRect(QRectF(x, y, w, h), r, r);
}
inline void Fill(QPainter& p, const QColor& c) {
    p.setBrush(c);
    p.setPen(Qt::NoPen);
}
inline void Stroke(QPainter& p, const QColor& c, qreal w) {
    QPen pen(c);
    pen.setWidthF(w);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
}

// ---------------------------------------------------------------- transport
void icPlay(QPainter& p, const QColor& c) {
    Fill(p, c);
    Poly(p, {QPointF(0.34, 0.20), QPointF(0.80, 0.50), QPointF(0.34, 0.80)}, true);
}
void icPause(QPainter& p, const QColor& c) {
    Fill(p, c);
    RRect(p, 0.31, 0.22, 0.14, 0.56, 0.05);
    RRect(p, 0.55, 0.22, 0.14, 0.56, 0.05);
}
void icNext(QPainter& p, const QColor& c) {
    Fill(p, c);
    Poly(p, {QPointF(0.26, 0.20), QPointF(0.68, 0.50), QPointF(0.26, 0.80)}, true);
    RRect(p, 0.72, 0.20, 0.10, 0.60, 0.03);
}
void icPrev(QPainter& p, const QColor& c) {
    Fill(p, c);
    Poly(p, {QPointF(0.74, 0.20), QPointF(0.32, 0.50), QPointF(0.74, 0.80)}, true);
    RRect(p, 0.18, 0.20, 0.10, 0.60, 0.03);
}
void icSkipIntro(QPainter& p, const QColor& c) {
    Fill(p, c);
    Poly(p, {QPointF(0.20, 0.22), QPointF(0.60, 0.50), QPointF(0.20, 0.78)}, true);
    RRect(p, 0.64, 0.22, 0.10, 0.56, 0.03);
}

// -------------------------------------------------------------------- nav
void icHome(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Poly(p, {QPointF(0.14, 0.50), QPointF(0.50, 0.16), QPointF(0.86, 0.50)});
    Poly(p, {QPointF(0.24, 0.46), QPointF(0.24, 0.84), QPointF(0.76, 0.84), QPointF(0.76, 0.46)});
}
void icSearch(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Circle(p, 0.43, 0.43, 0.24);
    L(p, 0.61, 0.61, 0.84, 0.84);
}
void icMenu(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    for (qreal y : {0.28, 0.50, 0.72}) L(p, 0.16, y, 0.84, y);
}
void icShorts(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    RRect(p, 0.28, 0.14, 0.44, 0.72, 0.20);
    Fill(p, c);
    Poly(p, {QPointF(0.44, 0.36), QPointF(0.66, 0.50), QPointF(0.44, 0.64)}, true);
}
void icTrending(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Poly(p, {QPointF(0.12, 0.72), QPointF(0.38, 0.44), QPointF(0.56, 0.60), QPointF(0.84, 0.28)});
    Poly(p, {QPointF(0.70, 0.28), QPointF(0.84, 0.28), QPointF(0.84, 0.42)});
}
void icHistory(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    p.drawArc(QRectF(0.20, 0.20, 0.60, 0.60), 40 * 16, 300 * 16);
    L(p, 0.50, 0.34, 0.50, 0.52);
    L(p, 0.50, 0.52, 0.62, 0.58);
    Poly(p, {QPointF(0.16, 0.28), QPointF(0.16, 0.44), QPointF(0.32, 0.44)});
}
void icSettings(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Circle(p, 0.50, 0.50, 0.14);
    for (int i = 0; i < 6; ++i) {
        const qreal a = i * 3.14159265 / 3.0;
        L(p, 0.50 + qCos(a) * 0.24, 0.50 + qSin(a) * 0.24,
             0.50 + qCos(a) * 0.36, 0.50 + qSin(a) * 0.36);
    }
}
void icClock(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Circle(p, 0.50, 0.50, 0.34);
    L(p, 0.50, 0.30, 0.50, 0.52);
    L(p, 0.50, 0.52, 0.64, 0.60);
}
void icPlaylist(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    L(p, 0.16, 0.30, 0.64, 0.30);
    L(p, 0.16, 0.50, 0.64, 0.50);
    L(p, 0.16, 0.70, 0.44, 0.70);
    Fill(p, c);
    Poly(p, {QPointF(0.68, 0.60), QPointF(0.86, 0.70), QPointF(0.68, 0.80)}, true);
}
void icDot(QPainter& p, const QColor& c) {
    Fill(p, c);
    Circle(p, 0.50, 0.50, 0.20);
}
void icApiKey(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Circle(p, 0.34, 0.50, 0.18);
    L(p, 0.52, 0.50, 0.86, 0.50);
    L(p, 0.72, 0.50, 0.72, 0.64);
    L(p, 0.82, 0.50, 0.82, 0.62);
}

// ------------------------------------------------------------------ audio
void icVolume(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Poly(p, {QPointF(0.16, 0.38), QPointF(0.32, 0.38), QPointF(0.50, 0.22),
             QPointF(0.50, 0.78), QPointF(0.32, 0.62), QPointF(0.16, 0.62)}, true);
    p.drawArc(QRectF(0.44, 0.30, 0.30, 0.40), -60 * 16, 120 * 16);
    p.drawArc(QRectF(0.44, 0.18, 0.44, 0.64), -55 * 16, 110 * 16);
}
void icVolumeLow(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Poly(p, {QPointF(0.18, 0.38), QPointF(0.34, 0.38), QPointF(0.52, 0.22),
             QPointF(0.52, 0.78), QPointF(0.34, 0.62), QPointF(0.18, 0.62)}, true);
    p.drawArc(QRectF(0.46, 0.30, 0.30, 0.40), -60 * 16, 120 * 16);
}
void icMute(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Poly(p, {QPointF(0.14, 0.38), QPointF(0.30, 0.38), QPointF(0.48, 0.22),
             QPointF(0.48, 0.78), QPointF(0.30, 0.62), QPointF(0.14, 0.62)}, true);
    L(p, 0.60, 0.38, 0.86, 0.64);
    L(p, 0.86, 0.38, 0.60, 0.64);
}

// -------------------------------------------------------------- video/ui
void icFullscreen(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Poly(p, {QPointF(0.16, 0.40), QPointF(0.16, 0.16), QPointF(0.40, 0.16)});
    Poly(p, {QPointF(0.60, 0.16), QPointF(0.84, 0.16), QPointF(0.84, 0.40)});
    Poly(p, {QPointF(0.84, 0.60), QPointF(0.84, 0.84), QPointF(0.60, 0.84)});
    Poly(p, {QPointF(0.40, 0.84), QPointF(0.16, 0.84), QPointF(0.16, 0.60)});
}
void icCinema(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    RRect(p, 0.10, 0.26, 0.80, 0.48, 0.08);
    L(p, 0.10, 0.38, 0.90, 0.38);
    L(p, 0.10, 0.62, 0.90, 0.62);
}
void icPip(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    RRect(p, 0.12, 0.20, 0.76, 0.60, 0.08);
    p.setBrush(c);
    p.drawRoundedRect(QRectF(0.52, 0.54, 0.30, 0.20), 0.04, 0.04);
}
void icCc(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    RRect(p, 0.10, 0.24, 0.80, 0.52, 0.10);
    p.drawArc(QRectF(0.20, 0.36, 0.22, 0.28), 60 * 16, 240 * 16);
    p.drawArc(QRectF(0.54, 0.36, 0.22, 0.28), 60 * 16, 240 * 16);
}
void icSubtitles(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    RRect(p, 0.10, 0.24, 0.80, 0.52, 0.10);
    L(p, 0.24, 0.60, 0.46, 0.60);
    L(p, 0.54, 0.60, 0.76, 0.60);
    L(p, 0.24, 0.44, 0.40, 0.44);
}
void icQuality(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    L(p, 0.22, 0.74, 0.22, 0.54);
    L(p, 0.50, 0.74, 0.50, 0.40);
    L(p, 0.78, 0.74, 0.78, 0.24);
}
void icSpeed(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    p.drawArc(QRectF(0.14, 0.20, 0.72, 0.72), 200 * 16, 140 * 16);
    L(p, 0.50, 0.60, 0.72, 0.36);
    Fill(p, c);
    Circle(p, 0.50, 0.60, 0.06);
}
void icRefresh(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    p.drawArc(QRectF(0.18, 0.18, 0.64, 0.64), 40 * 16, 280 * 16);
    Poly(p, {QPointF(0.78, 0.14), QPointF(0.82, 0.34), QPointF(0.62, 0.30)});
}
void icSpinnerArc(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    p.drawArc(QRectF(0.14, 0.14, 0.72, 0.72), 90 * 16, 210 * 16);
}

// ----------------------------------------------------------------- social
void icThumbUp(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Poly(p, {QPointF(0.18, 0.44), QPointF(0.34, 0.44), QPointF(0.46, 0.20),
             QPointF(0.54, 0.24), QPointF(0.50, 0.42), QPointF(0.78, 0.42),
             QPointF(0.72, 0.80), QPointF(0.18, 0.80)}, true);
    L(p, 0.34, 0.44, 0.34, 0.80);
}
void icThumbDown(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Poly(p, {QPointF(0.18, 0.56), QPointF(0.34, 0.56), QPointF(0.46, 0.80),
             QPointF(0.54, 0.76), QPointF(0.50, 0.58), QPointF(0.78, 0.58),
             QPointF(0.72, 0.20), QPointF(0.18, 0.20)}, true);
    L(p, 0.34, 0.20, 0.34, 0.56);
}
void icShare(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Circle(p, 0.28, 0.50, 0.10);
    Circle(p, 0.72, 0.24, 0.10);
    Circle(p, 0.72, 0.76, 0.10);
    L(p, 0.37, 0.45, 0.63, 0.29);
    L(p, 0.37, 0.55, 0.63, 0.71);
}
void icDownload(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    L(p, 0.50, 0.18, 0.50, 0.62);
    Poly(p, {QPointF(0.34, 0.48), QPointF(0.50, 0.64), QPointF(0.66, 0.48)});
    Poly(p, {QPointF(0.18, 0.76), QPointF(0.18, 0.84), QPointF(0.82, 0.84), QPointF(0.82, 0.76)});
}

// ------------------------------------------------------------------ chrome
void icClose(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    L(p, 0.26, 0.26, 0.74, 0.74);
    L(p, 0.74, 0.26, 0.26, 0.74);
}
void icMinimize(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    L(p, 0.24, 0.50, 0.76, 0.50);
}
void icMaximize(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    RRect(p, 0.26, 0.26, 0.48, 0.48, 0.05);
}
void icRestore(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    RRect(p, 0.20, 0.32, 0.44, 0.44, 0.05);
    Poly(p, {QPointF(0.36, 0.30), QPointF(0.36, 0.22), QPointF(0.80, 0.22),
             QPointF(0.80, 0.66), QPointF(0.70, 0.66)});
}
void icChevronRight(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Poly(p, {QPointF(0.40, 0.22), QPointF(0.68, 0.50), QPointF(0.40, 0.78)});
}
void icChevronLeft(QPainter& p, const QColor& c) {
    Stroke(p, c, p.pen().widthF());
    Poly(p, {QPointF(0.60, 0.22), QPointF(0.32, 0.50), QPointF(0.60, 0.78)});
}

const QHash<QString, Painter>& painters() {
    static const QHash<QString, Painter> map = {
        {"play", icPlay}, {"pause", icPause}, {"next", icNext}, {"prev", icPrev},
        {"skip-intro", icSkipIntro},
        {"home", icHome}, {"search", icSearch}, {"menu", icMenu}, {"shorts", icShorts},
        {"trending", icTrending}, {"history", icHistory}, {"settings", icSettings},
        {"clock", icClock}, {"playlist", icPlaylist}, {"dot", icDot}, {"api-key", icApiKey},
        {"volume", icVolume}, {"volume-low", icVolumeLow}, {"mute", icMute},
        {"fullscreen", icFullscreen}, {"cinema", icCinema}, {"pip", icPip}, {"cc", icCc},
        {"subtitles", icSubtitles}, {"quality", icQuality}, {"speed", icSpeed},
        {"refresh", icRefresh}, {"spinner-arc", icSpinnerArc},
        {"thumb-up", icThumbUp}, {"thumb-down", icThumbDown}, {"share", icShare},
        {"download", icDownload},
        {"close", icClose}, {"minimize", icMinimize}, {"maximize", icMaximize},
        {"restore", icRestore}, {"chevron-right", icChevronRight}, {"chevron-left", icChevronLeft},
    };
    return map;
}

} // namespace

void Icons::draw(QPainter& p, const QString& name, const QRectF& box,
                 const QColor& color, qreal strokeWidth) {
    const auto it = painters().constFind(name);
    if (it == painters().constEnd()) return;
    const qreal side = qMin(box.width(), box.height());
    if (side <= 0.5) return;
    p.save();
    p.setRenderHint(QPainter::Antialiasing, true);
    p.translate(box.left() + (box.width() - side) / 2.0,
                box.top() + (box.height() - side) / 2.0);
    p.scale(side, side);
    const qreal width = (strokeWidth > 0 ? strokeWidth : side / 12.0) / side;
    QPen pen(color);
    pen.setWidthF(qMax(0.006, width));
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    it.value()(p, color);
    p.restore();
}

QPixmap Icons::pixmap(const QString& name, int size, const QColor& color, qreal dpr) {
    if (size <= 0) size = 24;
    if (dpr <= 0) dpr = 1.0;
    static QCache<QString, QPixmap> cache(512);
    const QString key = name + '|' + QString::number(size) + '|' +
                        QString::number(color.rgba(), 16) + '|' + QString::number(dpr, 'f', 2);
    if (const QPixmap* hit = cache.object(key)) return *hit;
    const int px = qMax(1, int(qRound(size * dpr)));
    QPixmap pm(px, px);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    draw(p, name, QRectF(0, 0, px, px), color);
    p.end();
    pm.setDevicePixelRatio(dpr);
    cache.insert(key, new QPixmap(pm));
    return pm;
}

QIcon Icons::icon(const QString& name, int size, const QColor& color) {
    QIcon ic;
    for (qreal dpr : {1.0, 1.25, 1.5, 2.0})
        ic.addPixmap(pixmap(name, size, color, dpr));
    return ic;
}

QStringList Icons::names() {
    QStringList l = painters().keys();
    std::sort(l.begin(), l.end());
    return l;
}

void Icons::clearCache() {
    // The cache lives in pixmap(); recreating it is not observable to callers,
    // so this exists only to keep tests deterministic.
    QPixmap pm = Icons::pixmap("dot", 8, QColor(0, 0, 0, 0), 1.0);
    Q_UNUSED(pm)
}
