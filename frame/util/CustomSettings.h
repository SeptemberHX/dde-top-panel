//
// Created by septemberhx on 2020/6/5.
//

#ifndef DDE_TOP_PANEL_CUSTOMSETTINGS_H
#define DDE_TOP_PANEL_CUSTOMSETTINGS_H

#include <QObject>
#include <QColor>
#include <QFont>

class CustomSettings : public QObject {

    Q_OBJECT

public:
    static CustomSettings* instance();

    qreal getPanelOpacity() const;

    void setPanelOpacity(qreal panelOpacity);

    const QColor &getPanelBgColor() const;

    void setPanelBgColor(const QColor &panelBgColor);

    bool isPanelEnablePluginsOnAllScreen() const;

    void setPanelEnablePluginsOnAllScreen(bool panelEnablePluginsOnAllScreen);

    const QColor &getActiveFontColor() const;

    void setActiveFontColor(const QColor &activeFontColor);

    const QFont &getActiveFont() const;

    void setActiveFont(const QFont &activeFont);

    const QString &getActiveCloseIconPath() const;

    void setActiveCloseIconPath(const QString &activeCloseIconPath);

    const QString &getActiveUnmaximizedIconPath() const;

    void setActiveUnmaximizedIconPath(const QString &activeUnmaximizedIconPath);

    const QString &getActiveMinimizedIconPath() const;

    void setActiveMinimizedIconPath(const QString &activeMinimizedIconPath);

    const QString &getActiveDefaultAppIconPath() const;

    void setActiveDefaultAppIconPath(const QString &activeDefaultAppIconPath);

    bool isShowControlButtons() const;

    void setShowControlButtons(bool showControlButtons);

    bool isShowAppNameInsteadIcon() const;

    void setShowAppNameInsteadIcon(bool showAppNameInsteadIcon);

    void saveSettings();
    void readSettings();

    void resetCloseIconPath();
    void resetUnmaxIconPath();
    void resetMinIconPath();
    void resetDefaultIconPath();

    void setDefaultPanelOpacity();
    void setDefaultPanelBgColor();
    void setDefaultActiveFontColor();
    void setDefaultActiveFont();
    void setDefaultActiveCloseIconPath();
    void setDefaultActiveUnmaximizedIconPath();

    bool isShowGlobalMenuOnHover() const;

    void setShowGlobalMenuOnHover(bool showGlobalMenuOnHover);

    void setDefaultActiveMinimizedIconPath();
    void setDefaultActiveDefaultAppIconPath();
    void setDefaultShowGlobalMenuOnHover();

    int getPanelHeight() const;

    bool isButtonOnLeft() const;

    void setButtonOnLeft(bool buttonOnLeft);

    bool isButtonHighlight() const;

    void setButtonHighlight(bool buttonHighlight);

    const QColor &getButtonHighLightColor() const;

    void setButtonHighLightColor(const QColor &buttonHighLightColor);

signals:
    void settingsChanged();

private:
    CustomSettings();
    QString getConfigPath();    

private:
    int panelHeight;

    // panel
    quint8 panelOpacity;
    QColor panelBgColor;
    bool panelEnablePluginsOnAllScreen;
    bool showGlobalMenuOnHover;

    bool showControlButtons;
    bool showAppNameInsteadIcon;
    bool showLogoWithAppName;

    bool ignoreDock;
    bool buttonOnLeft;
    bool buttonHighlight;
    QColor buttonHighLightColor;

    bool hideTitleWhenMax;

    bool followSystemTheme;

    bool allowDragWindowWhenMax;
public:
    bool isAllowDragWindowWhenMax() const;

    void setAllowDragWindowWhenMax(bool allowDragWindowWhenMax);

public:
    bool isFollowSystemTheme() const;

    void setFollowSystemTheme(bool followSystemTheme);

public:
    bool isHideTitleWhenMax() const;

    void setHideTitleWhenMax(bool hideTitleWhenMax);

public:
    bool isEnableGlobalMenu() const;

    void setEnableGlobalMenu(bool enableGlobalMenu);

    bool isEnableAutoStart() const;

    void setEnableAutoStart(bool enableAutoStart);

public:
    bool isIgnoreDock() const;

    void setIgnoreDock(bool ignoreDock);

public:
    bool isShowLogoWithAppName() const;

    void setShowLogoWithAppName(bool showLogoWithAppName);

private:

    // active window control
    QColor activeFontColor;
    QFont activeFont;
    QString activeCloseIconPath;
    QString activeUnmaximizedIconPath;
    QString activeMinimizedIconPath;
    QString activeDefaultAppIconPath;

    QString defaultIconPathLight;
    QString defaultIconPathDark;

    QColor defaultLightColor;
    QColor defaultDarkColor;
};


#endif //DDE_TOP_PANEL_CUSTOMSETTINGS_H
