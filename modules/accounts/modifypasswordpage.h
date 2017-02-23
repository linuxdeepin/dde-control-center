#ifndef MODIFYPASSWORDPAGE_H
#define MODIFYPASSWORDPAGE_H

#include "contentwidget.h"
#include "user.h"
#include "lineeditwidget.h"

#include "buttontuple.h"

namespace dcc {
namespace accounts {

class ModifyPasswordPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ModifyPasswordPage(User *user, QWidget *parent = 0);

signals:
    void requestChangePassword(User *userInter, const QString &password) const;

private slots:
    void passwordSubmit();
    void checkPwd();

private:
    User *m_userInter;

    dcc::widgets::LineEditWidget *m_pwdEdit;
    dcc::widgets::LineEditWidget *m_pwdEditRepeat;
    dcc::widgets::ButtonTuple *m_buttonTuple;
};

} // namespace accounts
} // namespace dcc

#endif // MODIFYPASSWORDPAGE_H
