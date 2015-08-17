#ifndef ACCOUNTLISTPANEL_H
#define ACCOUNTLISTPANEL_H

#include <QMap>
#include <QList>
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include "accountexpand.h"
#include "dbus/dbussessionmanager.h"
#include "dbus/dbusaccount.h"
#include "libdui/dexpandgroup.h"

class AccountListPanel : public QWidget
{
    Q_OBJECT
public:
    explicit AccountListPanel(QWidget *parent = 0);

private:
    void initSessionManager();
    void initAccount();
    void onUserAdded(const QString &path);
    void onUserDeleted(const QString &path);
private:
    QVBoxLayout *m_mainLayout = NULL;
    DBusAccount *m_account = NULL;
    DBusSessionManager *m_sessionManager = NULL;
    DExpandGroup *m_expandGroup = NULL;

    QMap<QString, AccountExpand *> m_expands;   //userPath expandObj
};

#endif // ACCOUNTLISTPANEL_H
