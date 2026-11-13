#pragma once
class QStackedWidget;

// Slide one stacked page out and the next one in. Only "pos" is animated, and
// geometry is never touched, so the GMA 3100 never re-lays-out during the move.
namespace PageTransition {
void slideTo(QStackedWidget* stack, int index, int durationMs = 260);
bool running();
}
