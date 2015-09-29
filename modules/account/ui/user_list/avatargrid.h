#ifndef AVATARGRID_H
#define AVATARGRID_H

#include <QWidget>
#include <QLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QButtonGroup>
#include "dbus/dbusaccountuser.h"

#include "useravatar.h"

class AvatarGrid : public QTableWidget
{
    Q_OBJECT
public:
    explicit AvatarGrid(const QString &userPath = "", QWidget *parent = 0);

    void setAvatars(const QStringList &list);

signals:
    void avatarSelected(const QString &path);
    void clearAvatar();

private:
    void init();
    void clearUp();
    void onIconPress();
    void onRequestDelete();
    void onIconFileChanged();

private slots:
    void onButtonToggled(QAbstractButton * button, bool checked);

private:
    QButtonGroup *m_buttonGroup = NULL;
    DBusAccountUser *m_user = NULL;

    const int COLUMN_COUNT = 3;
    const int ICON_SIZE = 103;
};

#endif // AVATARGRID_H
