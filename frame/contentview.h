#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include <QFrame>
#include <QPluginLoader>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include <libdui/dseparatorvertical.h>
#include <libdui/libdui_global.h>

#include "modulemetadata.h"

DUI_USE_NAMESPACE

class QHBoxLayout;
class SideBar;
class ContentView : public QFrame
{
    Q_OBJECT
public:
    explicit ContentView(QList<ModuleMetaData> modules, bool hideInLeft = false, QWidget *parent = 0);
    ~ContentView();

    void setModule(ModuleMetaData module);
    void hide();
    void show();

public slots:
    inline void unloadOldPlugin() {m_pluginLoader->unload();}
    void reLayout(bool hideInLeft);

signals:
    void homeSelected();
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
    bool m_hideInLeft;
};

#endif // CONTENTVIEW_H
