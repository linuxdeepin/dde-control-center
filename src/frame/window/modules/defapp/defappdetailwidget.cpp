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
#include "modules/defapp/model/category.h"
#include "modules/defapp/defappmodel.h"
#include "window/utils.h"

#include <DFloatingButton>
#include <DListView>
#include <DStyle>
#include <DApplicationHelper>

#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QStandardItemModel>
#include <QIcon>
#include <QFileDialog>
#include <QMimeDatabase>

using namespace DCC_NAMESPACE::defapp;

DWIDGET_USE_NAMESPACE

DefappDetailWidget::DefappDetailWidget(dcc::defapp::DefAppWorker::DefaultAppsCategory category, QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout)
    , m_defApps(new DListView(this))
    , m_model(new QStandardItemModel(this))
    , m_addBtn(new DFloatingButton(DStyle::SP_IncreaseElement, this))
    , m_categoryValue(category)
    , m_category(nullptr)
    , m_systemAppCnt(0)
    , m_userAppCnt(0)
{
    m_defApps->setAccessibleName("List_defapplist");
    m_defApps->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_defApps->setEditTriggers(QListView::NoEditTriggers);
    m_defApps->setIconSize(QSize(32, 32));
    m_defApps->setResizeMode(QListView::Adjust);
    m_defApps->setMovement(QListView::Static);
    m_defApps->setSelectionMode(QListView::NoSelection);
    m_defApps->setFrameShape(QFrame::NoFrame);
    m_defApps->setModel(m_model);
    m_defApps->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_defApps->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_defApps->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    m_defApps->setViewportMargins(ScrollAreaMargins);

    m_centralLayout->setMargin(0);
    m_centralLayout->addWidget(m_defApps, 1);
    if (!DGuiApplicationHelper::isTabletEnvironment()) {
        m_centralLayout->addWidget(m_addBtn, 0, Qt::AlignHCenter | Qt::AlignBottom);
        connect(m_addBtn, &Dtk::Widget::DFloatingButton::clicked, this, &DefappDetailWidget::onAddBtnClicked);
        //~ contents_path /defapp/Webpage
        m_addBtn->setToolTip(tr("Add Application"));
        m_addBtn->setAccessibleName(tr("Add Application"));
    } else {
        m_addBtn->setVisible(false);
    }

    setLayout(m_centralLayout);
}

void DefappDetailWidget::setModel(dcc::defapp::DefAppModel *const model)
{
    switch(m_categoryValue) {
    case dcc::defapp::DefAppWorker::Browser:
        setCategory(model->getModBrowser());
        break;
#ifndef USE_TABLET
    case dcc::defapp::DefAppWorker::Mail:
        setCategory(model->getModMail());
        break;
#endif
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
#ifndef USE_TABLET
    case dcc::defapp::DefAppWorker::Terminal:
        setCategory(model->getModTerminal());
        break;
#endif
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
    connect(m_category, &dcc::defapp::Category::clearAll, this, &DefappDetailWidget::onClearAll);

    AppsItemChanged(m_category->getappItem());

    onDefaultAppSet(m_category->getDefault());
    setCategoryName(m_category->getName());
}

QIcon DefappDetailWidget::getAppIcon(const QString &appIcon, const QSize &size) {
    QIcon icon = QIcon::fromTheme(appIcon, QIcon::fromTheme("application-x-desktop"));

    const qreal ratio = devicePixelRatioF();
    QPixmap pixmap = icon.pixmap(size * ratio).scaled(size * ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmap.setDevicePixelRatio(ratio);

    return pixmap;
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
        QString id = m_model->data(m_model->index(row, 0), DefAppIdRole).toString();
        if (id == item.Id) {
            m_model->removeRow(row);
            if (item.isUser) {
                m_userAppCnt--;
            } else {
                m_systemAppCnt--;
            }
            break;
        }
    }
    updateListView(m_category->getDefault());
}

void DefappDetailWidget::showInvalidText(DStandardItem *modelItem, const QString &name, const QString &iconName)
{
    if (name.isEmpty())
        return;

    DViewItemActionList actions;
    QPointer<DViewItemAction> act(new DViewItemAction(Qt::AlignVCenter | Qt::AlignLeft, QSize(32, 32), QSize(), false));
    QIcon icon = getAppIcon(iconName, QSize(32, 32));
    act->setIcon(icon);
    act->setTextColorRole(DPalette::TextWarning);
    act->setIconText(name);
    actions << act;
    modelItem->setActionList(Qt::LeftEdge, actions);
}

void DefappDetailWidget::setCategoryName(const QString &name)
{
    m_categoryName = name;
}

void DefappDetailWidget::updateListView(const dcc::defapp::App &defaultApp) {
    int cnt = m_model->rowCount();
    for (int row = 0; row < cnt; row++) {
        DStandardItem *modelItem = dynamic_cast<DStandardItem *>(m_model->item(row));
        QString id = modelItem->data(DefAppIdRole).toString();
        bool isUser = modelItem->data(DefAppIsUserRole).toBool();
        bool canDelete = modelItem->data(DefAppCanDeleteRole).toBool();
        QString name = modelItem->data(DefAppNameRole).toString();
        QString iconName = modelItem->data(DefAppIconRole).toString();

        if (id == defaultApp.Id) {
            modelItem->setCheckState(Qt::Checked);
            //remove user clear button
            if (!isUser && !canDelete)
                continue;

            DViewItemActionList actions;
            modelItem->setActionList(Qt::RightEdge, actions);
            showInvalidText(modelItem, name, iconName);
        } else {
            modelItem->setCheckState(Qt::Unchecked);
            //add user clear button
            if (!isUser && !canDelete)
                continue;

            DViewItemActionList btnActList;
            QPointer<DViewItemAction> delAction(new DViewItemAction(Qt::AlignVCenter | Qt::AlignRight, QSize(21, 21), QSize(19, 19), true));
            delAction->setVisible(!DGuiApplicationHelper::isTabletEnvironment());
            delAction->setIcon(DStyleHelper(style()).standardIcon(DStyle::SP_CloseButton, nullptr, this));
            connect(delAction, &QAction::triggered, this, &DefappDetailWidget::onDelBtnClicked);
            btnActList << delAction;
            modelItem->setActionList(Qt::RightEdge, btnActList);
            m_actionMap.insert(delAction, id);
            showInvalidText(modelItem, name, iconName);
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

    connect(m_defApps, &DListView::clicked, this, &DefappDetailWidget::onListViewClicked);
    connect(m_defApps, &DListView::activated, m_defApps, &QListView::clicked);
}

void DefappDetailWidget::onListViewClicked(const QModelIndex& index) {
    if (!index.isValid()) return;
    QString id = m_defApps->model()->data(m_defApps->currentIndex(), DefAppIdRole).toString();
    dcc::defapp::App app = getAppById(id);
    if (!isValid(app))
        return;
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
        QFileDialog dialog(this);
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

void  DefappDetailWidget::onDelBtnClicked() {
    DViewItemAction *action = qobject_cast<DViewItemAction *>(sender());
    if (!m_actionMap.contains(action))
        return;

    QString id = m_actionMap[action];

    dcc::defapp::App app = getAppById(id);
    if (!isValid(app) || !(app.isUser || app.CanDelete))
        return;
    qDebug() << "delete app " << app.Id;
    //delete user app
    Q_EMIT requestDelUserApp(m_categoryName, app);
}

void DefappDetailWidget::onClearAll()
{
    int cnt = m_model->rowCount();
    m_model->removeRows(0, cnt);
    m_systemAppCnt = 0;
    m_userAppCnt = 0;
}

dcc::defapp::App DefappDetailWidget::getAppById(const QString &appId)
{
    auto res = std::find_if(m_category->getappItem().cbegin(), m_category->getappItem().cend(), [ = ] (const dcc::defapp::App &item)->bool{
        return item.Id == appId;
    });

    if (res != m_category->getappItem().cend()) {
        return *res;
    }
    dcc::defapp::App app;
    app.Id = nullptr;
    return app;
}

void DefappDetailWidget::appendItemData(const dcc::defapp::App &app)
{
    qDebug() << "appendItemData=" << app.MimeTypeFit;
    DStandardItem *item = new DStandardItem;
    QString appName = (!app.isUser || app.MimeTypeFit)
            ? app.Name : QString("%1(%2)").arg(app.Name).arg(tr("Invalid"));

    if (!app.isUser || app.MimeTypeFit) {
        item->setText(appName);
        item->setIcon(getAppIcon(app.Icon, QSize(32, 32)));
    } else {
        item->setData(appName, DefAppNameRole);
        item->setData(app.Icon, DefAppIconRole);
    }
    item->setData(app.Id, DefAppIdRole);
    item->setData(app.isUser, DefAppIsUserRole);
    item->setData(app.CanDelete, DefAppCanDeleteRole);
    item->setData(VListViewItemMargin, Dtk::MarginsRole);

    int index = 0;
    if (app.isUser) {
        index = m_systemAppCnt + m_userAppCnt;
        m_userAppCnt++;
    } else {
        index = m_systemAppCnt;
        m_systemAppCnt++;
    }
    m_model->insertRow(index, item);
}

bool DefappDetailWidget::isDesktopOrBinaryFile(const QString &fileName)
{
    QMimeDatabase mimeDatabase;
    if (mimeDatabase.suffixForFileName(fileName) == "desktop") {
        return true;
    }

    QMimeType mimeType(mimeDatabase.mimeTypeForFile(fileName, QMimeDatabase::MatchExtension));
    return mimeType.name().startsWith("application/octet-stream");
}

bool DefappDetailWidget::isValid(const dcc::defapp::App &app)
{
    return (!app.Id.isNull() && !app.Id.isEmpty());
}
