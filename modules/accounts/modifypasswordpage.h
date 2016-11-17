#ifndef MODIFYPASSWORDPAGE_H
#define MODIFYPASSWORDPAGE_H

#include "contentwidget.h"
#include "user.h"
#include "lineeditwidget.h"

#include <QPushButton>

class ModifyPasswordPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ModifyPasswordPage(User *user, QWidget *parent = 0);

private:
    dcc::LineEditWidget *m_pwdEdit;
    dcc::LineEditWidget *m_pwdEditRepeat;
    QPushButton *m_cancel;
    QPushButton *m_accept;
};

#endif // MODIFYPASSWORDPAGE_H
