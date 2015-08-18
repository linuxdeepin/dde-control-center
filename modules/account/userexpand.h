#ifndef ACCOUNTEXPAND_H
#define ACCOUNTEXPAND_H

#include <QWidget>
#include <QResizeEvent>
#include "libdui/dbaseexpand.h"
#include "userheader.h"
#include "UserContent/usercontent.h"

DUI_USE_NAMESPACE

class UserExpand : public DBaseExpand
{
    Q_OBJECT
public:
    explicit UserExpand(const QString &userPath, QWidget *parent = 0);
    void setIsCurrentUser(bool v);
    void setExpand(bool v);

    QString userPath() const;

private:
    QString m_userPath = "";
    UserHeader *m_header = NULL;
    UserContent *m_content = NULL;

    const int HEADER_HEIGHT = 100;
    const int HEADER_CURRENT_USER_HEIGHT = 118;
};

#endif // ACCOUNTEXPAND_H
