#ifndef CREATEUSERPANEL_H
#define CREATEUSERPANEL_H

#include <QTimer>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "dbus/dbusaccount.h"
#include "dbus/dbusaccountuser.h"
#include "libdui/dseparatorhorizontal.h"
#include "../user_list/accounttypeline.h"
#include "useravatar.h"
#include "libdui/dheaderline.h"
#include "confirmbuttonline.h"
#include "passwdline.h"
#include "switchline.h"
#include "inputline.h"

DUI_USE_NAMESPACE

class CreateUserPanel : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString lineBackgroundColor READ lineBackgroundColor WRITE setLineBackgroundColor)
public:
    explicit CreateUserPanel(QWidget *parent = 0);
    QString lineBackgroundColor() const;
    void setLineBackgroundColor(const QString &lineBackgroundColor);
    void preDestroy();

signals:
    void createCancel();
    void createConfirm();

private:
    void initDbusData();
    void initHeader();
    void initInfoLine();
    void initInputLline();
    void initConfirmLine();

private slots:
    bool validate();
    void updateLineStyle();
    void resetData();
    void onCancel();
    void onConfirm();
    void onUserAdded(const QString &path);
    void onNameFocusChanged(bool focus);
    void onPasswdFocusChanged(bool focus);
    void onPasswdRepeatFocusChanged(bool focus);
    void onNameChanged(const QString &name);
    void onPasswdChanged(const QString &);
    void onPasswdRepeatChanged(const QString &passwd);

private:
    QString m_oldName = "";
    UserAvatar *m_avatar = NULL;
    DBusAccount *m_account = NULL;
    QVBoxLayout *m_layout = NULL;
    InputLine *m_nameLine = NULL;
    QLabel *m_newNameLabel = NULL;
    SwitchLine *m_autoLogin = NULL;
    PasswdLine *m_passwdNew = NULL;
    PasswdLine *m_passwdRepeat = NULL;
    AccountTypeLine *m_accountType = NULL;
    ConfirmButtonLine *m_confirmLine = NULL;

    QString m_randIcon = "";
    QString m_lineBackgroundColor = "";

    const int ICON_SIZE = 60;
};

#endif // CREATEUSERPANEL_H
