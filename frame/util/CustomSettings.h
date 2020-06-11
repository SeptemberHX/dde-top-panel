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

signals:
    void settingsChanged();

private:
    CustomSettings();

private:
    // panel
    quint8 panelOpacity;
    QColor panelBgColor;
    bool panelEnablePluginsOnAllScreen;
    bool showGlobalMenuOnHover;

    // active window control
    QColor activeFontColor;
    QFont activeFont;
    QString activeCloseIconPath;
    QString activeUnmaximizedIconPath;
    QString activeMinimizedIconPath;
    QString activeDefaultAppIconPath;
};


#endif //DDE_TOP_PANEL_CUSTOMSETTINGS_H
