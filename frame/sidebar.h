#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QFrame>

#include "modulemetadata.h"

class SideBar : public QFrame
{
    Q_OBJECT
public:
    explicit SideBar(QList<ModuleMetaData> modules, QWidget *parent = 0);

signals:
    void moduleSelected(ModuleMetaData meta);

private slots:
    void onSideBarButtonClicked();
};


class QLabel;
class QEvent;
class QMouseEvent;
class SideBarButton : public QFrame
{
    Q_OBJECT
public:
    SideBarButton(ModuleMetaData metaData, QWidget * parent = 0);

    void enterEvent(QEvent * event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent * event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

    ModuleMetaData metaData();

signals:
    void clicked();

private:
    enum State { Normal, Hover, Selected };

    ModuleMetaData m_meta;

    QLabel * m_icon;

    void setState(State state);
};

#endif // SIDEBAR_H
