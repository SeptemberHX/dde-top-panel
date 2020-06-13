#include "mainsettingwidget.h"
#include "ui_mainsettingwidget.h"
#include "../frame/util/CustomSettings.h"
#include <QIcon>
#include <QColorDialog>
#include <QFileDialog>
#include <QMovie>
#include <QApplication>

MainSettingWidget::MainSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSettingWidget)
{
    ui->setupUi(this);
    ui->panelColorlabel->setStyleSheet(QString("QLabel {background-color: %1;}").arg(CustomSettings::instance()->getPanelBgColor().name()));
    ui->fontColorLabel->setStyleSheet(QString("QLabel {background-color: %1;}").arg(CustomSettings::instance()->getActiveFontColor().name()));


    ui->opacitySpinBox->setValue(CustomSettings::instance()->getPanelOpacity());

    ui->closeButtonLabel->setPixmap(QIcon(CustomSettings::instance()->getActiveCloseIconPath()).pixmap(ui->closeButtonLabel->size()));
    ui->unmaxButtonLabel->setPixmap(QIcon(CustomSettings::instance()->getActiveUnmaximizedIconPath()).pixmap(ui->unmaxButtonLabel->size()));
    ui->minButtonLabel->setPixmap(QIcon(CustomSettings::instance()->getActiveMinimizedIconPath()).pixmap(ui->minButtonLabel->size()));
    ui->defaultIconLabel->setPixmap(QIcon(CustomSettings::instance()->getActiveDefaultAppIconPath()).pixmap(ui->defaultIconLabel->size()));
    ui->menuOnHoverCheckBox->setChecked(CustomSettings::instance()->isShowGlobalMenuOnHover());

    ui->appNameLabel->setText(QApplication::applicationDisplayName());
    ui->appVersionLabel->setText(QApplication::applicationVersion());

    ui->panelColorToolButton->setIcon(QIcon(":/icons/config.svg"));
    ui->fontColorToolButton->setIcon(QIcon(":/icons/config.svg"));
    ui->defaultIconResetToolButton->setIcon(QIcon(":/icons/reset.svg"));
    ui->defaultIconToolButton->setIcon(QIcon(":/icons/config.svg"));
    ui->minToolButton->setIcon(QIcon(":/icons/config.svg"));
    ui->minResetToolButton->setIcon(QIcon(":/icons/reset.svg"));
    ui->unmaxButtonToolButton->setIcon(QIcon(":/icons/config.svg"));
    ui->unmaxResetToolButton->setIcon(QIcon(":/icons/reset.svg"));
    ui->closeButtonToolButton->setIcon(QIcon(":/icons/config.svg"));
    ui->closeResetToolButton->setIcon(QIcon(":/icons/reset.svg"));

    movie = new QMovie(":/icons/doge.gif");
    ui->pMovieLabel->setMovie(movie);

    connect(ui->opacitySpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &MainSettingWidget::opacityValueChanged);
    connect(ui->panelColorToolButton, &QToolButton::clicked, this, &MainSettingWidget::panelColorButtonClicked);
    connect(ui->fontColorToolButton, &QToolButton::clicked, this, &MainSettingWidget::fontColorButtonClicked);
    connect(ui->closeButtonToolButton, &QToolButton::clicked, this, &MainSettingWidget::closeButtonClicked);
    connect(ui->closeResetToolButton, &QToolButton::clicked, this, &MainSettingWidget::closeResetButtonClicked);
    connect(ui->unmaxButtonToolButton, &QToolButton::clicked, this, &MainSettingWidget::unmaxButtonClicked);
    connect(ui->unmaxResetToolButton, &QToolButton::clicked, this, &MainSettingWidget::unmaxResetButtonClicked);
    connect(ui->minToolButton, &QToolButton::clicked, this, &MainSettingWidget::minButtonClicked);
    connect(ui->minResetToolButton, &QToolButton::clicked, this, &MainSettingWidget::minResetButtonClicked);
    connect(ui->defaultIconToolButton, &QToolButton::clicked, this, &MainSettingWidget::defaultButtonClicked);
    connect(ui->defaultIconResetToolButton, &QToolButton::clicked, this, &MainSettingWidget::defaultResetButtonClicked);
    connect(ui->menuOnHoverCheckBox, &QCheckBox::stateChanged, this, [this]() {
        CustomSettings::instance()->setShowGlobalMenuOnHover(ui->menuOnHoverCheckBox->isChecked());
    });
}

MainSettingWidget::~MainSettingWidget()
{
    delete ui;
}

void MainSettingWidget::opacityValueChanged(int value) {
    CustomSettings::instance()->setPanelOpacity(value);
}

void MainSettingWidget::panelColorButtonClicked() {
    QColor newPanelBgColor = QColorDialog::getColor(CustomSettings::instance()->getPanelBgColor());
    ui->panelColorlabel->setStyleSheet(QString("QLabel {background-color: %1;}").arg(newPanelBgColor.name()));
    CustomSettings::instance()->setPanelBgColor(newPanelBgColor);
}

void MainSettingWidget::fontColorButtonClicked() {
    QColor newFontColor = QColorDialog::getColor(CustomSettings::instance()->getActiveFontColor());
    ui->fontColorLabel->setStyleSheet(QString("QLabel {background-color: %1;}").arg(newFontColor.name()));
    CustomSettings::instance()->setActiveFontColor(newFontColor);

}

void MainSettingWidget::closeButtonClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select your close button icon"), "~", tr("Images (*.png *.jpg *.svg)"));
    if (!fileName.isNull()) {
        ui->closeButtonLabel->setPixmap(QIcon(fileName).pixmap(ui->closeButtonLabel->size()));
        CustomSettings::instance()->setActiveCloseIconPath(fileName);
    }
}

void MainSettingWidget::closeResetButtonClicked() {
    CustomSettings::instance()->resetCloseIconPath();
    ui->closeButtonLabel->setPixmap(QIcon(CustomSettings::instance()->getActiveCloseIconPath()).pixmap(ui->closeButtonLabel->size()));
}

void MainSettingWidget::unmaxButtonClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select your unmaximized button icon"), "~", tr("Images (*.png *.jpg *.svg)"));
    if (!fileName.isNull()) {
        ui->unmaxButtonLabel->setPixmap(QIcon(fileName).pixmap(ui->unmaxButtonLabel->size()));
        CustomSettings::instance()->setActiveUnmaximizedIconPath(fileName);
    }
}

void MainSettingWidget::unmaxResetButtonClicked() {
    CustomSettings::instance()->resetUnmaxIconPath();
    ui->unmaxButtonLabel->setPixmap(QIcon(CustomSettings::instance()->getActiveUnmaximizedIconPath()).pixmap(ui->unmaxButtonLabel->size()));
}

void MainSettingWidget::minButtonClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select your minimized button icon"), "~", tr("Images (*.png *.jpg *.svg)"));
    if (!fileName.isNull()) {
        ui->minButtonLabel->setPixmap(QIcon(fileName).pixmap(ui->minButtonLabel->size()));
        CustomSettings::instance()->setActiveMinimizedIconPath(fileName);
    }
}

void MainSettingWidget::minResetButtonClicked() {
    CustomSettings::instance()->resetMinIconPath();
    ui->minButtonLabel->setPixmap(QIcon(CustomSettings::instance()->getActiveMinimizedIconPath()).pixmap(ui->minButtonLabel->size()));
}

void MainSettingWidget::defaultButtonClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select your default icon"), "~", tr("Images (*.png *.jpg *.svg)"));
    if (!fileName.isNull()) {
        ui->defaultIconLabel->setPixmap(QIcon(fileName).pixmap(ui->defaultIconLabel->size()));
        CustomSettings::instance()->setActiveDefaultAppIconPath(fileName);
    }
}

void MainSettingWidget::defaultResetButtonClicked() {
    CustomSettings::instance()->resetDefaultIconPath();
    ui->defaultIconLabel->setPixmap(QIcon(CustomSettings::instance()->getActiveDefaultAppIconPath()).pixmap(ui->defaultIconLabel->size()));
}

void MainSettingWidget::showEvent(QShowEvent *event) {
    movie->start();
    QWidget::showEvent(event);
}

void MainSettingWidget::hideEvent(QHideEvent *event) {
    movie->stop();
    QWidget::hideEvent(event);
}

void MainSettingWidget::closeEvent(QCloseEvent *event) {
    movie->stop();
    QWidget::closeEvent(event);
}
