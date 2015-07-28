#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include <QFrame>
#include <QPluginLoader>
#include <QGraphicsOpacityEffect>

#include "modulemetadata.h"

class QHBoxLayout;
class SideBar;
class ContentView : public QFrame
{
    Q_OBJECT
public:
    explicit ContentView(QList<ModuleMetaData> modules, QWidget *parent = 0);

    void setModule(ModuleMetaData module);
    void hide();
    void show();

signals:
    void homeSelected();

private:
    QHBoxLayout * m_layout;
    SideBar * m_sideBar;
    QPluginLoader * m_pluginLoader;

private slots:
    void onModuleSelected(ModuleMetaData meta);

private:
    QGraphicsOpacityEffect *m_opacityEffect;

    const int m_animationDuration = 300;
};

#endif // CONTENTVIEW_H
