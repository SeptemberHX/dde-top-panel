#include <QPixmap>
#include <QImageReader>
#include <QApplication>
#include <QScreen>

namespace Utils {
    static QPixmap renderSVG(const QString &path, const QSize &size, const qreal devicePixelRatio) {
    QImageReader reader;
    QPixmap pixmap;
    reader.setFileName(path);
    if (reader.canRead()) {
        reader.setScaledSize(size * devicePixelRatio);
        pixmap = QPixmap::fromImage(reader.read());
        pixmap.setDevicePixelRatio(devicePixelRatio);
    }
    else {
        pixmap.load(path);
    }

    return pixmap;
    }

    static QScreen * screenAt(const QPoint &point) {
        for (QScreen *screen : qApp->screens()) {
            const QRect r { screen->geometry() };
            const QRect rect { r.topLeft(), r.size() * screen->devicePixelRatio() };
            if (rect.contains(point)) {
                return screen;
            }
        }

        return nullptr;
    }

    static QScreen * screenAtByScaled(const QPoint &point) {
        for (QScreen *screen : qApp->screens()) {
            if (screen->geometry().contains(point)) {
                return screen;
            }
        }

        return nullptr;
    }
}
