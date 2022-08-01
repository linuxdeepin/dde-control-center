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
#include "src/plugin-accounts/operation/user.h"
#include "widgets/accessibleinterface.h"
#include "avataritemdelegate.h"

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QFileInfoList>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDateTime>
#include <QRandomGenerator>
#include <DSuggestButton>
#include <DConfig>

const int MaxAvatarSize = 14;

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DCC_USE_NAMESPACE
SET_FORM_ACCESSIBLE(AvatarListWidget,"AvatarListWidget")
AvatarListWidget::AvatarListWidget(User *usr, QWidget *parent)
    : DListView(parent)
    , m_curUser(usr)
    , m_avatarItemModel(new QStandardItemModel(this))
    , m_avatarItemDelegate(new AvatarItemDelegate(this))
    , m_avatarSize(QSize(90, 90))
    , m_fd(new QFileDialog(this))
    , m_dconfig(DConfig::create("org.deepin.dde.control-center", QStringLiteral("org.deepin.dde.control-center.accounts"), QString(), this))
{
    initWidgets();

    connect(this, &DListView::clicked, this, &AvatarListWidget::onItemClicked);
    connect(m_fd, &QFileDialog::finished, this, [=](int result) {
        if (result == QFileDialog::Accepted) {
            const QString iconpath = m_fd->selectedFiles().first();

            QFileInfo info(iconpath);
            m_dconfig->setValue("avatarPath",info.absolutePath());

            int row = -1;
            for (int i = 1; i <= m_avatarItemModel->rowCount(); ++i) {
                if (iconpath == m_avatarItemModel->index(i, 0).data(AvatarListWidget::SaveAvatarRole)) {
                    row = i;
                    break;
                }
            }
            if (row == -1) {
                QStandardItem *item = getAndSelectCustomAvatar();
                item->setAccessibleText(iconpath);
                auto ratio = devicePixelRatioF();
                auto px = QPixmap(iconpath).scaled(QSize(74, 74) * ratio,
                                                   Qt::KeepAspectRatio, Qt::FastTransformation);
                px.setDevicePixelRatio(ratio);

                item->setData(QVariant::fromValue(px), Qt::DecorationRole);
                item->setData(QVariant::fromValue(iconpath), AvatarListWidget::SaveAvatarRole);
                item->setData(m_avatarSize, Qt::SizeHintRole);
                row = 1;
            }
            onItemClicked(m_avatarItemModel->index(row, 0));
        }
    });
}

AvatarListWidget::~AvatarListWidget()
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

void AvatarListWidget::initWidgets()
{
    m_fd->setAccessibleName("QFileDialog");

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setViewMode(QListView::IconMode);
    setDragDropMode(QAbstractItemView::NoDragDrop);
    setDragEnabled(false);
    setSpacing(4);
    setResizeMode(DListView::Adjust);
    setFrameShape(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setItemDelegate(m_avatarItemDelegate);
    setModel(m_avatarItemModel);

    addLastItem();
    addItemFromDefaultDir();

    m_fd->setModal(true);
    m_fd->setNameFilter(tr("Images") + "(*.png *.bmp *.jpg *.jpeg)");

    if (m_curUser) {
        refreshCustomAvatar(getUserAddedCustomPicPath(m_curUser->name()));
        setCurrentAvatarChecked(m_curUser->currentAvatar());
    }
}

void AvatarListWidget::refreshCustomAvatar(const QString &str)
{
    QString customPicPath = str;
    if (customPicPath.isEmpty())
        return;

    QStandardItem *item = getAndSelectCustomAvatar();

    item->setData(QVariant::fromValue(QPixmap(customPicPath)), Qt::DecorationRole);
    item->setData(QVariant::fromValue(customPicPath), AvatarListWidget::SaveAvatarRole);
    item->setData(m_avatarSize, Qt::SizeHintRole);

    if (m_currentSelectIndex.isValid() && m_currentSelectIndex != item->index()) {
        m_avatarItemModel->setData(m_currentSelectIndex, Qt::Unchecked, Qt::CheckStateRole);
    }
    item->setCheckState(Qt::Checked);
    m_currentSelectIndex = item->index();
    Q_EMIT requesRetract();
}

void AvatarListWidget::setCurrentAvatarChecked(const QString &avatar)
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
        QString itemAvatar = m_avatarItemModel->index(i, 0).data(AvatarListWidget::SaveAvatarRole).value<QString>();
        if (currentAvatar != itemAvatar)
            continue;

        if (m_currentSelectIndex.isValid()) {
            m_avatarItemModel->setData(m_currentSelectIndex, Qt::Unchecked, Qt::CheckStateRole);
        }
        m_avatarItemModel->item(i)->setCheckState(Qt::Checked);
        m_currentSelectIndex = m_avatarItemModel->index(i, 0);
        Q_EMIT requesRetract();
        return;
    }

    refreshCustomAvatar(currentAvatar);
}

void AvatarListWidget::onItemClicked(const QModelIndex &index)
{
    if (index.data(Qt::CheckStateRole) == Qt::Checked)
        return;
    const QString filePath = index.data(SaveAvatarRole).toString();
    if (filePath.isEmpty()) {
        QString dir = m_dconfig->value("avatarPath").toString();
        if (dir.isEmpty() || !QDir(dir).exists()) {
            QStringList directory = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
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
        Q_EMIT requestSetAvatar(filePath);
    }
}

void AvatarListWidget::addItemFromDefaultDir()
{
    QString dirpath("/var/lib/AccountsService/icons/");
    QDir dir(dirpath);
    QStringList hideList;
    hideList << "default.png"
             << "guest.png";
    QStringList filters;
    filters << "*.png";          //设置过滤类型
    dir.setNameFilters(filters); //设置文件名的过滤
    QFileInfoList list = dir.entryInfoList();

    //根据文件名进行排序
    std::sort(list.begin(), list.end(), [&](const QFileInfo &fileinfo1, const QFileInfo &fileinfo2) {
        return fileinfo1.baseName() < fileinfo2.baseName();
    });

    for (int i = 0; i < MaxAvatarSize && i < list.size(); ++i) {
        if (hideList.contains(list.at(i).fileName())) {
            continue;
        }

        QString iconpath = list.at(i).filePath();

        DStandardItem *item = new DStandardItem();
        item->setAccessibleText(iconpath);
        auto ratio = devicePixelRatioF();

        auto pxPath = iconpath;
        if (ratio > 1.0) {
            pxPath.replace("icons/", "icons/bigger/");
        }
        auto px = QPixmap(pxPath).scaled(QSize(74, 74) * ratio,
                                         Qt::KeepAspectRatio, Qt::FastTransformation);
        px.setDevicePixelRatio(ratio);

        item->setData(QVariant::fromValue(px), Qt::DecorationRole);
        item->setData(QVariant::fromValue(iconpath), AvatarListWidget::SaveAvatarRole);
        item->setData(m_avatarSize, Qt::SizeHintRole);
        m_avatarItemModel->appendRow(item);
    }
}

void AvatarListWidget::addLastItem()
{
    DStandardItem *item = new DStandardItem();
    item->setAccessibleText("LastItem");
    item->setData(m_avatarSize, Qt::SizeHintRole);
    item->setData("", AvatarListWidget::SaveAvatarRole);
    item->setData(true, AvatarListWidget::AddAvatarRole);
    m_avatarItemModel->appendRow(item);
}

QString AvatarListWidget::getUserAddedCustomPicPath(const QString &usrName)
{
    if (usrName.isEmpty())
        return "";

    auto key = usrName + '-';
    QString newiconpath;
    QString dirpath("/var/lib/AccountsService/icons/local/");
    QDir dir(dirpath);
    QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot); //去除.和..
    for (auto fi : list) {
        auto str = fi.fileName();
        if (0 != str.indexOf(key))
            continue;

        if (str.right(str.size() - key.size()).indexOf('-') != -1)
            continue;

        newiconpath = fi.absoluteFilePath();
        break;
    }

    return newiconpath;
}

QStandardItem *AvatarListWidget::getAndSelectCustomAvatar()
{
    QStandardItem *item = m_avatarItemModel->item(1);
    // 默认项MaxAvatarSize个，添加项一个，自定义项一个
    if (m_avatarItemModel->rowCount() != MaxAvatarSize + 2) {
        item = new QStandardItem();
        if (m_currentSelectIndex.isValid())
            m_avatarItemModel->item(m_currentSelectIndex.row())->setCheckState(Qt::Unchecked);
        m_avatarItemModel->insertRow(1, item);
    }
    item->setCheckState(Qt::Checked);
    return item;
}

QString AvatarListWidget::getAvatarPath() const
{
    auto index = QRandomGenerator::global()->bounded(14);
    if (m_currentSelectIndex.isValid())
        index = m_currentSelectIndex.row();

    auto idx = m_avatarItemModel->index(index, 0);
    return m_avatarItemModel->data(idx, SaveAvatarRole).toString();
}

void AvatarListWidget::setAvatarSize(const QSize &size)
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

AvatarListDialog::AvatarListDialog(User *usr, QWidget *parent)
    : QDialog(parent)
    , m_avatarList(new AvatarListWidget(usr, this))
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_avatarList);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);
    QPushButton *cancleBtn = new QPushButton(tr("Cancel"));
    DSuggestButton *addBtn = new DSuggestButton(tr("Modify"));
    cancleBtn->setDefault(true);
    addBtn->setDefault(true);
    btnLayout->addWidget(cancleBtn);
    btnLayout->addWidget(addBtn);
    mainLayout->addSpacing(0);
    mainLayout->addLayout(btnLayout);

    connect(cancleBtn, &QPushButton::clicked, this, &AvatarListDialog::reject);
    connect(addBtn, &DSuggestButton::clicked, this, &AvatarListDialog::accept);
    resize(510, 365);
}

AvatarListDialog::~AvatarListDialog()
{
}

QString AvatarListDialog::getAvatarPath() const
{
    return m_avatarList->getAvatarPath();
}
