#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QFrame>
#include <QAbstractButton>
#include <QVBoxLayout>

#include "modulemetadata.h"
#include "dtipsframe.h"
#include "pluginsmanager.h"
#include "sidebarview.h"

class SideBar : public QFrame
{
    Q_OBJECT
public:
    explicit SideBar(QWidget *parent = 0);

    void switchToModule(const ModuleMetaData &pluginId);
    DTipsFrame *getTipFrame() const;

signals:
    void moduleSelected(ModuleMetaData meta);

private:
    QVBoxLayout *m_sidebarLayout;
    SidebarView *m_sidebarView;
};

#endif // sidebar.h
