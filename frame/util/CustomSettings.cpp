//
// Created by septemberhx on 2020/6/5.
//

#include "CustomSettings.h"

CustomSettings::CustomSettings() {
    this->setDefaultPanelOpacity();
    this->setDefaultPanelBgColor();

    this->setDefaultActiveFont();
    this->setDefaultActiveFontColor();
    this->setDefaultActiveCloseIconPath();
    this->setDefaultActiveDefaultAppIconPath();
    this->setDefaultActiveMinimizedIconPath();
    this->setDefaultActiveUnmaximizedIconPath();
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
}

const QColor &CustomSettings::getPanelBgColor() const {
    return panelBgColor;
}

void CustomSettings::setPanelBgColor(const QColor &panelBgColor) {
    CustomSettings::panelBgColor = panelBgColor;
}

bool CustomSettings::isPanelEnablePluginsOnAllScreen() const {
    return panelEnablePluginsOnAllScreen;
}

void CustomSettings::setPanelEnablePluginsOnAllScreen(bool panelEnablePluginsOnAllScreen) {
    CustomSettings::panelEnablePluginsOnAllScreen = panelEnablePluginsOnAllScreen;
}

const QColor &CustomSettings::getActiveFontColor() const {
    return activeFontColor;
}

void CustomSettings::setActiveFontColor(const QColor &activeFontColor) {
    CustomSettings::activeFontColor = activeFontColor;
}

const QFont &CustomSettings::getActiveFont() const {
    return activeFont;
}

void CustomSettings::setActiveFont(const QFont &activeFont) {
    CustomSettings::activeFont = activeFont;
}

const QString &CustomSettings::getActiveCloseIconPath() const {
    return activeCloseIconPath;
}

void CustomSettings::setActiveCloseIconPath(const QString &activeCloseIconPath) {
    CustomSettings::activeCloseIconPath = activeCloseIconPath;
}

const QString &CustomSettings::getActiveUnmaximizedIconPath() const {
    return activeUnmaximizedIconPath;
}

void CustomSettings::setActiveUnmaximizedIconPath(const QString &activeUnmaximizedIconPath) {
    CustomSettings::activeUnmaximizedIconPath = activeUnmaximizedIconPath;
}

const QString &CustomSettings::getActiveMinimizedIconPath() const {
    return activeMinimizedIconPath;
}

void CustomSettings::setActiveMinimizedIconPath(const QString &activeMinimizedIconPath) {
    CustomSettings::activeMinimizedIconPath = activeMinimizedIconPath;
}

const QString &CustomSettings::getActiveDefaultAppIconPath() const {
    return activeDefaultAppIconPath;
}

void CustomSettings::setActiveDefaultAppIconPath(const QString &activeDefaultAppIconPath) {
    CustomSettings::activeDefaultAppIconPath = activeDefaultAppIconPath;
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


