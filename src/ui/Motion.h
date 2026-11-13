#pragma once
#include <QApplication>
#include <QEasingCurve>
#include <QStyle>

#include "services/AppSettings.h"

// Central motion rules. Durations come from Section 4 of the design brief:
// standard 180-260ms, emphasis 300-420ms, ambient 700ms, all OutCubic
// (InOutQuad for ambient). Animations are suppressed entirely when the user
// disables Qt effects or turns on Performance mode.
namespace Motion {

enum Duration {
    Fast = 140,
    Standard = 200,
    Emphasis = 320,
    Ripple = 420,
    KenBurns = 600,
    Ambient = 700
};

inline QEasingCurve outCubic() { return QEasingCurve(QEasingCurve::OutCubic); }
inline QEasingCurve inOutQuad() { return QEasingCurve(QEasingCurve::InOutQuad); }

inline bool enabled() {
    if (AppSettings::instance().flag("perfMode")) return false;
    return QApplication::isEffectEnabled(Qt::UI_Animate);
}

// Zero when animations are off, so callers can feed it straight into a timer.
inline int ms(int duration) { return enabled() ? duration : 0; }

} // namespace Motion
