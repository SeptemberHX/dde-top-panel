#include "spliteranimated.h"

#include <QPainter>
#include <QDebug>

#define OpacityMax 0.3
#define OpacityMini 0.1

SpliterAnimated::SpliterAnimated(QWidget *parent)
    : QWidget(parent),
      m_sizeAnimation(new QVariantAnimation(this)),
      m_currentOpacity(OpacityMini),
      m_dockPosition(Dock::Position::Bottom)
{
    m_sizeAnimation->setDuration(500);
    m_sizeAnimation->setLoopCount(-1);

    connect(m_sizeAnimation, &QVariantAnimation::valueChanged, this, &SpliterAnimated::onSizeAnimationValueChanged);
}

void SpliterAnimated::setStartValue(const QVariant &value)
{
    m_sizeAnimation->setStartValue(value);
}

void SpliterAnimated::setEndValue(const QVariant &value)
{
    m_sizeAnimation->setEndValue(value);
}

void SpliterAnimated::startAnimation()
{
    if (!isVisible()) {
        return;
    }

    m_currentOpacity = OpacityMini;

    if (m_dockPosition == Dock::Position::Top || m_dockPosition == Dock::Position::Bottom) {
        m_opacityChangeStep = (OpacityMax - OpacityMini) /
                (m_sizeAnimation->endValue().toSizeF().height() -
                 m_sizeAnimation->startValue().toSizeF().height());
    } else {
        m_opacityChangeStep = (OpacityMax - OpacityMini) /
                (m_sizeAnimation->endValue().toSizeF().width() -
                 m_sizeAnimation->startValue().toSizeF().width());
    }

    m_sizeAnimation->start();

    update();
}

void SpliterAnimated::stopAnimation()
{
    m_sizeAnimation->stop();
    m_currentOpacity = OpacityMini;

    update();
}

void SpliterAnimated::setDockPosition(const Dock::Position position)
{
    m_dockPosition = position;
}

void SpliterAnimated::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QSizeF destSize = m_sizeAnimation->state() == QVariantAnimation::Running
            ? m_sizeAnimation->currentValue().toSizeF()
            : m_sizeAnimation->startValue().toSizeF();

    QRectF destRect(rect().topLeft(), destSize);
    destRect.moveCenter(QRectF(rect()).center());

    QPainterPath path;
    path.addRect(destRect);

    QPainter painter(this);
    painter.setOpacity(m_currentOpacity);
    painter.fillPath(path, QColor::fromRgb(255, 255, 255));
}

void SpliterAnimated::onSizeAnimationValueChanged(const QVariant &value)
{
    if (m_sizeAnimation->direction() == QVariantAnimation::Direction::Forward) {
        m_currentOpacity += m_opacityChangeStep;
        if (m_currentOpacity > OpacityMax) {
            m_currentOpacity = OpacityMax;
        }
    } else {
        m_currentOpacity -= m_opacityChangeStep;
        if (m_currentOpacity < OpacityMini) {
            m_currentOpacity = OpacityMini;
        }
    }

    if (value == m_sizeAnimation->endValue()) {
        m_sizeAnimation->setDirection(QVariantAnimation::Direction::Backward);
    } else if (value == m_sizeAnimation->startValue()) {
        m_sizeAnimation->setDirection(QVariantAnimation::Direction::Forward);
    }

    update();
}
