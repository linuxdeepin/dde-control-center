#ifndef USEREXPANDCONTENT_H
#define USEREXPANDCONTENT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QFileDialog>
#include <QDir>
#include "dbus/dbusaccountuser.h"
#include "libdui/dsegmentedcontrol.h"
#include "libdui/dswitchlineexpand.h"
#include "webcamavatarpanel.h"
#include "accounttypeline.h"
#include "passwordframe.h"
#include "avatargrid.h"
#include "switchline.h"

DUI_USE_NAMESPACE

class UserExpandContent : public QWidget
{
    Q_OBJECT
public:
    explicit UserExpandContent(const QString &userPath = "", QWidget *parent = 0);
    void setIsCurrentUser(bool isCurrentUser);

public slots:
    void onRequestPreDestroy();

signals:
    void sizeChanged();
    void changeToSetting(bool value);

private:
    void initSegmentedControl();
    void initAvatarPanel();
    void initAutoLogin();
    void initUserEnable();
    void initAccountType();
    void initPassword();

    void onAvatarSelected(const QString &avatar);

    void updateSize(bool note = true);

private:
    QVBoxLayout *m_mainLayout = NULL;
    DBusAccountUser *m_accountUser = NULL;
    QStackedWidget *m_stackWidget = NULL;
    DSegmentedControl *m_segmentedControl = NULL;
    PasswordFrame *m_passwordFrame = NULL;
    AccountTypeLine *m_typeLine = NULL;
    SwitchLine *m_autoLoginLine = NULL;
    SwitchLine *m_lockLine = NULL;

    QString m_userPath = "";

    const int LAYOUT_SPACING = 10;
    const QString ADD_AVATAR_ICON = ":/images/dark/images/avatar_add.png";
};

#endif // USEREXPANDCONTENT_H
