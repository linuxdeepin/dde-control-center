#ifndef AVATARGRID_H
#define AVATARGRID_H

#include <QWidget>
#include <QLayout>
#include <QTableWidget>
#include <QHeaderView>
#include "usericon.h"

class AvatarGrid : public QTableWidget
{
    Q_OBJECT
public:
    explicit AvatarGrid(QWidget *parent = 0);

    void setAvatars(const QStringList &list);

signals:
    void avatarSelected(const QString &path);
    void clearAvatar();

private:
    void init();
    void onIconPress();

private:
    const int COLUMN_COUNT = 3;
    const int ICON_SIZE = 103;
};

#endif // AVATARGRID_H
