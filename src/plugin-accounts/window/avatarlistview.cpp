// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "avatarlistview.h"

#include "avataritemdelegate.h"
#include "widgets/accessibleinterface.h"

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

#include <DConfig>

const int MaxAvatarSize = 20;
const int MaxCustomAvatarSize = 4;

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
using namespace DCC_NAMESPACE;

AvatarListView::AvatarListView(const int &role,
                               const int &type,
                               const QString &path,
                               QWidget *parent)
    : DListView(parent)
    , m_currentAvatarRole(role)
    , m_currentAvatarType(type)
    , m_path(path)
    , m_avatarItemModel(new QStandardItemModel(this))
    , m_avatarItemDelegate(new AvatarItemDelegate(this))
    , m_avatarSize(QSize(80, 80))
    , m_fd(new QFileDialog(this))
    , m_dconfig(DConfig::create("org.deepin.dde.control-center",
                                QStringLiteral("org.deepin.dde.control-center.accounts"),
                                QString(),
                                this))
{
    initWidgets();
    connect(this, &DListView::clicked, this, [this](const QModelIndex &index) {
        // 用户自定义图片最多只支持四张
        if (m_currentAvatarRole == Custom && index.row() == 0
            && m_avatarItemModel->rowCount() > MaxCustomAvatarSize) {
            return;
        }
        m_save = false;
        onItemClicked(index);
    });

    connect(m_fd, &QFileDialog::finished, this, [=](int result) {
        if (result == QFileDialog::Accepted) {
            const QString path = m_fd->selectedFiles().first();
            QFileInfo info(path);

            m_dconfig->setValue("avatarPath", info.absolutePath());

            int row = -1;
            for (int i = 1; i <= m_avatarItemModel->rowCount(); ++i) {
                if (path == m_avatarItemModel->index(i, 0).data(AvatarListView::SaveAvatarRole)) {
                    row = i;
                    break;
                }
            }

            if (row == -1) {
                return addCustomAvatar(path, false);
            }

            onItemClicked(m_avatarItemModel->index(row, 0));
        }
    });
}

AvatarListView::~AvatarListView()
{
    if (m_fd)
        m_fd->deleteLater();

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

void AvatarListView::initWidgets()
{
    m_fd->setAccessibleName("QFileDialog");

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

    m_fd->setModal(true);
    m_fd->setNameFilter(tr("Images") + "(*.png *.bmp *.jpg *.jpeg)");
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

    QStandardItem *item = m_avatarItemModel->item(1);
    // 默认项MaxAvatarSize个，添加项一个
    if (m_avatarItemModel->rowCount() < MaxCustomAvatarSize + 1) {
        item = new QStandardItem();
        if (m_currentSelectIndex.isValid())
            m_avatarItemModel->item(m_currentSelectIndex.row())->setCheckState(Qt::Unchecked);
        m_avatarItemModel->insertRow(1, item);
    }
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
    std::sort(list.begin(),
              list.end(),
              [&](const QFileInfo &fileinfo1, const QFileInfo &fileinfo2) {
                  return fileinfo1.baseName() < fileinfo2.baseName();
              });

    if (m_currentAvatarRole == Role::Custom && !list.isEmpty()) {
        addLastItem();
    }

    for (int i = 0; i < MaxAvatarSize && i < list.size(); ++i) {
        if (hideList.contains(list.at(i).fileName())) {
            continue;
        }

        QString iconPath = list.at(i).filePath();

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
    addCustomAvatar(path, true);
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

void AvatarListView::addCustomAvatar(const QString &path, bool isFirst)
{
    m_save = true;

    if (isFirst) {
        addLastItem();
    }

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

void AvatarListView::onItemClicked(const QModelIndex &index)
{
    if (index.data(Qt::CheckStateRole) == Qt::Checked)
        return;
    const QString filePath = index.data(SaveAvatarRole).toString();
    if (filePath.isEmpty()) {
        QString dir = m_dconfig->value("avatarPath").toString();
        if (dir.isEmpty() || !QDir(dir).exists()) {
            QStringList directory =
                    QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
            if (!directory.isEmpty()) {
                m_fd->setDirectory(directory.first());
            }
        } else {
            m_fd->setDirectory(dir);
        }
        m_fd->show();
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

    addCustomAvatar(path, false);
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
    if (avatar.startsWith("file://"))
        currentAvatar = QUrl(avatar).toLocalFile();

    if (!QFile(currentAvatar).exists())
        return;

    if (currentAvatar.isEmpty())
        return;

    for (int i = 0; i < m_avatarItemModel->rowCount(); ++i) {
        QString itemAvatar = m_avatarItemModel->index(i, 0).data(AvatarListView::SaveAvatarRole).value<QString>();
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
