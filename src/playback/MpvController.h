#pragma once
#include <QObject>
#include <QVariantList>
#include <QStringList>
struct mpv_handle;

class MpvController : public QObject {
    Q_OBJECT
public:
    explicit MpvController(QObject* p = nullptr) : QObject(p) {}
    ~MpvController() override { shutdown(); }
    bool init(quintptr wid, QString* err = nullptr);
    void shutdown();
    bool isReady() const { return h_ != nullptr; }

    bool load(const QString& video, const QString& audio, double start = 0, bool loop = false);
    void applySubtitleStyle();
    void stop();
    void setPause(bool p);
    bool paused() const;
    void seekAbs(double s);
    void seekRel(double d);
    void setVolume(int v);
    void setMute(bool m);
    bool muted() const;
    void setSpeed(double s);
    void setAudioTrack(int id);
    void setSubTrack(int id);               // 0 = off
    void addSubtitle(const QString& url, const QString& title, const QString& lang);
    void setSubDelay(double d);
    double subDelay() const;
    void setAspect(const QString& a);       // "" = automatic, "16:9", "4:3", "1:1"
    void setLoop(bool on);
    double position() const { return pos_; }
    double duration() const { return dur_; }
    QString statsText() const;
    QString version() const;
    QString prop(const char* name) const;

signals:
    void positionChanged(double pos, double dur);
    void pauseChanged(bool paused);
    void bufferingChanged(bool buffering);
    void fileLoaded();
    void playbackEnded();
    void errorOccurred(const QString& msg);
    void tracksChanged(const QVariantList& tracks);   // maps: type,id,lang,title,selected
    void inputAction(const QString& action);          // keys/mouse captured by the mpv window

private slots:
    void onWakeup();
private:
    void emitTracks();
    void setStr(const char* n, const QString& v);
    mpv_handle* h_ = nullptr;
    double pos_ = 0, dur_ = 0;
    bool eof_ = false;
    QString pendingAudio_;
};
