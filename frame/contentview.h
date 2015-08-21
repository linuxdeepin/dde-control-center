#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include <QFrame>
#include <QPluginLoader>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include "modulemetadata.h"

class QHBoxLayout;
class SideBar;
class ContentView : public QFrame
{
    Q_OBJECT
public:
    explicit ContentView(QList<ModuleMetaData> modules, QWidget *parent = 0);
    ~ContentView();

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
    QPropertyAnimation *m_showAni;
    QPropertyAnimation *m_hideAni;
};

#endif // CONTENTVIEW_H
