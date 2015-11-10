#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include <QList>
#include <QDBusAbstractAdaptor>
#include <QPropertyAnimation>

#include "interfaces.h"
#include "modulemetadata.h"
#include "dbus/dbuscontrolcenter.h"
#include "dbus/dbusxmousearea.h"

class QStackedLayout;
class QKeyEvent;
class HomeScreen;
class ContentView;
class DBusControlCenterService;
class Frame: public QFrame
{
    Q_OBJECT

    Q_PROPERTY(bool hideInLeft READ isHideInLeft WRITE setHideInLeft NOTIFY hideInLeftChanged)
    Q_PROPERTY(bool canNotHide READ canNotHide WRITE setCanNotHide NOTIFY canNotHideChanged)

public:
    Frame(QWidget *parent = 0);
    ~Frame();

    void show(bool imme = false);
    void hide(bool imme = false);
    inline bool isHideInLeft() const {return m_hideInLeft;}
    void selectModule(const QString &moduleId);
    int visibleFrameXPos();
    bool canNotHide() const;

public slots:
    void setHideInLeft(bool hideInLeft);
    void updateGeometry();
    void setCanNotHide(bool canNotHide);
    void toggle(bool inLeft);

signals:
    void hideInLeftChanged(bool hideInLeft);
    void xChanged();
    void canNotHideChanged(bool canNotHide);

private:
    void listPlugins();

private slots:
    void selectModule(ModuleMetaData metaData);
    void globalMouseReleaseEvent(int button, int x, int y);
    void hideAndShowAnotherSide();
    void hideAndShowAnotherSideFinish();

private:
    HomeScreen *m_homeScreen;
    ContentView *m_contentView;
    QList<ModuleMetaData> m_modules;
    QPropertyAnimation *m_showAni;
    QPropertyAnimation *m_hideAni;
    DBusXMouseArea *m_dbusXMouseArea;
    QWidget *m_centeralWidget;
    QWidget *m_centeralWarpper;
    QWidget *m_leftShadow;
    QWidget *m_rightShadow;

    QString m_dbusFullScreenKey = QString();

    bool m_visible = false;
    bool m_hideInLeft = true;
    bool m_canNotHide = false;
};

#endif
