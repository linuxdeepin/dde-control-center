#include "avatargrid.h"

AvatarGrid::AvatarGrid(const QString &userPath, QWidget *parent)
    : QTableWidget(parent)
{
    m_user = new DBusAccountUser(userPath, this);

    init();
}

void AvatarGrid::setAvatars(const QStringList &list)
{
    int listCount = list.count();
    int rows = listCount / COLUMN_COUNT;
    rows += listCount % COLUMN_COUNT > 0 ? 1 : 0;

    setRowCount(rows);
    setColumnCount(COLUMN_COUNT);

    int listIndex = 0;
    for (int r = 0; r < rows; r ++){

        setRowHeight(r, ICON_SIZE);

        for (int c = 0; c < COLUMN_COUNT; c ++){
            if (listIndex >= listCount)
                break;

            QString iconName = list.at(listIndex ++);
            UserAvatar *icon = new UserAvatar(this, m_user->IsIconDeletable(iconName).value());
            icon->setFixedSize(ICON_SIZE, ICON_SIZE);
            icon->setIcon(iconName);
            connect(icon, &UserAvatar::mousePress, this, &AvatarGrid::onIconPress);
            connect(icon, &UserAvatar::requestDelete, this, &AvatarGrid::onRequestDelete);
            setCellWidget(r, c, icon);  //set and delete old one
        }
    }

    setFixedSize(COLUMN_COUNT * ICON_SIZE, rows * ICON_SIZE);
}

void AvatarGrid::init()
{
    setAttribute(Qt::WA_TranslucentBackground);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    setEditTriggers(NoEditTriggers);
    setDragDropMode(NoDragDrop);
    setSelectionMode(NoSelection);
    setAcceptDrops(false);
    setDragEnabled(false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setShowGrid(false);
}

void AvatarGrid::onIconPress()
{
    UserAvatar * icon = qobject_cast<UserAvatar *>(sender());
    if (icon){
        emit avatarSelected(icon->iconPath());
    }
}

void AvatarGrid::onRequestDelete()
{
    UserAvatar * icon = qobject_cast<UserAvatar *>(sender());
    if (icon){
        m_user->DeleteIconFile(icon->iconPath());
    }
}


