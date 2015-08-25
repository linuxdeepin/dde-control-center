#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include <QList>
#include <QDBusAbstractAdaptor>
#include <QPropertyAnimation>

#include "interfaces.h"
#include "modulemetadata.h"
#include "dbuscontrolcenter.h"

class QStackedLayout;
class QKeyEvent;
class HomeScreen;
class ContentView;
class DBusControlCenter;
class Frame: public QFrame
{
    Q_OBJECT

public:
    Frame(QWidget * parent = 0);
    ~Frame();

    void changeEvent(QEvent *e);
    void keyPressEvent(QKeyEvent * event) Q_DECL_OVERRIDE;
    void show(bool imme = false);
    void hide(bool imme = false);
    bool isHideInLeft() const;
    void selectModule(const QString & moduleId);

private:
    void listPlugins();

private slots:
    void selectModule(ModuleMetaData metaData);

private:
    HomeScreen * m_homeScreen;
    ContentView * m_contentView;
    QList<ModuleMetaData> m_modules;
    QPropertyAnimation *m_showAni;
    QPropertyAnimation *m_hideAni;

    bool m_visible = false;

    bool HideInLeft = false;
};

#endif
