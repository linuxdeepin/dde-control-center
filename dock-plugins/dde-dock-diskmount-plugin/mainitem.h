#ifndef MAINITEM_H
#define MAINITEM_H

#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include <dde-dock/dockconstants.h>

class MainItem : public QLabel
{
    Q_OBJECT
public:
    MainItem(QWidget *parent = 0);
    void onDockModeChanged(Dock::DockMode mode);

private:
    void updateIcon();

private:
    Dock::DockMode m_dockMode = Dock::FashionMode;
};

#endif // MAINITEM_H
