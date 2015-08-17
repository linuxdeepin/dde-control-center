#ifndef ACCOUNTEXPAND_H
#define ACCOUNTEXPAND_H

#include <QWidget>
#include "libdui/dbaseexpand.h"
#include "accountheader.h"
#include "accountcontent.h"

DUI_USE_NAMESPACE

class AccountExpand : public DBaseExpand
{
    Q_OBJECT
public:
    explicit AccountExpand(const QString &userPath, QWidget *parent = 0);
    void setIsCurrentUser(bool v);
    void setExpand(bool v);

    QString userPath() const;

private:
    QString m_userPath = "";
    AccountHeader *m_header = NULL;
    AccountContent *m_content = NULL;

    const int HEADER_HEIGHT = 100;
    const int HEADER_CURRENT_USER_HEIGHT = 118;
};

#endif // ACCOUNTEXPAND_H
