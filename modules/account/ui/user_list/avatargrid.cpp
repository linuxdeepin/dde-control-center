#include "avatargrid.h"

AvatarGrid::AvatarGrid(QWidget *parent) : QTableWidget(parent)
{
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

            UserIcon *icon = new UserIcon(this);
            icon->setFixedSize(ICON_SIZE, ICON_SIZE);
            icon->setIcon(list.at(listIndex ++));
            connect(icon, &UserIcon::mousePress, this, &AvatarGrid::onIconPress);
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
    UserIcon * icon = qobject_cast<UserIcon *>(sender());
    if (icon){
        emit avatarSelected(icon->iconPath());
    }
}


