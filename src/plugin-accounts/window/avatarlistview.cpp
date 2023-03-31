//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "avatarlistview.h"
#include "avataritemdelegate.h"
#include "widgets/accessibleinterface.h"

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QDir>
#include <QLabel>
#include <QFileInfo>
#include <QDebug>
#include <QFileInfoList>
#include <QFileDialog>
#include <QStandardPaths>
#include <QRandomGenerator>
#include <QDateTime>

#include <DConfig>

const int MaxAvatarSize = 20;
const int CustomAvatarType = 4;
const int MaxCustomAvatarSize = 4;

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
using namespace DCC_NAMESPACE;

SET_FORM_ACCESSIBLE(AvatarListView,"AvatarListView")
AvatarListView::AvatarListView(const int &role, const int &type, const QString &path, QWidget *parent)
    : DListView(parent)
    , m_currentAvatarRole(role)
    , m_currentAvatarType(type)
    , m_path(path)
    , m_avatarItemModel(new QStandardItemModel(this))
    , m_avatarItemDelegate(new AvatarItemDelegate(role, this))
    , m_avatarSize(QSize(80, 80))
    , m_fd(new QFileDialog(this))
    , m_dconfig(DConfig::create("org.deepin.dde.control-center", QStringLiteral("org.deepin.dde.control-center.accounts"), QString(), this))
{
    initWidgets();
    connect(this, &DListView::clicked, this, &AvatarListView::onItemClicked);
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

QString AvatarListView::getCustomAvatarPath()
{
    auto homeDir = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    auto path = homeDir.first() + QString("/.local/share/icons/");

    return path;
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

    if (m_avatarItemModel->rowCount() > MaxCustomAvatarSize + 1) {
        item->setEnabled(false);
    }
}

QStandardItem *AvatarListView::getCustomAvatar()
{
    QStandardItem *item = m_avatarItemModel->item(1);
    // 默认项MaxAvatarSize个，添加项一个
    if (m_avatarItemModel->rowCount() < MaxCustomAvatarSize + 1) {
        if (m_updateItem) {
            m_updateItem = false;
            return m_avatarItemModel->item(m_currentSelectIndex.row());
        }

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
    if (m_currentAvatarRole == CustomAvatarType) {
        avatarPath = getCustomAvatarPath();
    }
    QDir dir(avatarPath);
    QStringList hideList;
    hideList << "default.png"
             << "guest.png";
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg";       // 设置过滤类型
    dir.setNameFilters(filters); // 设置文件名的过滤
    QFileInfoList list = dir.entryInfoList();

    // 根据文件名进行排序
    std::sort(list.begin(),
              list.end(),
              [&](const QFileInfo &fileinfo1, const QFileInfo &fileinfo2) {
                  return fileinfo1.baseName() < fileinfo2.baseName();
              });

    if (m_currentAvatarRole == 	CustomAvatarType && !list.isEmpty()) {
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

void AvatarListView::requestUpdateCustomAvatar(const QString &path)
{
    addCustomAvatar(path, true);
}

bool AvatarListView::isExistCustomAvatar()
{
    auto path = getCustomAvatarPath();

    QDir dir(path);

    if (dir.entryInfoList().isEmpty()) {
        return false;
    }

    return true;
}

void AvatarListView::addCustomAvatar(const QString &path, bool isFirst)
{
    if (m_avatarItemModel->rowCount() > MaxCustomAvatarSize + 1) {
        return;
    }

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

    if (m_currentSelectIndex.isValid())
        m_avatarItemModel->item(m_currentSelectIndex.row())->setCheckState(Qt::Unchecked);

    if (m_avatarItemModel->item(m_currentSelectIndex.row())) {
        m_avatarItemModel->item(m_currentSelectIndex.row())->setCheckState(Qt::Checked);
        m_avatarItemModel->item(m_currentSelectIndex.row())
                ->setData(QVariant::fromValue(path), AvatarListView::SaveAvatarRole);
    }

    Q_EMIT requestUpdateListView(m_currentAvatarRole, m_currentAvatarType);
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
        if(m_currentSelectIndex.isValid())
            m_avatarItemModel->item(m_currentSelectIndex.row())->setCheckState(Qt::Unchecked);

        m_currentSelectIndex = index;
        m_avatarItemModel->item(m_currentSelectIndex.row())->setCheckState(Qt::Checked);
        m_avatarItemModel->item(m_currentSelectIndex.row())->setData(QVariant::fromValue(filePath), AvatarListView::SaveAvatarRole);
        Q_EMIT requestUpdateListView(m_currentAvatarRole, m_currentAvatarType);
    }
}

void AvatarListView::saveAvatar(const QString &oldPath, const QString &path)
{
    m_updateItem = true;
    QFileInfo info(path);
    QFile::remove(getAvatarPath());

    addCustomAvatar(path, false);
}

QString AvatarListView::getCurrentSelectAvatar() const
{
    return m_currentSelectIndex.data(AvatarListView::SaveAvatarRole).toString();
}

QString AvatarListView::getAvatarPath() const
{
    auto index = QRandomGenerator::global()->bounded(20);
    if (!m_currentSelectIndex.isValid())
        return QString();

    index = m_currentSelectIndex.row();
    auto idx = m_avatarItemModel->index(index, 0);
    return m_avatarItemModel->data(idx, SaveAvatarRole).toString();
}

void AvatarListView::setAvatarSize(const QSize &size)
{
    if (m_avatarSize == size)
        return;

    m_avatarSize = size;

    auto count = m_avatarItemModel->rowCount();
    for (auto i = 0; i < count; ++i) {
        auto idx = m_avatarItemModel->index(i, 0);
        m_avatarItemModel->setData(idx, m_avatarSize, Qt::SizeHintRole);
    }
}