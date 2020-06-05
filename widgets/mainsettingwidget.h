#ifndef MAINSETTINGWIDGET_H
#define MAINSETTINGWIDGET_H

#include <QWidget>

namespace Ui {
class MainSettingWidget;
}

class MainSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainSettingWidget(QWidget *parent = nullptr);
    ~MainSettingWidget();

private:
    Ui::MainSettingWidget *ui;
    QMovie *movie;

    void opacityValueChanged(int value);
    void panelColorButtonClicked();
    void fontColorButtonClicked();
    void closeButtonClicked();
    void closeResetButtonClicked();

    void unmaxButtonClicked();
    void unmaxResetButtonClicked();

    void minButtonClicked();
    void minResetButtonClicked();

    void defaultButtonClicked();
    void defaultResetButtonClicked();
};

#endif // MAINSETTINGWIDGET_H
