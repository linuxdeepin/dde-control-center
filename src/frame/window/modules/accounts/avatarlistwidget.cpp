/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "avatarlistwidget.h"
#include "modules/accounts/user.h"
#include "avataritemdelegate.h"

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QList>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QFileInfoList>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;

AvatarListWidget::AvatarListWidget(QWidget *parent, bool displayLastItem)
    : QWidget(parent)
    , m_mainContentLayout(new QVBoxLayout())
    , m_avatarListView(new DListView())
    , m_avatarItemModel(new QStandardItemModel())
    , m_avatarItemDelegate(new AvatarItemDelegate())
    , m_prevSelectIndex(-1)
    , m_currentSelectIndex(-1)
    , m_displayLastItem(displayLastItem)
{
    initWidgets();
    initDatas();
}

void AvatarListWidget::initWidgets()
{
    m_mainContentLayout->addWidget(m_avatarListView);

    m_avatarListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_avatarListView->setViewMode(QListView::IconMode);
    m_avatarListView->setDragDropMode(QAbstractItemView::NoDragDrop);
    m_avatarListView->setDragEnabled(false);
    m_avatarListView->setWordWrap(true);
    m_avatarListView->setSpacing(15);
    m_avatarListView->setResizeMode(QListView::Adjust);
    m_avatarListView->setFrameShape(QFrame::NoFrame);
    m_avatarListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_avatarListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_mainContentLayout->setContentsMargins(0, 0, 0, 0);
    m_mainContentLayout->setMargin(0);
    setLayout(m_mainContentLayout);

    connect(m_avatarListView, &QListView::clicked, this, &AvatarListWidget::onItemClicked);
}

void AvatarListWidget::initDatas()
{
    addItemFromDefaultDir();
    if (m_displayLastItem) {
        //如果用户之前添加过新图片，那么用新图片替换最后一张图片；反之直接加上"+"
        QString customPicPath = getUserAddedCustomPicPath();
        if (!customPicPath.isEmpty() && checkFileIsExists(customPicPath)) {
            //先删除最后一张图片
            m_iconpathList.pop_back();
            m_avatarItemModel->removeRow(m_avatarItemModel->rowCount() - 1);
            //添加用户新添加的图片
            addSecond2Last(customPicPath);
        }
        addLastItem();
    }

    m_avatarListView->setItemDelegate(m_avatarItemDelegate);
    m_avatarListView->setModel(m_avatarItemModel);
}

void AvatarListWidget::setUserModel(dcc::accounts::User *user)
{
    m_curUser = user;
}

void AvatarListWidget::setCurrentAvatarChecked(const QString &avatar)
{
    QString currentAvatar;
    if (avatar.startsWith("file://")) {
        currentAvatar = QUrl(avatar).toLocalFile();
    }
    for (int i = 0; i< m_avatarItemModel->rowCount(); ++i) {
        QString itemAvatar = m_avatarItemModel->index(i, 0).data(AvatarListWidget::SaveAvatarRole).value<QString>();
        if (!currentAvatar.isEmpty() && !itemAvatar.isEmpty() && currentAvatar == itemAvatar) {
            if (m_prevSelectIndex != -1) {
                m_avatarItemModel->item(m_prevSelectIndex)->setCheckState(Qt::Unchecked);
            }
            m_prevSelectIndex = i;
            m_avatarItemModel->item(i)->setCheckState(Qt::Checked);
            break;
        } else {
            m_prevSelectIndex = -1;
        }
    }
}

void AvatarListWidget::onItemClicked(const QModelIndex &index)
{
    m_currentSelectIndex = index.row();
    if (index.data(AvatarListWidget::AddAvatarRole).value<LastItemData>().isDrawLast) {
        if (m_prevSelectIndex != -1) {
            m_avatarItemModel->item(m_prevSelectIndex)->setCheckState(Qt::Unchecked);
        }

        if (index.row() == 14) {
            Q_EMIT requestAddNewAvatar(m_curUser);
        }
    } else {
        if (m_prevSelectIndex != -1) {
            m_avatarItemModel->item(m_prevSelectIndex)->setCheckState(Qt::Unchecked);
        }

        m_avatarItemModel->item(index.row())->setCheckState(Qt::Checked);
        Q_EMIT requestSetAvatar(m_iconpathList.at(index.row()));
        if (!m_displayLastItem) {
            m_prevSelectIndex = m_currentSelectIndex;
        }

        m_avatarItemModel->item(index.row())->setCheckState(Qt::Checked);
        Q_EMIT requestSetAvatar(m_iconpathList.at(index.row()));
    }
}

void AvatarListWidget::onAddNewAvatarSuccess(bool added)
{
    if (added) {
        //1.先把最后２项删除
        for (int i = 0; i < 2; ++i) {
            m_iconpathList.pop_back();
            m_avatarItemModel->removeRow(m_avatarItemModel->rowCount() - 1);
        }

        //2.重新添加最后２项
        QString newiconpath = getUserAddedCustomPicPath();
        if (!newiconpath.isEmpty() && checkFileIsExists(newiconpath)) {
            addSecond2Last(newiconpath);
        }
        addLastItem();

        //3.选中当前的新添加的图像
        int index = m_avatarItemModel->rowCount() - 2;
        m_avatarItemModel->item(index)->setCheckState(Qt::Checked);
        m_prevSelectIndex = index;
    } else {
        if (m_prevSelectIndex != -1) {
            m_avatarItemModel->item(m_prevSelectIndex)->setCheckState(Qt::Checked);
        }
    }
}

void AvatarListWidget::addItemFromDefaultDir()
{
    QString dirpath("/var/lib/AccountsService/icons/");
    QDir dir(dirpath);
    QStringList hideList;
    hideList << "default.png" << "guest.png";
    QStringList filters;
    filters << "*.png";//设置过滤类型
    dir.setNameFilters(filters);//设置文件名的过滤
    QFileInfoList list = dir.entryInfoList();
    //根据文件名进行排序
    qSort(list.begin(), list.end(), [&](const QFileInfo &fileinfo1, const QFileInfo &fileinfo2) {
        return fileinfo1.baseName() < fileinfo2.baseName();
    });
    for (int i = 0; i < list.size(); ++i) {
        if (hideList.contains(list.at(i).fileName())) {
            continue;
        }
        QString iconpath = list.at(i).filePath();
        m_iconpathList.push_back(iconpath);
        QStandardItem *item = new QStandardItem();
        auto ratio = devicePixelRatioF();
        if (ratio > 1.0) {
            iconpath.replace("icons/", "icons/bigger/");
        }
        auto px = QPixmap(iconpath).scaled(QSize(74, 74) * ratio,
                                           Qt::KeepAspectRatio, Qt::SmoothTransformation);
        px.setDevicePixelRatio(ratio);

        item->setData(QVariant::fromValue(px), Qt::DecorationRole);
        item->setData(QVariant::fromValue(iconpath), AvatarListWidget::SaveAvatarRole);
        m_avatarItemModel->appendRow(item);
    }
}

void AvatarListWidget::addLastItem()
{
    QString iconpath = "Deepin";
    LastItemData lastItemData;
    lastItemData.isDrawLast = true;
    lastItemData.iconPath = iconpath;

    m_iconpathList.push_back(iconpath);
    QStandardItem *item = new QStandardItem();
    item->setData(QVariant::fromValue(lastItemData), AvatarListWidget::AddAvatarRole);
    m_avatarItemModel->appendRow(item);
}

void AvatarListWidget::addSecond2Last(QString newiconpath)
{
    m_iconpathList << newiconpath;
    QStandardItem *item = new QStandardItem();
    item->setData(QVariant::fromValue(QPixmap(newiconpath)), Qt::DecorationRole);
    item->setData(QVariant::fromValue(newiconpath), AvatarListWidget::SaveAvatarRole);
    m_avatarItemModel->appendRow(item);
}

QString AvatarListWidget::getUserAddedCustomPicPath()
{
    QString newiconpath;
    QString dirpath("/var/lib/AccountsService/icons/local/");
    QDir dir(dirpath);
    QFileInfoList list = dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);//去除.和..
    if (list.size() > 0) { //如果之前添加过新图片
        for (int i = 0; i < list.size(); ++i) {
            QString iconpath = list.at(i).filePath();
            newiconpath = iconpath;
            break;
        }
    } else {
        QString iconpath = "Deepin";
        newiconpath = iconpath;
    }

    return newiconpath;
}

bool AvatarListWidget::checkFileIsExists(QString path)
{
    QFileInfo check_file(path);
    return check_file.exists() && check_file.isFile();
}

QString AvatarListWidget::getAvatarPath(int n) const
{
    return  m_iconpathList.at(n);
}

int AvatarListWidget::getCurrentSelectIndex() const
{
    return m_currentSelectIndex;
}
