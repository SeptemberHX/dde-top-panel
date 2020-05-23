/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "appdragwidget.h"

#include "../appitem.h"

class AppGraphicsObject : public QGraphicsObject
{
public:
    AppGraphicsObject(QGraphicsItem *parent = Q_NULLPTR) : QGraphicsObject(parent) {}
    ~AppGraphicsObject() { }

    void setAppPixmap(QPixmap pix)
    {
        m_appPixmap = pix;
        resetProperty();
        update();
    }

    void resetProperty()
    {
        setScale(1.0);
        setRotation(0);
        setOpacity(1.0);
        update();
    }

    QRectF boundingRect() const Q_DECL_OVERRIDE
    {
        return m_appPixmap.rect();
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE
    {
        Q_ASSERT(!m_appPixmap.isNull());

        painter->drawPixmap(QPoint(0, 0), m_appPixmap);
    }

private:
    QPixmap m_appPixmap;
};

AppDragWidget::AppDragWidget(QWidget *parent) :
    QGraphicsView(parent),
    m_object(new AppGraphicsObject),
    m_scene(new QGraphicsScene(this)),
    m_followMouseTimer(new QTimer(this)),
    m_animScale(new QPropertyAnimation(m_object, "scale", this)),
    m_animRotation(new QPropertyAnimation(m_object, "rotation", this)),
    m_animOpacity(new QPropertyAnimation(m_object, "opacity", this)),
    m_animGroup(new QParallelAnimationGroup(this)),
    m_goBackAnim(new QPropertyAnimation(this, "pos", this))
{
    m_scene->addItem(m_object);
    setScene(m_scene);

    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    setAttribute(Qt::WA_TranslucentBackground);
    viewport()->setAutoFillBackground(false);
    setFrameShape(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);

    setAcceptDrops(true);

    initAnimations();

    m_followMouseTimer->setSingleShot(false);
    m_followMouseTimer->setInterval(1);
    connect(m_followMouseTimer, &QTimer::timeout, [this] {
                QPoint destPos = QCursor::pos();
                move(destPos.x() - width() / 2, destPos.y() - height() / 2);
            });
    m_followMouseTimer->start();
}

AppDragWidget::~AppDragWidget() {
}

void AppDragWidget::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    // hide widget when receiving mouseMoveEvent because this means drag-and-drop has been finished
    if (m_goBackAnim->state() != QPropertyAnimation::State::Running
            && m_animGroup->state() != QParallelAnimationGroup::Running) {
        hide();
    }
}

void AppDragWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void AppDragWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (isRemoveAble()) {
        m_object->setOpacity(0.5);
        m_animOpacity->setStartValue(0.5);
    } else {
        m_object->setOpacity(1.0);
        m_animOpacity->setStartValue(1.0);
    }
}

void AppDragWidget::dropEvent(QDropEvent *event)
{
    m_followMouseTimer->stop();

    if (isRemoveAble()) {
        showRemoveAnimation();
        AppItem *appItem = static_cast<AppItem *>(event->source());
        appItem->undock();
    } else {
        showGoBackAnimation();;
    }
}

void AppDragWidget::hideEvent(QHideEvent *event)
{
    deleteLater();
}

void AppDragWidget::setAppPixmap(const QPixmap &pix)
{
    // QSize(3, 3) to fix pixmap be cliped
    setFixedSize(pix.size() + QSize(3, 3));

    m_object->setAppPixmap(pix);
    m_object->setTransformOriginPoint(pix.rect().center());
}

void AppDragWidget::setDockInfo(Dock::Position dockPosition, const QRect &dockGeometry)
{
    m_dockPosition = dockPosition;
    m_dockGeometry = dockGeometry;
}

void AppDragWidget::setOriginPos(const QPoint position)
{
    m_originPoint = position;
}

void AppDragWidget::initAnimations()
{
    m_animScale->setDuration(300);
    m_animScale->setStartValue(1.0);
    m_animScale->setEndValue(0.0);

    m_animRotation->setDuration(300);
    m_animRotation->setStartValue(0);
    m_animRotation->setEndValue(90);

    m_animOpacity->setDuration(300);
    m_animOpacity->setStartValue(1.0);
    m_animOpacity->setEndValue(0.0);

    m_animGroup->addAnimation(m_animScale);
    m_animGroup->addAnimation(m_animRotation);
    m_animGroup->addAnimation(m_animOpacity);

    connect(m_animGroup, &QParallelAnimationGroup::stateChanged,
            this, &AppDragWidget::onRemoveAnimationStateChanged);
    connect(m_goBackAnim, &QPropertyAnimation::finished, this, &AppDragWidget::hide);
}

void AppDragWidget::showRemoveAnimation()
{
    if (m_animGroup->state() == QParallelAnimationGroup::Running) {
        m_animGroup->stop();
    }
    m_object->resetProperty();
    m_animGroup->start();
}

void AppDragWidget::showGoBackAnimation()
{
    m_goBackAnim->setDuration(300);
    m_goBackAnim->setStartValue(pos());
    m_goBackAnim->setEndValue(m_originPoint);
    m_goBackAnim->start();
}

void AppDragWidget::onRemoveAnimationStateChanged(QAbstractAnimation::State newState,
        QAbstractAnimation::State oldState)
{
    if (newState == QAbstractAnimation::Stopped) {
        hide();
    }
}
bool AppDragWidget::isRemoveAble()
{
    const QPoint &p = QCursor::pos();
    switch (m_dockPosition) {
        case Dock::Position::Left:
            if ((p.x() - m_dockGeometry.topRight().x()) > (m_dockGeometry.width() * 3)) {
                return true;
            }
            break;
        case Dock::Position::Top:
            if ((p.y() - m_dockGeometry.bottomLeft().y()) > (m_dockGeometry.height() * 3)) {
                return true;
            }
            break;
        case Dock::Position::Right:
            if ((m_dockGeometry.topLeft().x() - p.x()) > (m_dockGeometry.width() * 3)) {
                return true;
            }
            break;
        case Dock::Position::Bottom:
            if ((m_dockGeometry.topLeft().y() - p.y()) > (m_dockGeometry.height() * 3)) {
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}
