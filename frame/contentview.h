#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include <QFrame>
#include <QPluginLoader>

#include "modulemetadata.h"

class QHBoxLayout;
class SideBar;
class ContentView : public QFrame
{
    Q_OBJECT
public:
    explicit ContentView(QList<ModuleMetaData> modules, QWidget *parent = 0);

    void setModule(ModuleMetaData module);

signals:
    void homeSelected();

private:
    QHBoxLayout * m_layout;
    SideBar * m_sideBar;
    QPluginLoader * m_pluginLoader;

private slots:
    void onModuleSelected(ModuleMetaData meta);
};

#endif // CONTENTVIEW_H
