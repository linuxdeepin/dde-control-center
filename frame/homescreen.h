#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include "modulemetadata.h"
#include "dbus/dbusaccounts.h"
#include "dbus/dbususer.h"

#include <QFrame>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class QLabel;
class QGridLayout;
class HomeScreen : public QFrame
{
    Q_OBJECT
public:
    explicit HomeScreen(QList<ModuleMetaData> modules, QWidget *parent = 0);
    ~HomeScreen();

    void hide();
    void show();

signals:
    void moduleSelected(ModuleMetaData);

public slots:
    void powerButtonClicked();

private slots:
    void buttonClicked();
    void userAvatarClicked();

private:
    QGridLayout * m_grid;

    DBusAccounts *m_dbusAccounts;
    DBusUser *m_dbusUserInfo;

    QGraphicsOpacityEffect *m_opacityEffect;
    QPropertyAnimation *m_topAni;
    QPropertyAnimation *m_ctrShowAni;
    QPropertyAnimation *m_ctrHideAni;
    QPropertyAnimation *m_botAni;

    QWidget *m_topWidget;
    QWidget *m_centerWidget;
    QWidget *m_bottomWidget;

    QList<ModuleMetaData> modules;

    int m_moduleCount = 0;
};

class QEvent;
class QMouseEvent;
class ModuleButton : public QFrame
{
    Q_OBJECT
public:
    ModuleButton(ModuleMetaData metaData, QWidget * parent = 0);

    void enterEvent(QEvent * event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent * event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

    ModuleMetaData metaData();

signals:
    void clicked();

private:
    enum State { Normal, Hover };

private:
    void setState(State state);

private:
    ModuleMetaData m_meta;

    QLabel * m_icon;
    QLabel * m_text;
};

#endif // HOMESCREEN_H
