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

private slots:
    void updateAvatarList(const QList<QString> &avatars);

private:
    QGridLayout *m_avatarsLayout;
};

#endif // MODIFYAVATARPAGE_H
