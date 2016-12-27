#ifndef MODIFYAVATARPAGE_H
#define MODIFYAVATARPAGE_H

#include "contentwidget.h"
#include "user.h"

#include <QGridLayout>

namespace dcc {
namespace accounts {

class ModifyAvatarPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ModifyAvatarPage(User *user, QWidget *parent = 0);

signals:
    void requestAddNewAvatar(User *user) const;
    void requestSetAvatar(User *user, const QString &filePath) const;
    void requestDeleteAvatar(User *user, const QString &iconPath) const;

private:
    void appendAvatar(const QString &avatar, const int index, const bool selected, const bool deletable = false);

private slots:
    void updateAvatarList();

private:
    User *m_userInter;
    QGridLayout *m_avatarsLayout;
};

} // namespace accounts
} // namespace dcc

#endif // MODIFYAVATARPAGE_H
