// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "avatarlistview.h"

#include "avataritemdelegate.h"
#include "widgets/accessibleinterface.h"

#include <DConfig>

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileInfoList>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QPixmap>
#include <QRandomGenerator>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QWidget>

const QString VarDirectory = QStringLiteral(VARDIRECTORY);
const QString DefaultAvatar =
        QStringLiteral("lib/AccountsService/icons/animal/dimensional/raccoon.png");

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
using namespace DCC_NAMESPACE;

AvatarListView::AvatarListView(
        User *user, const int &role, const int &type, const QString &path, QWidget *parent)
    : DListView(parent)
    , m_currentAvatarRole(role)
    , m_currentAvatarType(type)
    , m_path(path)
    , m_avatarItemModel(new QStandardItemModel(this))
    , m_avatarItemDelegate(new AvatarItemDelegate(m_currentAvatarRole == Custom, this))
    , m_avatarSize(QSize(80, 80))
    , m_curUser(user)
    , m_dconfig(DConfig::create("org.deepin.dde.control-center",
                                QStringLiteral("org.deepin.dde.control-center.accounts"),
                                QString(),
                                this))
{
    initWidgets();
    installEventFilter(this);
    connect(this, &DListView::clicked, this, [this](const QModelIndex &index) {
        m_save = false;
        onItemClicked(index);
    });
}

AvatarListView::~AvatarListView()
{

    if (m_avatarItemModel) {
        m_avatarItemModel->clear();
        m_avatarItemModel->deleteLater();
        m_avatarItemModel = nullptr;
    }
    if (m_avatarItemDelegate) {
        m_avatarItemDelegate->deleteLater();
        m_avatarItemDelegate = nullptr;
    }
}

void AvatarListView::updateGeometries()
{
    DListView::updateGeometries();
    if (model()->rowCount() == 0) {
        return;
    }
    QRect r = rectForIndex(model()->index(model()->rowCount() - 1, 0));
    QMargins margins = viewportMargins();
    setFixedHeight(r.y() + r.height() + margins.top() + margins.bottom() + 1);
}

bool AvatarListView::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() != QEvent::KeyPress) {
        return QObject::eventFilter(object, event);
    }
    QKeyEvent *keyevent = static_cast<QKeyEvent *>(event);
    if (keyevent->key() != Qt::Key_Tab) {
        return QObject::eventFilter(object, event);
    }
    bool is_back_select = keyevent->modifiers() == Qt::Modifier::CTRL;
    int count = m_avatarItemModel->rowCount();
    int current_index = m_currentSelectIndex.row();
    int next_index = 0;
    if (is_back_select) {
        if (current_index == 0) {
            next_index = count - 1;
        } else {
            next_index = current_index - 1;
        }
    } else {
        if (current_index == count - 1) {
            next_index = 0;
        } else {
            next_index = current_index + 1;
        }
    }

    if (m_currentSelectIndex.isValid()) {
        m_avatarItemModel->item(m_currentSelectIndex.row())->setCheckState(Qt::Unchecked);
    }
    m_currentSelectIndex = m_avatarItemModel->index(next_index, 0);
    const QString filePath = m_currentSelectIndex.data(SaveAvatarRole).toString();
    m_avatarItemModel->item(m_currentSelectIndex.row())->setCheckState(Qt::Checked);
    m_avatarItemModel->item(m_currentSelectIndex.row())
            ->setData(QVariant::fromValue(filePath), AvatarListView::SaveAvatarRole);
    Q_EMIT requestUpdateListView(m_save, m_currentAvatarRole, m_currentAvatarType);

    return true;
}

void AvatarListView::initWidgets()
{
    setViewMode(ViewMode::IconMode);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setContentsMargins(0, 0, 0, 0);
    setSpacing(2);
    setItemAlignment(Qt::AlignTop);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setDragDropMode(QAbstractItemView::NoDragDrop);
    setDragEnabled(false);
    setResizeMode(DListView::Adjust);
    setFrameShape(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setItemDelegate(m_avatarItemDelegate);
    setModel(m_avatarItemModel);

    addItemFromDefaultDir(m_path);
}

void AvatarListView::addLastItem()
{
    DStandardItem *item = new DStandardItem();
    item->setAccessibleText("LastItem");
    item->setData(m_avatarSize, Qt::SizeHintRole);
    item->setData("", AvatarListView::SaveAvatarRole);
    item->setData(true, AvatarListView::AddAvatarRole);
    m_avatarItemModel->appendRow(item);
}

QStandardItem *AvatarListView::getCustomAvatar()
{
    // 用户编辑修改的头像直接替换原来的头像
    if (m_updateItem) {
        return m_avatarItemModel->item(m_currentSelectIndex.row());
    }

    QStandardItem *item = new QStandardItem;
    // 默认项MaxAvatarSize个，添加项一个
    if (m_currentSelectIndex.isValid())
        m_avatarItemModel->item(m_currentSelectIndex.row())->setCheckState(Qt::Unchecked);
    m_avatarItemModel->insertRow(1, item);
    return item;
}

void AvatarListView::addItemFromDefaultDir(const QString &path)
{
    auto avatarPath = path;

    QDir dir(avatarPath);
    QStringList hideList;
    hideList << "default.png"
             << "guest.png";
    QStringList filters;
    filters << "*.png"
            << "*.jpg"
            << "*.jpeg";         // 设置过滤类型
    dir.setNameFilters(filters); // 设置文件名的过滤
    QFileInfoList list = dir.entryInfoList();

    // 根据文件名进行排序
    std::sort(list.begin(), list.end(), [](const QFileInfo &fileinfo1, const QFileInfo &fileinfo2) {
        return fileinfo1.baseName() < fileinfo2.baseName();
    });

    const auto &name = m_curUser->name();
    // 当前用户有自定义用户头像时, 需要添加用户添加按钮, 否则不需要添加
    if (m_currentAvatarRole == Custom) {
        auto res = std::find_if(list.cbegin(), list.cend(), [name](const QFileInfo &info) -> bool {
            return info.filePath().contains(name + "-");
        });

        if (res != list.cend()) {
            addLastItem();
        }
    }

    for (int i = 0; i < list.size(); ++i) {
        if (hideList.contains(list.at(i).fileName())) {
            continue;
        }

        QString iconPath = list.at(i).filePath();

        if (m_currentAvatarRole == Custom) {
            // 过滤掉非当前用户自定义头像
            if (!iconPath.contains(name + "-")) {
                continue;
            }
        }

        DStandardItem *item = new DStandardItem();
        item->setBackgroundRole(QPalette::ColorRole::Highlight);
        item->setAccessibleText(iconPath);
        auto ratio = devicePixelRatioF();

        auto pxPath = iconPath;
        auto px = QPixmap(pxPath).scaled(QSize(74, 74) * ratio,
                                         Qt::KeepAspectRatio,
                                         Qt::FastTransformation);
        px.setDevicePixelRatio(ratio);

        item->setData(QVariant::fromValue(px), Qt::DecorationRole);
        item->setData(QVariant::fromValue(iconPath), AvatarListView::SaveAvatarRole);
        item->setData(m_avatarSize, Qt::SizeHintRole);
        m_avatarItemModel->appendRow(item);
    }
}

void AvatarListView::setCurrentAvatarUnChecked()
{
    if (m_currentSelectIndex.isValid()) {
        m_avatarItemModel->setData(m_currentSelectIndex, Qt::Unchecked, Qt::CheckStateRole);
    }
}

void AvatarListView::requestAddCustomAvatar(const QString &path)
{
    addLastItem();
    addCustomAvatar(path);
}

void AvatarListView::requestUpdateCustomAvatar(const QString &path)
{
    if (m_currentAvatarRole != Custom) {
        return;
    }

    m_avatarItemModel->item(m_currentSelectIndex.row())
            ->setData(QVariant::fromValue(QUrl(path).toLocalFile()),
                      AvatarListView::SaveAvatarRole);
}

void AvatarListView::addCustomAvatar(const QString &path)
{
    m_save = true;

    QStandardItem *item = getCustomAvatar();
    item->setAccessibleText(path);
    auto ratio = devicePixelRatioF();
    auto px = QPixmap(path).scaled(QSize(74, 74) * ratio,
                                   Qt::KeepAspectRatio,
                                   Qt::FastTransformation);
    px.setDevicePixelRatio(ratio);

    item->setData(QVariant::fromValue(px), Qt::DecorationRole);
    item->setData(QVariant::fromValue(path), AvatarListView::SaveAvatarRole);
    item->setData(m_avatarSize, Qt::SizeHintRole);

    if (m_updateItem) {
        onItemClicked(m_avatarItemModel->index(m_currentSelectIndex.row(), 0));
        m_updateItem = false;
    } else {
        onItemClicked(m_avatarItemModel->index(1, 0));
    }
}

bool AvatarListView::checkIsToDeleteAvatar(const QModelIndex &index)
{
    if (m_currentAvatarRole != Custom) {
        return false;
    }
    if (index.row() == 0) {
        return false;
    }
    auto pos = mapFromGlobal(QCursor::pos());
    auto rect = visualRect(index);
    auto rectTopRight = rect.topRight();
    QRect closeBtnRegion =
            QRect{ rectTopRight.x() - 16, rectTopRight.y(), 16, 16 };
    if (closeBtnRegion.contains(pos)) {
        return true;
    }
    return false;
}

void AvatarListView::onItemClicked(const QModelIndex &index)
{
    // NOTE: as design, the last one should not bbe deleted
    if (checkIsToDeleteAvatar(index) && m_avatarItemModel->rowCount() > 2) {
        QString iconPath = index.data(AvatarListView::SaveAvatarRole).toString();
        Q_EMIT requestDeleteUserIcon(iconPath);
        m_avatarItemModel->removeRow(index.row());
        if (m_avatarItemModel->rowCount() >= 1) {
            m_currentSelectIndex = m_avatarItemModel->index(1, 0);
        } else {
            m_currentSelectIndex = m_avatarItemModel->index(-1, 0);
        }

        for (int i = 1; i < m_avatarItemModel->rowCount(); ++i) {
            auto currentItem = m_avatarItemModel->item(i);
            if (currentItem->index() == m_currentSelectIndex) {
                currentItem->setCheckState(Qt::Checked);
            } else {
                currentItem->setCheckState(Qt::Unchecked);
            }
        }

        return;
    }
    // check if is x button, if is x button, then delete the model, and request to remove the icon
    if (index.data(Qt::CheckStateRole) == Qt::Checked)
        return;
    const QString filePath = index.data(SaveAvatarRole).toString();
    if (filePath.isEmpty()) {
        QFileDialog dialog;
        dialog.setNameFilter(tr("Images") + "(*.png *.bmp *.jpg *.jpeg)");
        QString dir = m_dconfig->value("avatarPath").toString();
        if (dir.isEmpty() || !QDir(dir).exists()) {
            QStringList directory =
                    QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
            if (!directory.isEmpty()) {
                dialog.setDirectory(directory.first());
            }
        } else {
            dialog.setDirectory(dir);
        }
        if (dialog.exec() == QFileDialog::Accepted) {
            const QString path = dialog.selectedFiles().first();
            QFileInfo info(path);

            m_dconfig->setValue("avatarPath", info.absolutePath());

            int row = -1;
            for (int i = 1; i < m_avatarItemModel->rowCount(); ++i) {
                if (path == m_avatarItemModel->index(i, 0).data(AvatarListView::SaveAvatarRole)) {
                    row = i;
                    break;
                }
            }

            if (row == -1) {
                return addCustomAvatar(path);
            }

            onItemClicked(m_avatarItemModel->index(row, 0));
        }
    } else {
        if (m_currentSelectIndex.isValid())
            m_avatarItemModel->item(m_currentSelectIndex.row())->setCheckState(Qt::Unchecked);

        m_currentSelectIndex = index;
        m_avatarItemModel->item(m_currentSelectIndex.row())->setCheckState(Qt::Checked);
        m_avatarItemModel->item(m_currentSelectIndex.row())
                ->setData(QVariant::fromValue(filePath), AvatarListView::SaveAvatarRole);
        Q_EMIT requestUpdateListView(m_save, m_currentAvatarRole, m_currentAvatarType);
        m_save = false;
    }
}

void AvatarListView::saveAvatar(const QString &path)
{
    m_updateItem = true;

    addCustomAvatar(path);
}

QString AvatarListView::getAvatarPath() const
{
    if (!m_currentSelectIndex.isValid())
        return QString();

    auto index = m_currentSelectIndex.row();
    auto idx = m_avatarItemModel->index(index, 0);
    return m_avatarItemModel->data(idx, SaveAvatarRole).toString();
}

void AvatarListView::setCurrentAvatarChecked(const QString &avatar)
{
    if (avatar.isEmpty())
        return;

    QString currentAvatar = avatar;
    const QString urlPre = "file://";
    // 如果是默认的头像, 需要将路径换成实际的路径
    if (currentAvatar.contains("default")) {
        const auto defaultAvatar =
                QString("%1%2/%3").arg(urlPre).arg(VarDirectory).arg(DefaultAvatar);
        currentAvatar = defaultAvatar;
    }

    if (currentAvatar.startsWith(urlPre))
        currentAvatar = QUrl(currentAvatar).toLocalFile();

    if (!QFile(currentAvatar).exists())
        return;

    if (currentAvatar.isEmpty())
        return;

    for (int i = 0; i < m_avatarItemModel->rowCount(); ++i) {
        QString itemAvatar = m_avatarItemModel->index(i, 0)
                                     .data(AvatarListView::SaveAvatarRole)
                                     .value<QString>();
        if (currentAvatar != itemAvatar)
            continue;

        if (m_currentSelectIndex.isValid()) {
            m_avatarItemModel->setData(m_currentSelectIndex, Qt::Unchecked, Qt::CheckStateRole);
        }
        m_avatarItemModel->item(i)->setCheckState(Qt::Checked);
        m_currentSelectIndex = m_avatarItemModel->index(i, 0);
        break;
    }
}
