//
// Created by septemberhx on 2020/6/2.
//

#ifndef DDE_TOP_PANEL_QCLICKABLELABEL_H
#define DDE_TOP_PANEL_QCLICKABLELABEL_H


#include <QLabel>
#include "../frame/util/CustomSettings.h"

class QClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit QClickableLabel(QWidget *parent);

    void setDefaultFontColor(const QColor &defaultFontColor);
    int standardWidth();

    void resetClicked();

    bool isSelected() const;
    void setClicked(bool flag);

signals:
    void clicked();

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;

private:
    void setSelectedColor();
    void setNormalColor();

    QColor defaultFontColor = CustomSettings::instance()->getActiveFontColor();
    bool isClicked;

    QFontMetrics *metrics;
};


#endif //DDE_TOP_PANEL_QCLICKABLELABEL_H
