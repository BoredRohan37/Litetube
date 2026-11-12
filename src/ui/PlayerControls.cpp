#include "PlayerControls.h"
#include "playback/PlaybackSession.h"
#include "services/AppSettings.h"
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QMenu>
#include <QActionGroup>

static QToolButton* mk(const QString& t, const QString& tip) {
    auto* b = new QToolButton; b->setObjectName("ctl"); b->setText(t); b->setToolTip(tip); b->setAutoRaise(true); return b;
}

PlayerControls::PlayerControls(PlaybackSession* s, bool mini, QWidget* p) : QWidget(p), s_(s), mini_(mini) {
    auto* l = new QHBoxLayout(this);
    l->setContentsMargins(4, 2, 4, 2); l->setSpacing(2);
    play_ = mk("▶", "Play / Pause (Space)"); next_ = mk("⏭", "Next"); next_->hide();
    seek_ = new ClickSlider(Qt::Horizontal); seek_->setRange(0, 0);
    time_ = new QLabel("0:00 / 0:00"); time_->setMinimumWidth(96);
    buffering_ = new QLabel("Buffering…"); buffering_->setStyleSheet("color:#ff6666"); buffering_->hide();
    mute_ = mk("🔊", "Mute (M)");
    vol_ = new ClickSlider(Qt::Horizontal); vol_->setRange(0, 100); vol_->setFixedWidth(80);
    vol_->setValue(AppSettings::instance().num("volume"));
    cc_ = mk("CC", "Subtitles"); gear_ = mk("⚙", "Settings"); compact_ = mk("⧉", "Mini player (always on top)"); full_ = mk("⛶", "Fullscreen (F)");
    l->addWidget(play_); l->addWidget(next_); l->addWidget(seek_, 1); l->addWidget(buffering_); l->addWidget(time_);
    l->addWidget(mute_); l->addWidget(vol_);
    if (!mini) { l->addWidget(cc_); l->addWidget(gear_); }
    l->addWidget(compact_); l->addWidget(full_);
    if (mini) { cc_->hide(); gear_->hide(); }

    gearMenu_ = new QMenu(this); ccMenu_ = new QMenu(this);
    connect(gearMenu_, &QMenu::aboutToShow, this, &PlayerControls::buildSettingsMenu);
    connect(ccMenu_, &QMenu::aboutToShow, this, &PlayerControls::buildSubMenu);
    gear_->setMenu(gearMenu_); gear_->setPopupMode(QToolButton::InstantPopup);
    cc_->setMenu(ccMenu_); cc_->setPopupMode(QToolButton::InstantPopup);

    MpvController* m = s_->mpv();
    connect(play_, &QToolButton::clicked, this, [this, m] { m->setPause(!m->paused()); });
    connect(next_, &QToolButton::clicked, this, &PlayerControls::nextRequested);
    connect(compact_, &QToolButton::clicked, this, &PlayerControls::requestCompact);
    connect(full_, &QToolButton::clicked, this, &PlayerControls::toggleFullscreen);
    connect(mute_, &QToolButton::clicked, this, [this, m] {
        const bool nm = !m->muted(); m->setMute(nm); AppSettings::instance().set("muted", nm); mute_->setText(nm ? "🔇" : "🔊");
    });
    connect(vol_, &QSlider::valueChanged, this, [m](int v) { m->setVolume(v); AppSettings::instance().set("volume", v); });
    connect(seek_, &QSlider::sliderPressed, this, [this] { dragging_ = true; });
    connect(seek_, &QSlider::sliderReleased, this, [this, m] { dragging_ = false; m->seekAbs(seek_->value() / 10.0); });
    connect(seek_, &QSlider::sliderMoved, this, [this](int v) { time_->setText(QString("%1 / %2").arg(Format::clockTime(v / 10.0), Format::clockTime(seek_->maximum() / 10.0))); });
    connect(m, &MpvController::pauseChanged, this, [this](bool p) { play_->setText(p ? "▶" : "⏸"); });
    connect(s_, &PlaybackSession::started, this, [this] { play_->setText("⏸"); mute_->setText(s_->mpv()->muted() ? "🔇" : "🔊"); });
    connect(s_, &PlaybackSession::position, this, &PlayerControls::onPosition);
    connect(s_, &PlaybackSession::buffering, this, [this](bool b) { buffering_->setVisible(b); });
    connect(s_, &PlaybackSession::loading, this, [this] { seek_->setRange(0, 0); time_->setText("0:00 / 0:00"); buffering_->setText("Loading…"); buffering_->show(); });
    connect(s_, &PlaybackSession::started, this, [this] { buffering_->setText("Buffering…"); buffering_->hide(); });
}
void PlayerControls::setNextVisible(bool v) { next_->setVisible(v); }
void PlayerControls::onPosition(double pos, double dur) {
    if (dur > 0 && seek_->maximum() != int(dur * 10)) seek_->setRange(0, int(dur * 10));
    if (!dragging_) { seek_->setValue(int(pos * 10)); time_->setText(QString("%1 / %2").arg(Format::clockTime(pos), Format::clockTime(dur))); }
}
void PlayerControls::buildSettingsMenu() {
    gearMenu_->clear();
    QMenu* q = gearMenu_->addMenu("Quality");
    auto* g = new QActionGroup(q);
    QAction* a = q->addAction("Default (" + QString::number(AppSettings::instance().num("quality")) + "p)", this, [this] { s_->setQuality(AppSettings::instance().num("quality")); });
    a->setCheckable(true); g->addAction(a);
    for (int h : s_->resolved().heights()) {
        QAction* qa = q->addAction(QString::number(h) + "p", this, [this, h] { s_->setQuality(h); });
        qa->setCheckable(true); qa->setChecked(h == s_->currentHeight()); g->addAction(qa);
    }
    QMenu* sp = gearMenu_->addMenu("Speed");
    for (double v : {0.5, 0.75, 1.0, 1.25, 1.5, 2.0}) {
        QAction* sa = sp->addAction(QString::number(v) + "x", this, [this, v] { s_->mpv()->setSpeed(v); AppSettings::instance().set("speed", v); });
        sa->setCheckable(true); sa->setChecked(qFuzzyCompare(v, s_->mpv()->prop("speed").toDouble()));
    }
    QMenu* au = gearMenu_->addMenu("Audio track");
    int n = 0;
    for (const QVariant& t : s_->tracks()) {
        const QVariantMap m = t.toMap();
        if (m["type"].toString() != "audio") continue;
        ++n;
        const int id = m["id"].toInt();
        QAction* aa = au->addAction(QString("Track %1 %2").arg(id).arg(m["lang"].toString()), this, [this, id] { s_->mpv()->setAudioTrack(id); });
        aa->setCheckable(true); aa->setChecked(m["selected"].toBool());
    }
    if (!n) au->addAction("Only one track")->setEnabled(false);
    QMenu* ar = gearMenu_->addMenu("Aspect ratio");
    for (const QString& r : {"Auto", "16:9", "4:3", "1:1", "21:9"})
        ar->addAction(r, this, [this, r] { s_->mpv()->setAspect(r == "Auto" ? QString() : r); });
    QAction* st = gearMenu_->addAction("Playback statistics");
    st->setCheckable(true); st->setChecked(AppSettings::instance().flag("showStats"));
    connect(st, &QAction::toggled, this, [this](bool on) { AppSettings::instance().set("showStats", on); emit statsToggled(on); });
}
void PlayerControls::buildSubMenu() {
    ccMenu_->clear();
    ccMenu_->addAction("Off", this, [this] { s_->selectSubtitle(-1); });
    const auto subs = s_->resolved().subs;
    for (int i = 0; i < subs.size(); ++i) ccMenu_->addAction(subs[i].label, this, [this, i] { s_->selectSubtitle(i); });
    if (subs.isEmpty()) ccMenu_->addAction("No subtitles available")->setEnabled(false);
    ccMenu_->addSeparator();
    ccMenu_->addAction("Subtitles earlier (−0.1 s)", this, [this] { s_->shiftSubtitles(-0.1); });
    ccMenu_->addAction("Subtitles later (+0.1 s)", this, [this] { s_->shiftSubtitles(0.1); });
    ccMenu_->addAction("Reset subtitle sync", this, [this] { s_->shiftSubtitles(0); });
}
