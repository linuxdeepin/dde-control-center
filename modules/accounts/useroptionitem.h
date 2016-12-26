#ifndef USEROPTIONITEM_H
#define USEROPTIONITEM_H

#include "nextpagewidget.h"

class QLabel;

namespace dcc {

namespace accounts {

class UserOptionItem : public widgets::NextPageWidget
{
    Q_OBJECT

public:
    explicit UserOptionItem(QFrame *parent = 0);

public slots:
    void setAvatar(const QString &avatar);

private:
    QLabel *m_avatarLabel;
};

}

}

#endif // USEROPTIONITEM_H
