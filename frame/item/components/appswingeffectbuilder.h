#ifndef SWINGEFFECT
#define SWINGEFFECT

#include <QGraphicsView>
#include <QGraphicsItemAnimation>
#include <QGraphicsPixmapItem>
#include <QTimeLine>

const static qreal Frames[] = { 0,
                                0.327013,
                                0.987033,
                                1.77584,
                                2.61157,
                                3.45043,
                                4.26461,
                                5.03411,
                                5.74306,
                                6.37782,
                                6.92583,
                                7.37484,
                                7.71245,
                                7.92557,
                                8, 7.86164,
                                7.43184,
                                6.69344,
                                5.64142,
                                4.2916,
                                2.68986,
                                0.91694,
                                -0.91694,
                                -2.68986,
                                -4.2916,
                                -5.64142,
                                -6.69344,
                                -7.43184,
                                -7.86164,
                                -8,
                                -7.86164,
                                -7.43184,
                                -6.69344,
                                -5.64142,
                                -4.2916,
                                -2.68986,
                                -0.91694,
                                0.91694,
                                2.68986,
                                4.2916,
                                5.64142,
                                6.69344,
                                7.43184,
                                7.86164,
                                8,
                                7.93082,
                                7.71592,
                                7.34672,
                                6.82071,
                                6.1458,
                                5.34493,
                                4.45847,
                                3.54153,
                                2.65507,
                                1.8542,
                                1.17929,
                                0.653279,
                                0.28408,
                                0.0691776,
                                0,
                              };

static QPair<QGraphicsView *, QGraphicsItemAnimation *> SwingEffect(
        QWidget *parent, const QPixmap &icon, const QRect & rect, const qreal devicePixelRatio)
{
    QGraphicsView *swingEffectView = new QGraphicsView(parent);
    swingEffectView->viewport()->setAutoFillBackground(false);
    swingEffectView->setFrameShape(QFrame::NoFrame);

    QGraphicsScene *itemScene = new QGraphicsScene(parent);
    QGraphicsItemAnimation *itemAnimation = new QGraphicsItemAnimation(parent);

    swingEffectView->setAttribute(Qt::WA_TransparentForMouseEvents);
    swingEffectView->setScene(itemScene);
    swingEffectView->setAlignment(Qt::AlignCenter);
    swingEffectView->setFrameStyle(QFrame::NoFrame);
    swingEffectView->setContentsMargins(0, 0, 0, 0);
    swingEffectView->setRenderHints(QPainter::SmoothPixmapTransform);
    swingEffectView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    swingEffectView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    swingEffectView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QTimeLine *tl = itemAnimation->timeLine();
    if (!tl)
    {
        tl = new QTimeLine(1200, parent);
        tl->setFrameRange(0, 60);
        tl->setLoopCount(1);
        tl->setEasingCurve(QEasingCurve::Linear);
        tl->setStartFrame(0);

        itemAnimation->setTimeLine(tl);
    }

    QGraphicsPixmapItem *item = itemScene->addPixmap(icon);
    item->setTransformationMode(Qt::SmoothTransformation);
    item->setPos(QPointF(rect.center()) - QPointF(icon.rect().center()) / devicePixelRatio);

    itemAnimation->setItem(item);
    itemScene->setSceneRect(rect);
    swingEffectView->setSceneRect(rect);
    swingEffectView->setFixedSize(rect.size());

    const int px = qreal(-icon.rect().center().x()) / devicePixelRatio;
    const int py = qreal(-icon.rect().center().y()) / devicePixelRatio - 18.;
    const QPoint pos = rect.center() + QPoint(0, 18);
    for (int i(0); i != 60; ++i)
    {
        itemAnimation->setPosAt(i / 60.0, pos);
        itemAnimation->setTranslationAt(i / 60.0, px, py);
        itemAnimation->setRotationAt(i / 60.0, Frames[i]);
    }

    QObject::connect(tl, &QTimeLine::stateChanged, [=](QTimeLine::State newState) {
        if (newState == QTimeLine::NotRunning) {
            itemScene->deleteLater();
            swingEffectView->deleteLater();
            itemAnimation->deleteLater();
        }
    });

    return QPair<QGraphicsView *, QGraphicsItemAnimation *>(swingEffectView, itemAnimation);
}

#endif /* ifndef SWINGEFFECT */
