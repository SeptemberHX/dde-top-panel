//
// Created by septemberhx on 2020/5/26.
//

#ifndef DDE_TOP_PANEL_ACTIVEWINDOWCONTROLWIDGET_H
#define DDE_TOP_PANEL_ACTIVEWINDOWCONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

class ActiveWindowControlWidget : public QWidget {

    Q_OBJECT

public:
    explicit ActiveWindowControlWidget(QWidget *parent = 0);

public slots:
    void activeWindowInfoChanged();

private:
    QHBoxLayout *m_layout;
    QLabel *m_winTitleLabel;

    int currActiveWinId;
    QString currActiveWinTitle;
};


#endif //DDE_TOP_PANEL_ACTIVEWINDOWCONTROLWIDGET_H
