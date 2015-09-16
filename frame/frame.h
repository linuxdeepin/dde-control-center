#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include <QList>
#include <QDBusAbstractAdaptor>
#include <QPropertyAnimation>

#include "anchors.h"

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

public:
    Frame(QWidget *parent = 0);
    ~Frame();

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void show(bool imme = false);
    void hide(bool imme = false);
    bool isHideInLeft() const;
    void selectModule(const QString &moduleId);

public slots:
    void setHideInLeft(bool hideInLeft);

signals:
    void hideInLeftChanged(bool hideInLeft);
    void xChanged();

protected:
    void moveEvent(QMoveEvent *e);

private:
    void listPlugins();

private slots:
    void selectModule(ModuleMetaData metaData);
    void globalMouseReleaseEvent(int button, int x, int y);
    void updateFrameGeometry(QRect rect);

private:
    HomeScreen *m_homeScreen;
    ContentView *m_contentView;
    QList<ModuleMetaData> m_modules;
    QPropertyAnimation *m_showAni;
    QPropertyAnimation *m_hideAni;
    DBusXMouseArea *m_dbusXMouseArea;
    QScreen *m_primaryScreen;

    QString m_dbusFullScreenKey = QString();

    bool m_visible = false;
    bool HideInLeft = false;
};

#endif
