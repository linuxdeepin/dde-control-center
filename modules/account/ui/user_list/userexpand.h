#ifndef ACCOUNTEXPAND_H
#define ACCOUNTEXPAND_H

#include <QWidget>
#include <QResizeEvent>
#include "libdui/dbaseexpand.h"
#include "userexpandheader.h"
#include "userexpandcontent.h"

DUI_USE_NAMESPACE

class UserExpand : public DBaseExpand
{
    Q_OBJECT
public:
    explicit UserExpand(const QString &userPath, QWidget *parent = 0);
    void setIsCurrentUser(bool v);
    void setExpand(bool v);
    QString userPath() const;

signals:
    void requestDelete(bool flag);
    void cancelDelete();

private:
    QString m_userPath = "";
    UserExpandHeader *m_header = NULL;
    UserExpandContent *m_content = NULL;

    const int HEADER_HEIGHT = 100;
    const int HEADER_CURRENT_USER_HEIGHT = 118;
};

#endif // ACCOUNTEXPAND_H
