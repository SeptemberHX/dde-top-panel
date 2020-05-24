//
// Created by septemberhx on 2020/5/23.
//

#include "TopPanelSettings.h"
#include "utils.h"
#include <QApplication>
#include <QScreen>

#define EffICIENT_DEFAULT_HEIGHT 40
#define WINDOW_MAX_SIZE          100
#define WINDOW_MIN_SIZE          40

extern const QPoint rawXPosition(const QPoint &scaledPos);


TopPanelSettings::TopPanelSettings(QWidget *parent)
        : QObject(parent)
        , m_dockWindowSize(EffICIENT_DEFAULT_HEIGHT)
        , m_position(Top)
        , m_displayMode(Dock::Efficient)
        , m_displayInter(new DBusDisplay(this))
{
    m_primaryRawRect = m_displayInter->primaryRawRect();
    m_screenRawHeight = m_displayInter->screenRawHeight();
    m_screenRawWidth = m_displayInter->screenRawWidth();

    calculateWindowConfig();
}

void TopPanelSettings::calculateWindowConfig()
{
    if (m_displayMode == Dock::Efficient) {
        if (m_dockWindowSize > WINDOW_MAX_SIZE || m_dockWindowSize < WINDOW_MIN_SIZE) {
            m_dockWindowSize = EffICIENT_DEFAULT_HEIGHT;
        }

        m_mainWindowSize.setHeight(m_dockWindowSize);
        m_mainWindowSize.setWidth(primaryRect().width() - 40);
    } else {
        Q_ASSERT(false);
    }

    resetFrontendGeometry();
}

const QRect TopPanelSettings::primaryRect() const
{
    QRect rect = m_primaryRawRect;
    qreal scale = qApp->primaryScreen()->devicePixelRatio();

    rect.setWidth(std::round(qreal(rect.width()) / scale));
    rect.setHeight(std::round(qreal(rect.height()) / scale));

    return rect;
}

void TopPanelSettings::resetFrontendGeometry()
{
    const QRect r = this->windowRect(m_position, false);
    const qreal ratio = dockRatio();
    const QPoint p = rawXPosition(r.topLeft());
    const uint w = r.width() * ratio;
    const uint h = r.height() * ratio;

    m_frontendRect = QRect(p.x(), p.y(), w, h);
}

const QRect TopPanelSettings::windowRect(const Position position, const bool hide) const
{
    QSize size = m_mainWindowSize;
    if (hide) {
        switch (position) {
            case Top:
            case Bottom:    size.setHeight(2);      break;
            case Left:
            case Right:     size.setWidth(2);       break;
        }
    }

    const QRect primaryRect = this->primaryRect();
    const int offsetX = (primaryRect.width() - size.width()) / 2;
    const int offsetY = (primaryRect.height() - size.height()) / 2;
    int margin = hide ?  0 : this->dockMargin();
    QPoint p(0, 0);
    switch (position) {
        case Top:
            p = QPoint(offsetX, margin);
            break;
        case Left:
            p = QPoint(margin, offsetY);
            break;
        case Right:
            p = QPoint(primaryRect.width() - size.width() - margin, offsetY);
            break;
        case Bottom:
            p = QPoint(offsetX, primaryRect.height() - size.height() - margin);
            break;
        default: Q_UNREACHABLE();
    }

    return QRect(primaryRect.topLeft() + p, size);
}

const int TopPanelSettings::dockMargin() const
{
    return 0;
}

qreal TopPanelSettings::dockRatio() const
{
    QScreen const *screen = Utils::screenAtByScaled(m_frontendRect.center());

    return screen ? screen->devicePixelRatio() : qApp->devicePixelRatio();
}

TopPanelSettings &TopPanelSettings::Instance()
{
    static TopPanelSettings settings;
    return settings;
}