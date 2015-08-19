#ifndef ACCOUNTCONTENT_H
#define ACCOUNTCONTENT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QStackedWidget>
#include "dbus/dbusaccountuser.h"
#include "libdui/dsegmentedcontrol.h"
#include "libdui/dswitchlineexpand.h"
#include "webcamavatarpanel.h"
#include "accounttypeline.h"
#include "passwordline.h"
#include "avatargrid.h"
#include "switchline.h"

DUI_USE_NAMESPACE

class UserContent : public QWidget
{
    Q_OBJECT
public:
    explicit UserContent(const QString &userPath, QWidget *parent = 0);

signals:
    void sizeChanged();

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
    PasswordLine *m_passwordLine = NULL;
    AccountTypeLine *m_typeLine = NULL;
    SwitchLine *m_autoLoginLine = NULL;
    SwitchLine *m_lockLine = NULL;


    QString m_userPath = "";

    const int LAYOUT_SPACING = 10;
};

#endif // ACCOUNTCONTENT_H
