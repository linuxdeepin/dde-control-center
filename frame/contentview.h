#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include <QFrame>
#include <QPluginLoader>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include <libdui/dseparatorvertical.h>
#include <libdui/libdui_global.h>

#include "modulemetadata.h"
#include "pluginsmanager.h"
#include "interfaces.h"

DUI_USE_NAMESPACE

class QHBoxLayout;
class SideBar;
class ContentView : public QFrame
{
    Q_OBJECT
public:
    explicit ContentView(QWidget *parent = 0);
    ~ContentView();

    void switchToModule(ModuleMetaData module);
    void hide();
    void show();

public slots:
    void reLayout(bool hideInLeft);
    void switchToModule(const QString pluginId);
    void unloadPlugin();

signals:
    void backToHome();
    void shutdownSelected();

private:
    QHBoxLayout *m_layout;
    SideBar *m_sideBar;
    QPluginLoader *m_pluginLoader;

private slots:
    void onModuleSelected(ModuleMetaData meta);

private:
    QGraphicsOpacityEffect *m_opacityEffect;
    QPropertyAnimation *m_showAni;
    QPropertyAnimation *m_hideAni;
    DSeparatorVertical *m_leftSeparator;
    DSeparatorVertical *m_rightSeparator;
    PluginsManager *m_pluginsManager;
    QWidget *m_lastPluginWidget = nullptr;
    ModuleInterface *m_lastPluginInterface = nullptr;
    QWidget *m_lastPluginWidgetContainer;
    QHBoxLayout *m_lastPluginWidgetContainerLayout;

    bool m_hideInLeft = false;
};

#endif // CONTENTVIEW_H
