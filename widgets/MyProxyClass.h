//
// Created by ragdoll on 2022/3/15.
//

#ifndef DDE_TOP_PANEL_MYPROXYCLASS_H
#define DDE_TOP_PANEL_MYPROXYCLASS_H


#include <QProxyStyle>
#include <iostream>


class MyProxyStyle : public QProxyStyle {
public:
    void setPixelRatio(qreal pixelRatio) {
        MyProxyStyle::pixelRatio = pixelRatio;
    }

    void drawItemPixmap(QPainter *painter, const QRect &rect, int alignment, const QPixmap &pixmap) const override {
        std::cout << "-----------------" << std::endl;
        QProxyStyle::drawItemPixmap(painter, rect, alignment, pixmap);
    }



    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                       const QWidget *widget) const override {
        std::cout << "=================" << std::endl;
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }

    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override {
        if (metric == QStyle::PM_ButtonIconSize) {
            return QProxyStyle::pixelMetric(metric, option, widget) * pixelRatio;
        }
        return QProxyStyle::pixelMetric(metric, option, widget);
    }

private:
    qreal pixelRatio;
};

#endif //DDE_TOP_PANEL_MYPROXYCLASS_H
