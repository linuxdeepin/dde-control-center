#ifndef USEREXPANDHEADER_H
#define USEREXPANDHEADER_H

#include <QLabel>
#include <QWidget>
#include <QHBoxLayout>
#include <QMouseEvent>
#include "usericon.h"
#include "usernametitle.h"
#include "dbus/dbusaccountuser.h"
#include "libdui/darrowbutton.h"

DUI_USE_NAMESPACE

class UserExpandHeader : public QLabel
{
    Q_OBJECT
public:
    explicit UserExpandHeader(const QString &userPath, QWidget *parent = 0);
    void setIsCurrentUser(bool isCurrentUser);
    void setExpand(bool value);

signals:
    void mousePress();

protected:
    void mousePressEvent(QMouseEvent *);

private:
    void initData();
    void reverseArrowDirection();
    QString getTypeName(int type);

    void updateIcon();
    void updateAccountName();
    void updateAccountType();

private:
    QString m_userPath = "";
    UserIcon *m_icon = NULL;
    QHBoxLayout *m_mainLayout = NULL;
    UserNameTitle *m_nameTitle = NULL;
    DBusAccountUser *m_accountUser = NULL;
    DArrowButton *m_arrowButton = NULL;

    const int ICON_WIDTH = 118;
    const int ICON_NORMAL_HEIGHT = 100;
    const int ICON_BIG_HEIGHT = 118;
};

#endif // USEREXPANDHEADER_H
