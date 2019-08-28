/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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

#include "defappdetailwidget.h"
#include "defapplistview.h"
#include "delappdelegate.h"
#include "modules/defapp/model/category.h"
#include "modules/defapp/defappmodel.h"

#include <DStyleOption>
#include <DFloatingButton>

#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QStandardItemModel>
#include <QPushButton>
#include <QIcon>
#include <QFileDialog>

using namespace DCC_NAMESPACE::defapp;

DWIDGET_USE_NAMESPACE

Q_DECLARE_METATYPE(QMargins)

DefappDetailWidget::DefappDetailWidget(dcc::defapp::DefAppWorker::DefaultAppsCategory category, QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout)
    , m_defApps(new DefAppListView(this))
    , m_model(new QStandardItemModel(this))
    , m_addBtn(new DFloatingButton(DStyle::SP_IncreaseElement, this))
    , m_categoryValue(category)
    , m_category(nullptr)
    , m_delItemDelegate(new DelAppDelegate(this))
{
    const QMap<dcc::defapp::DefAppWorker::DefaultAppsCategory, QString> names {
        {dcc::defapp::DefAppWorker::Browser, tr("Webpage")},
        {dcc::defapp::DefAppWorker::Mail, tr("Mail")},
        {dcc::defapp::DefAppWorker::Text, tr("Text")},
        {dcc::defapp::DefAppWorker::Music, tr("Music")},
        {dcc::defapp::DefAppWorker::Video, tr("Video")},
        {dcc::defapp::DefAppWorker::Picture, tr("Picture")},
        {dcc::defapp::DefAppWorker::Terminal, tr("Terminal")}
    };

    m_centralLayout->addWidget(new QLabel(names[category], this));

    m_defApps->setEditTriggers(QListView::NoEditTriggers);
    m_defApps->setIconSize(QSize(32, 32));
    m_defApps->setResizeMode(QListView::Adjust);
    m_defApps->setMovement(QListView::Static);
    m_defApps->setSelectionMode(QListView::SingleSelection);
    m_defApps->setFrameShape(QFrame::NoFrame);
    m_defApps->setContentsMargins(10, 10, 10, 10);
    m_defApps->setMinimumWidth(360);
    m_defApps->setModel(m_model);
    m_defApps->setItemDelegate(m_delItemDelegate);

    m_addBtn->setMinimumSize(47, 47);

    m_centralLayout->addWidget(m_defApps);
    m_centralLayout->addStretch();
    m_centralLayout->addWidget(m_addBtn, 0, Qt::AlignHCenter);
    setLayout(m_centralLayout);

    connect(m_addBtn, &Dtk::Widget::DFloatingButton::clicked, this, &DefappDetailWidget::onAddBtnClicked);
    connect(m_delItemDelegate, &DelAppDelegate::removeBtnClicked, this, &DefappDetailWidget::onDelBtnClicked);
}

void DefappDetailWidget::setModel(dcc::defapp::DefAppModel *const model)
{
    switch(m_categoryValue) {
    case dcc::defapp::DefAppWorker::Browser:
        setCategory(model->getModBrowser());
        break;
    case dcc::defapp::DefAppWorker::Mail:
        setCategory(model->getModMail());
        break;
    case dcc::defapp::DefAppWorker::Text:
        setCategory(model->getModText());
        break;
    case dcc::defapp::DefAppWorker::Music:
        setCategory(model->getModMusic());
        break;
    case dcc::defapp::DefAppWorker::Video:
        setCategory(model->getModVideo());
        break;
    case dcc::defapp::DefAppWorker::Picture:
        setCategory(model->getModPicture());
        break;
    case dcc::defapp::DefAppWorker::Terminal:
        setCategory(model->getModTerminal());
        break;
    default:
        break;
    }
}

void DefappDetailWidget::setCategory(dcc::defapp::Category *const category)
{
    m_category = category;

    connect(m_category, &dcc::defapp::Category::defaultChanged, this, &DefappDetailWidget::onDefaultAppSet);
    connect(m_category, &dcc::defapp::Category::addedUserItem, this, &DefappDetailWidget::addItem);
    connect(m_category, &dcc::defapp::Category::removedUserItem, this, &DefappDetailWidget::removeItem);
    connect(m_category, &dcc::defapp::Category::categoryNameChanged, this, &DefappDetailWidget::setCategoryName);

    AppsItemChanged(m_category->getappItem());

    onDefaultAppSet(m_category->getDefault());
    setCategoryName(m_category->getName());
}

QIcon DefappDetailWidget::getAppIcon(const dcc::defapp::App &app) {
    return QIcon::fromTheme(app.Icon, QIcon::fromTheme("application-x-desktop"));
}

void DefappDetailWidget::addItem(const dcc::defapp::App &item)
{
    qDebug() << Q_FUNC_INFO << item.Id << ", isUser :" << item.isUser;
    appendItemData(item);
    updateListView(m_category->getDefault());
}

void DefappDetailWidget::removeItem(const dcc::defapp::App &item)
{
    qDebug() << "DefappDetailWidget::removeItem id " << item.Id;
    //update model
    int cnt = m_model->rowCount();
    for (int row = 0; row < cnt; row++) {
        QString id = m_model->data(m_model->index(row, 0), DefAppListView::DefAppIdRole).toString();
        if (id == item.Id) {
            m_model->removeRow(row);
            break;
        }
    }
    updateListView(m_category->getDefault());
}

void DefappDetailWidget::setCategoryName(const QString &name)
{
    m_categoryName = name;
}

void DefappDetailWidget::updateListView(const dcc::defapp::App &app) {
    int cnt = m_model->rowCount();
    for (int row = 0; row < cnt; row++) {
        QString id = m_model->data(m_model->index(row, 0), DefAppListView::DefAppIdRole).toString();
        QStandardItem *modelItem = m_model->item(row);
        if (id == app.Id) {
            modelItem->setCheckState(Qt::Checked);
        } else {
            modelItem->setCheckState(Qt::Unchecked);
        }
    }
}

void DefappDetailWidget::onDefaultAppSet(const dcc::defapp::App &app)
{
    qDebug() << Q_FUNC_INFO << app.Name;

    updateListView(app);
}


void DefappDetailWidget::AppsItemChanged(const QList<dcc::defapp::App> &list)
{
    for (const dcc::defapp::App& app : list) {
        appendItemData(app);
    }

    connect(m_defApps, &DefAppListView::clicked, this, &DefappDetailWidget::onListViewClicked);
}

void DefappDetailWidget::onListViewClicked(const QModelIndex& index) {
    if (!index.isValid()) return;
    QString id = m_defApps->model()->data(m_defApps->currentIndex(), DefAppListView::DefAppIdRole).toString();
    dcc::defapp::App app = getAppById(id);
    qDebug()  <<  "set default app "  << app.Name;
    updateListView(app);
    //set default app
    Q_EMIT requestSetDefaultApp(m_categoryName, app);
}

void DefappDetailWidget::onAddBtnClicked() {
    do {
        if (!isEnabled())
            break;

        Q_EMIT requestFrameAutoHide(false);
        QFileDialog dialog;
        dialog.setWindowTitle(tr("Open Desktop file"));

        QStringList directory = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
        if (!directory.isEmpty())
            dialog.setDirectory(directory.first());

        if (dialog.exec() != QDialog::Accepted)
            break;

        QString path = dialog.selectedFiles()[0];

        if (path.isEmpty())
            break;

        QFileInfo info(path);
        Q_EMIT requestCreateFile(m_categoryName, info);
    } while(false);

    Q_EMIT requestFrameAutoHide(true);
}

void  DefappDetailWidget::onDelBtnClicked(const QModelIndex &index) {
    QString id = m_model->data(index, DefAppListView::DefAppIdRole).toString();

    dcc::defapp::App app = getAppById(id);
    if (!app.isUser)  return;
    qDebug() << "delete app " << app.Id;
    //delete user app
    Q_EMIT requestDelUserApp(m_categoryName, app);
}

dcc::defapp::App DefappDetailWidget::getAppById(const QString &appId) {
    for (dcc::defapp::App item : m_category->getappItem()) {
        if (item.Id == appId) {
            return item;
        }
    }

    Q_UNREACHABLE();
}

void DefappDetailWidget::appendItemData(const dcc::defapp::App &app)
{
    QString appName = (!app.isUser || isDesktopOrBinaryFile(app.Exec))
            ? app.Name : QString("%1(%2)").arg(app.Name).arg(tr("Invalid"));
    DStandardItem *item = new DStandardItem(getAppIcon(app), appName);

    item->setData(app.isUser, DefAppListView::DefAppIsUserRole);
    item->setData(app.Id, DefAppListView::DefAppIdRole);
    item->setSizeHint(QSize(340, 48));
    item->setData(QVariant::fromValue(QMargins(10, 10, 10, 0)), Dtk::MarginsRole);
    m_model->appendRow(item);
}

bool DefappDetailWidget::isDesktopOrBinaryFile(const QString &fileName)
{
    if (QFileInfo(fileName).suffix() == "desktop")
        return true;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return false;

    long text_char_count = 0;
    unsigned char byte;

    while (file.read((char *)(&byte), 1) > 0) {
        if (byte == 9 || byte == 10 || byte == 13 || (byte >= 32)) {
            text_char_count++;
        } else if ((byte <= 6) || (byte >= 14 && byte <= 31)) {
            file.close();
            return true;
        }
    }

    file.close();
    return (text_char_count >= 1) ? false : true;
}
