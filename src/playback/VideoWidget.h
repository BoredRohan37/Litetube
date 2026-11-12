#pragma once
#include <QWidget>
// Native child window that libmpv renders into (Direct3D on Windows).
class VideoWidget : public QWidget {
    Q_OBJECT
public:
    explicit VideoWidget(QWidget* p = nullptr) : QWidget(p) {
        setAttribute(Qt::WA_NativeWindow);
        setAttribute(Qt::WA_DontCreateNativeAncestors);
        setAttribute(Qt::WA_OpaquePaintEvent);
        setAttribute(Qt::WA_NoSystemBackground);
        setFocusPolicy(Qt::StrongFocus);
        setMinimumSize(160, 90);
        QPalette pal = palette(); pal.setColor(QPalette::Window, Qt::black); setPalette(pal); setAutoFillBackground(true);
    }
    quintptr nativeId() { return quintptr(winId()); }
    QPaintEngine* paintEngine() const override { return nullptr; }
};
