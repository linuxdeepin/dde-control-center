#ifndef MODIFYAVATARPAGE_H
#define MODIFYAVATARPAGE_H

#include "contentwidget.h"
#include "user.h"

#include <QGridLayout>

class ModifyAvatarPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ModifyAvatarPage(User *user, QWidget *parent = 0);

signals:
    void requestAddNewAvatar(User *user) const;

private slots:
    void updateAvatarList(const QList<QString> &avatars);

private:
    User *m_userInter;
    QGridLayout *m_avatarsLayout;
};

#endif // MODIFYAVATARPAGE_H
