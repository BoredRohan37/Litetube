#pragma once
#include <QWidget>

class QVariantAnimation;

// Loading placeholder: grey blocks with a moving highlight. At most 12
// shimmer at once; any further cards paint as static grey so the CPU stays
// free for decode on the target hardware.
class SkeletonCard : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal shimmer READ shimmer WRITE setShimmer)
public:
    explicit SkeletonCard(QWidget* parent = nullptr);
    ~SkeletonCard() override;

    void setRowStyle(bool rows) { rows_ = rows; update(); }
    qreal shimmer() const { return shimmer_; }
    void setShimmer(qreal v);

    static int activeCount();
    static const int kShimmerCap = 12;

protected:
    void paintEvent(QPaintEvent*) override;
    void showEvent(QShowEvent*) override;
    void hideEvent(QHideEvent*) override;

private:
    bool shimmering_ = false;
    bool rows_ = false;
    qreal shimmer_ = 0.0;
    QVariantAnimation* anim_ = nullptr;
};
