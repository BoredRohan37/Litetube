#pragma once
#include <Qt>
#include <QString>
#include <QElapsedTimer>

// Turns wheel / key input into Shorts actions: next, prev, pause, mute, exit.
class ShortsNavigation {
public:
    void setCooldown(int ms) { cooldown_ = qMax(100, ms); }
    // Returns "" when ignored (still in cooldown).
    QString fromWheel(int delta) {
        if (delta == 0) return QString();
        if (timer_.isValid() && timer_.elapsed() < cooldown_) return QString();
        timer_.restart();
        return delta < 0 ? QStringLiteral("next") : QStringLiteral("prev");
    }
    QString fromAction(const QString& a) {      // actions coming from mpv's captured input
        if (a == "wheeldown") return fromWheel(-1);
        if (a == "wheelup") return fromWheel(1);
        if (a == "down" || a == "pgdown") return "next";
        if (a == "up" || a == "pgup") return "prev";
        if (a == "space" || a == "click") return "pause";
        if (a == "mute") return "mute";
        if (a == "esc") return "exit";
        return QString();
    }
    static QString fromKey(int key) {
        switch (key) {
        case Qt::Key_Down: case Qt::Key_PageDown: return "next";
        case Qt::Key_Up: case Qt::Key_PageUp: return "prev";
        case Qt::Key_Space: return "pause";
        case Qt::Key_M: return "mute";
        case Qt::Key_Escape: return "exit";
        default: return QString();
        }
    }
private:
    int cooldown_ = 450;
    QElapsedTimer timer_;
};
