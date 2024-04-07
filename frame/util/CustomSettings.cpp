//
// Created by septemberhx on 2020/6/5.
//

#include "CustomSettings.h"
#include <QSettings>
#include <QDir>
#include <DGuiApplicationHelper>
#include <DApplicationHelper>
#include <DSysInfo>

DGUI_USE_NAMESPACE

CustomSettings::CustomSettings() {
    this->defaultIconPathLight = ":/icons/linux.svg";
    this->defaultIconPathDark = ":/icons/linux-light.svg";

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
    this->showLogoWithAppName = true;
    this->ignoreDock = false;
    this->buttonOnLeft = true;
    this->panelHeight = 24;

    this->buttonHighlight = true;
    this->buttonHighLightColor = QColor("#9d2933");
    this->followSystemTheme = true;

    this->defaultDarkColor = Qt::black;
    this->defaultLightColor = Qt::white;

    this->readSettings();
    connect(this, &CustomSettings::settingsChanged, this, &CustomSettings::saveSettings);
}

CustomSettings *CustomSettings::instance() {
    static CustomSettings customSettings;
    return &customSettings;
}

qreal CustomSettings::getPanelOpacity() const {
    if (this->isFollowSystemTheme()) {
        return 80;
    } else {
        return panelOpacity;
    }
}

void CustomSettings::setPanelOpacity(qreal panelOpacity) {
    CustomSettings::panelOpacity = panelOpacity;
    emit settingsChanged();
}

const QColor &CustomSettings::getPanelBgColor() const {
    if (this->isFollowSystemTheme()) {
        switch (DGuiApplicationHelper::instance()->themeType()) {
            case Dtk::Gui::DGuiApplicationHelper::DarkType:
                return this->defaultDarkColor;
            case Dtk::Gui::DGuiApplicationHelper::LightType:
                return this->defaultLightColor;
        }
    }
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
    if (this->isFollowSystemTheme()) {
        switch (DGuiApplicationHelper::instance()->themeType()) {
            case Dtk::Gui::DGuiApplicationHelper::DarkType:
                return this->defaultLightColor;
            case Dtk::Gui::DGuiApplicationHelper::LightType:
                return this->defaultDarkColor;
        }
    }
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
    if (this->isFollowSystemTheme() && activeDefaultAppIconPath == ":/icons/linux.svg") {
        switch (DGuiApplicationHelper::instance()->themeType()) {
            case Dtk::Gui::DGuiApplicationHelper::DarkType: 
                return this->defaultIconPathDark;
            case Dtk::Gui::DGuiApplicationHelper::LightType:
                return this->defaultIconPathLight;
        }
    }
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
    settings.setValue("panel/followSystemTheme", this->isFollowSystemTheme());
    settings.setValue("windowControl/fontColor", this->getActiveFontColor());
    settings.setValue("windowControl/closeIcon", this->getActiveCloseIconPath());
    settings.setValue("windowControl/unmaxIcon", this->getActiveUnmaximizedIconPath());
    settings.setValue("windowControl/minIcon", this->getActiveMinimizedIconPath());
    settings.setValue("windowControl/defaultIcon", this->getActiveDefaultAppIconPath());
    settings.setValue("windowControl/showMenuOnHover", this->isShowGlobalMenuOnHover());
    settings.setValue("windowControl/showControlButtons", this->isShowControlButtons());
    settings.setValue("windowControl/showAppNameInsteadIcon", this->isShowAppNameInsteadIcon());
    settings.setValue("windowControl/showLogoWithAppName", this->isShowLogoWithAppName());
    settings.setValue("windowControl/ignoreDock", this->isIgnoreDock());
    settings.setValue("windowControl/buttonOnRight", !this->isButtonOnLeft());
    settings.setValue("windowControl/enableButtonHighlight", this->isButtonHighlight());
    settings.setValue("windowControl/buttonHighlightColor", this->buttonHighLightColor);
    settings.setValue("windowControl/allowDragWindowWhenMax", this->allowDragWindowWhenMax);

    QSettings kwinrc(getConfigPath(), QSettings::IniFormat);
    kwinrc.setValue("Windows/BorderlessMaximizedWindows", this->hideTitleWhenMax);
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
    this->showLogoWithAppName = settings.value("windowControl/showLogoWithAppName", this->showLogoWithAppName).toBool();
    this->ignoreDock = settings.value("windowControl/ignoreDock", this->isIgnoreDock()).toBool();
    this->buttonOnLeft = !settings.value("windowControl/buttonOnRight", !this->isButtonOnLeft()).toBool();

    this->buttonHighlight = settings.value("windowControl/enableButtonHighlight", this->isButtonHighlight()).toBool();
    this->buttonHighLightColor = settings.value("windowControl/buttonHighlightColor", this->buttonHighLightColor).value<QColor>();
    this->followSystemTheme = settings.value("panel/followSystemTheme", this->isFollowSystemTheme()).toBool();
    this->allowDragWindowWhenMax = settings.value("windowControl/allowDragWindowWhenMax", this->allowDragWindowWhenMax).toBool();

    QSettings kwinrc(getConfigPath(), QSettings::IniFormat);
    this->hideTitleWhenMax = kwinrc.value("Windows/BorderlessMaximizedWindows", false).toBool();
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

bool CustomSettings::isShowLogoWithAppName() const {
    return showLogoWithAppName;
}

void CustomSettings::setShowLogoWithAppName(bool showLogoWithAppName) {
    CustomSettings::showLogoWithAppName = showLogoWithAppName;
    emit settingsChanged();
}

bool CustomSettings::isIgnoreDock() const {
    return ignoreDock;
}

void CustomSettings::setIgnoreDock(bool ignoreDock) {
    CustomSettings::ignoreDock = ignoreDock;
    emit settingsChanged();
}

int CustomSettings::getPanelHeight() const {
    return panelHeight;
}

bool CustomSettings::isButtonOnLeft() const {
    return buttonOnLeft;
}

void CustomSettings::setButtonOnLeft(bool buttonOnLeft) {
    CustomSettings::buttonOnLeft = buttonOnLeft;
    emit settingsChanged();
}

bool CustomSettings::isButtonHighlight() const {
    return buttonHighlight;
}

void CustomSettings::setButtonHighlight(bool buttonHighlight) {
    CustomSettings::buttonHighlight = buttonHighlight;
    emit settingsChanged();
}

const QColor &CustomSettings::getButtonHighLightColor() const {
    return buttonHighLightColor;
}

void CustomSettings::setButtonHighLightColor(const QColor &buttonHighLightColor) {
    CustomSettings::buttonHighLightColor = buttonHighLightColor;
    emit settingsChanged();
}

bool CustomSettings::isHideTitleWhenMax() const {
    return hideTitleWhenMax;
}

void CustomSettings::setHideTitleWhenMax(bool hideTitleWhenMax) {
    CustomSettings::hideTitleWhenMax = hideTitleWhenMax;
    emit settingsChanged();
}

bool CustomSettings::isFollowSystemTheme() const {
    return followSystemTheme;
}

void CustomSettings::setFollowSystemTheme(bool followSystemTheme) {
    CustomSettings::followSystemTheme = followSystemTheme;
    emit settingsChanged();
}

bool CustomSettings::isAllowDragWindowWhenMax() const {
    return allowDragWindowWhenMax;
}

void CustomSettings::setAllowDragWindowWhenMax(bool allowDragWindowWhenMax) {
    CustomSettings::allowDragWindowWhenMax = allowDragWindowWhenMax;
    emit settingsChanged();
}

QString CustomSettings::getConfigPath() {
    QString version = Dtk::Core::DSysInfo::deepinVersion();
    QString configPath = QDir::homePath();
    if (version == "20") {
        configPath += "/.config/kwinrc";
    } else if(version == "23"){
        configPath += "/.config/deepin-kwinrc";
    }
    return configPath;
}
