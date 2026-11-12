#pragma once
#include <QString>
#include <QRegularExpression>
namespace TextUtil {
// HTML-escape and turn m:ss / h:mm:ss into seek:<seconds> links.
inline QString linkify(QString t) {
    t.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;").replace("\n", "<br>");
    static const QRegularExpression ts("\\b((?:\\d{1,2}:)?\\d{1,2}:\\d{2})\\b");
    QString out; int last = 0;
    auto it = ts.globalMatch(t);
    while (it.hasNext()) {
        const auto m = it.next();
        int sec = 0; for (const QString& x : m.captured(1).split(':')) sec = sec * 60 + x.toInt();
        out += t.mid(last, m.capturedStart() - last) + QString("<a style='color:#3ea6ff;text-decoration:none' href='seek:%1'>%2</a>").arg(sec).arg(m.captured(1));
        last = m.capturedEnd();
    }
    return out + t.mid(last);
}
}
