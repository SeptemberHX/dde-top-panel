//
// Created by septemberhx on 2020/6/5.
//

#include "CustomSettings.h"
#include <QSettings>

CustomSettings::CustomSettings() {
    this->setDefaultPanelOpacity();
    this->setDefaultPanelBgColor();

    this->setDefaultActiveFont();
    this->setDefaultActiveFontColor();
    this->setDefaultActiveCloseIconPath();
    this->setDefaultActiveDefaultAppIconPath();
    this->setDefaultActiveMinimizedIconPath();
    this->setDefaultActiveUnmaximizedIconPath();
    this->setDefaultShowGlobalMenuOnHover();

    this->showAppNameInsteadIcon = true;
    this->showControlButtons = true;

    this->readSettings();
    connect(this, &CustomSettings::settingsChanged, this, &CustomSettings::saveSettings);
}

CustomSettings *CustomSettings::instance() {
    static CustomSettings customSettings;
    return &customSettings;
}

qreal CustomSettings::getPanelOpacity() const {
    return panelOpacity;
}

void CustomSettings::setPanelOpacity(qreal panelOpacity) {
    CustomSettings::panelOpacity = panelOpacity;
    emit settingsChanged();
}

const QColor &CustomSettings::getPanelBgColor() const {
    return panelBgColor;
}

void CustomSettings::setPanelBgColor(const QColor &panelBgColor) {
    CustomSettings::panelBgColor = panelBgColor;
    emit settingsChanged();
}

bool CustomSettings::isPanelEnablePluginsOnAllScreen() const {
    return panelEnablePluginsOnAllScreen;
}

void CustomSettings::setPanelEnablePluginsOnAllScreen(bool panelEnablePluginsOnAllScreen) {
    CustomSettings::panelEnablePluginsOnAllScreen = panelEnablePluginsOnAllScreen;
    emit settingsChanged();
}

const QColor &CustomSettings::getActiveFontColor() const {
    return activeFontColor;
}

void CustomSettings::setActiveFontColor(const QColor &activeFontColor) {
    CustomSettings::activeFontColor = activeFontColor;
    emit settingsChanged();
}

const QFont &CustomSettings::getActiveFont() const {
    return activeFont;
}

void CustomSettings::setActiveFont(const QFont &activeFont) {
    CustomSettings::activeFont = activeFont;
    emit settingsChanged();
}

const QString &CustomSettings::getActiveCloseIconPath() const {
    return activeCloseIconPath;
}

void CustomSettings::setActiveCloseIconPath(const QString &activeCloseIconPath) {
    CustomSettings::activeCloseIconPath = activeCloseIconPath;
    emit settingsChanged();
}

const QString &CustomSettings::getActiveUnmaximizedIconPath() const {
    return activeUnmaximizedIconPath;
}

void CustomSettings::setActiveUnmaximizedIconPath(const QString &activeUnmaximizedIconPath) {
    CustomSettings::activeUnmaximizedIconPath = activeUnmaximizedIconPath;
    emit settingsChanged();
}

const QString &CustomSettings::getActiveMinimizedIconPath() const {
    return activeMinimizedIconPath;
}

void CustomSettings::setActiveMinimizedIconPath(const QString &activeMinimizedIconPath) {
    CustomSettings::activeMinimizedIconPath = activeMinimizedIconPath;
    emit settingsChanged();
}

const QString &CustomSettings::getActiveDefaultAppIconPath() const {
    return activeDefaultAppIconPath;
}

void CustomSettings::setActiveDefaultAppIconPath(const QString &activeDefaultAppIconPath) {
    CustomSettings::activeDefaultAppIconPath = activeDefaultAppIconPath;
    emit settingsChanged();
}

void CustomSettings::setDefaultActiveCloseIconPath() {
    this->activeCloseIconPath = ":/icons/close.svg";
}

void CustomSettings::setDefaultActiveUnmaximizedIconPath() {
    this->activeUnmaximizedIconPath = ":/icons/maximum.svg";
}

void CustomSettings::setDefaultActiveMinimizedIconPath() {
    this->activeMinimizedIconPath = ":/icons/minimum.svg";
}

void CustomSettings::setDefaultActiveDefaultAppIconPath() {
    this->activeDefaultAppIconPath = ":/icons/linux.svg";
}

void CustomSettings::setDefaultActiveFont() {
    this->activeFont = QFont();
}

void CustomSettings::setDefaultActiveFontColor() {
    this->activeFontColor = Qt::black;
}

void CustomSettings::setDefaultPanelBgColor() {
    this->panelBgColor = Qt::white;
}

void CustomSettings::setDefaultPanelOpacity() {
    this->panelOpacity = 50;
}

void CustomSettings::resetCloseIconPath() {
    this->setDefaultActiveCloseIconPath();
    emit settingsChanged();
}

void CustomSettings::resetUnmaxIconPath() {
    this->setDefaultActiveUnmaximizedIconPath();
    emit settingsChanged();
}

void CustomSettings::resetMinIconPath() {
    this->setDefaultActiveMinimizedIconPath();
    emit settingsChanged();
}

void CustomSettings::resetDefaultIconPath() {
    this->setDefaultActiveDefaultAppIconPath();
    emit settingsChanged();
}

void CustomSettings::setDefaultShowGlobalMenuOnHover() {
    this->showGlobalMenuOnHover = false;
}

bool CustomSettings::isShowGlobalMenuOnHover() const {
    return showGlobalMenuOnHover;
}

void CustomSettings::setShowGlobalMenuOnHover(bool showGlobalMenuOnHover) {
    CustomSettings::showGlobalMenuOnHover = showGlobalMenuOnHover;
    emit settingsChanged();
}

void CustomSettings::saveSettings() {
    QSettings settings("dde-top-panel", "top-panel");

    settings.setValue("panel/bgColor", this->getPanelBgColor());
    settings.setValue("panel/opacity", this->getPanelOpacity());
    settings.setValue("windowControl/fontColor", this->getActiveFontColor());
    settings.setValue("windowControl/closeIcon", this->getActiveCloseIconPath());
    settings.setValue("windowControl/unmaxIcon", this->getActiveUnmaximizedIconPath());
    settings.setValue("windowControl/minIcon", this->getActiveMinimizedIconPath());
    settings.setValue("windowControl/defaultIcon", this->getActiveDefaultAppIconPath());
    settings.setValue("windowControl/showMenuOnHover", this->isShowGlobalMenuOnHover());
    settings.setValue("windowControl/showControlButtons", this->isShowControlButtons());
    settings.setValue("windowControl/showAppNameInsteadIcon", this->isShowAppNameInsteadIcon());
}

void CustomSettings::readSettings() {
    QSettings settings("dde-top-panel", "top-panel");
    this->panelBgColor = settings.value("panel/bgColor", this->panelBgColor).value<QColor>();
    this->panelOpacity = settings.value("panel/opacity", this->panelOpacity).toUInt();
    this->activeFontColor = settings.value("windowControl/fontColor", this->activeFontColor).value<QColor>();
    this->activeCloseIconPath = settings.value("windowControl/closeIcon", this->activeCloseIconPath).toString();
    this->activeUnmaximizedIconPath = settings.value("windowControl/unmaxIcon", this->activeUnmaximizedIconPath).toString();
    this->activeMinimizedIconPath = settings.value("windowControl/minIcon", this->activeMinimizedIconPath).toString();
    this->activeDefaultAppIconPath = settings.value("windowControl/defaultIcon", this->activeDefaultAppIconPath).toString();
    this->showGlobalMenuOnHover = settings.value("windowControl/showMenuOnHover", this->showGlobalMenuOnHover).toBool();
    this->showControlButtons = settings.value("windowControl/showControlButtons", this->showControlButtons).toBool();
    this->showAppNameInsteadIcon = settings.value("windowControl/showAppNameInsteadIcon", this->showAppNameInsteadIcon).toBool();
}

bool CustomSettings::isShowControlButtons() const {
    return showControlButtons;
}

void CustomSettings::setShowControlButtons(bool showControlButtons) {
    CustomSettings::showControlButtons = showControlButtons;
    emit settingsChanged();
}

bool CustomSettings::isShowAppNameInsteadIcon() const {
    return showAppNameInsteadIcon;
}

void CustomSettings::setShowAppNameInsteadIcon(bool showAppNameInsteadIcon) {
    CustomSettings::showAppNameInsteadIcon = showAppNameInsteadIcon;
    emit settingsChanged();
}
