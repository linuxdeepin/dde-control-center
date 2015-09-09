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
class DBusControlCenter;
class Frame: public QFrame
{
    Q_OBJECT

public:
    Frame(QWidget *parent = 0);
    ~Frame();

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void show(bool imme = false);
    void hide(bool imme = false);
    bool isHideInLeft() const;
    void selectModule(const QString &moduleId);

private:
    void listPlugins();

private slots:
    void selectModule(ModuleMetaData metaData);
    void globalMouseReleaseEvent(int button, int x, int y);

private:
    HomeScreen *m_homeScreen;
    ContentView *m_contentView;
    QList<ModuleMetaData> m_modules;
    QPropertyAnimation *m_showAni;
    QPropertyAnimation *m_hideAni;
    DBusXMouseArea *m_dbusXMouseArea;

    QString m_dbusFullScreenKey = QString();

    bool m_visible = false;
    bool HideInLeft = false;
};

#endif
