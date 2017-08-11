#ifndef MODIFYFULLNAMEPAGE_H
#define MODIFYFULLNAMEPAGE_H

#include "contentwidget.h"

namespace dcc {

namespace widgets {

class LineEditWidget;

}

namespace accounts {

class User;
class ModifyFullNamePage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ModifyFullNamePage(User *u, QWidget *parent = 0);

signals:
    void requestSetFullname(User *u, const QString &n) const;

private slots:
    void onFullnameChanged(const QString &fn);

private:
    User *m_user;
    widgets::LineEditWidget *m_fullnameWidget;
};

}

}

#endif // MODIFYFULLNAMEPAGE_H
