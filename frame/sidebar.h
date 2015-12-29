#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QFrame>
#include <QAbstractButton>
#include <QVBoxLayout>
#include <QTimer>

#include "modulemetadata.h"
#include "dtipsframe.h"
#include "pluginsmanager.h"
#include "sidebarview.h"

class ScrollHandle;
class SideBar : public QFrame
{
    Q_OBJECT
public:
    explicit SideBar(QWidget *parent = 0);

    void switchToModule(const ModuleMetaData &pluginId);

signals:
    void moduleSelected(ModuleMetaData meta);

protected:
    void resizeEvent(QResizeEvent *e);

private:
    void adjustScrollBox();

private:
    QVBoxLayout *m_sidebarLayout;
    SidebarView *m_sidebarView;
    ScrollHandle *m_topScrollHandle;
    ScrollHandle *m_botScrollHandle;
};

class ScrollHandle : public QLabel
{
    Q_OBJECT

public:
    explicit ScrollHandle(QWidget *parent = 0);

signals:
    void triggerd() const;

protected:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

private:
    QTimer *m_timer;
};

#endif // sidebar.h
