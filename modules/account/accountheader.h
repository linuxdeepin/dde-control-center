#ifndef ACCOUNTHEADER_H
#define ACCOUNTHEADER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QMouseEvent>
#include "accounticon.h"
#include "accountnametitle.h"
#include "dbus/dbusaccountuser.h"
#include "libdui/darrowbutton.h"

DUI_USE_NAMESPACE

class AccountHeader : public QWidget
{
    Q_OBJECT
public:
    explicit AccountHeader(const QString &userPath, QWidget *parent = 0);
    void setIcon(const QString &iconPath);
    void setAccountName(const QString &name);
    void setAccountType(const QString &type);
    void setIsCurrentUser(bool isCurrentUser);
    void setExpand(bool value);

signals:
    void mousePress();

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    void initData();
    void reverseArrowDirection();
    QString getTypeName(int type);

private:
    QString m_userPath = "";
    AccountIcon *m_icon = NULL;
    QHBoxLayout *m_mainLayout = NULL;
    AccountNameTitle *m_nameTitle = NULL;
    DBusAccountUser *m_accountUser = NULL;
    DArrowButton *m_arrowButton = NULL;

    const int ICON_WIDTH = 118;
    const int ICON_NORMAL_HEIGHT = 100;
    const int ICON_BIG_HEIGHT = 118;
};

#endif // ACCOUNTHEADER_H
