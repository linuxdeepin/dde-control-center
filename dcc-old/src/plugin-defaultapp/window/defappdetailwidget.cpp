// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "defappdetailwidget.h"

#include "defappmodel.h"
#include "defappworker.h"
#include "widgets/dcclistview.h"

#include <interface/moduleobject.h>

#include <DFloatingButton>
#include <DIconTheme>
#include <DListView>
#include <DStyle>

#include <QDebug>
#include <QIcon>
#include <QLoggingCategory>
#include <QMimeDatabase>
#include <QPointer>
#include <QStandardItemModel>
#include <QVBoxLayout>

Q_LOGGING_CATEGORY(DdcDefaultDetailWidget, "dcc-default-detailwidget")

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

DefappDetailWidget::DefappDetailWidget(DefAppWorker::DefaultAppsCategory category, QWidget *parent)
    : DCC_NAMESPACE::DCCListView(parent)
    , m_model(new QStandardItemModel(this))
    , m_categoryValue(category)
    , m_category(nullptr)
    , m_systemAppCnt(0)
    , m_userAppCnt(0)
{
    setAccessibleName("List_defapplist");
    setEditTriggers(QListView::NoEditTriggers);
    setIconSize(QSize(32, 32));
    setMovement(QListView::Static);
    setSelectionMode(QListView::NoSelection);
    setFrameShape(QFrame::NoFrame);
    setModel(m_model);
    setViewportMargins(0, 0, 10, 0);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

DefappDetailWidget::~DefappDetailWidget() { }

void DefappDetailWidget::setDetailModel(DefAppModel *const model)
{
    switch (m_categoryValue) {
    case DefAppWorker::Browser:
        setCategory(model->getModBrowser());
        break;
    case DefAppWorker::Mail:
        setCategory(model->getModMail());
        break;
    case DefAppWorker::Text:
        setCategory(model->getModText());
        break;
    case DefAppWorker::Music:
        setCategory(model->getModMusic());
        break;
    case DefAppWorker::Video:
        setCategory(model->getModVideo());
        break;
    case DefAppWorker::Picture:
        setCategory(model->getModPicture());
        break;
    case DefAppWorker::Terminal:
        setCategory(model->getModTerminal());
        break;
    default:
        break;
    }
}

void DefappDetailWidget::setCategory(Category *const category)
{
    m_category = category;

    connect(m_category, &Category::defaultChanged, this, &DefappDetailWidget::onDefaultAppSet);
    connect(m_category, &Category::addedUserItem, this, &DefappDetailWidget::addItem);
    connect(m_category, &Category::removedUserItem, this, &DefappDetailWidget::removeItem);
    connect(m_category, &Category::categoryNameChanged, this, &DefappDetailWidget::setCategoryName);
    connect(m_category, &Category::clearAll, this, &DefappDetailWidget::onClearAll);

    AppsItemChanged(m_category->getappItem());

    onDefaultAppSet(m_category->getDefault());
    setCategoryName(m_category->getName());
}

QIcon DefappDetailWidget::getAppIcon(const QString &appIcon, const QSize &size)
{
    QIcon icon(appIcon);
    if (icon.pixmap(size).isNull())
        icon = DIconTheme::findQIcon(appIcon, DIconTheme::findQIcon("application-x-desktop"));

    const qreal ratio = devicePixelRatioF();
    QPixmap pixmap = icon.pixmap(size * ratio)
                             .scaled(size * ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmap.setDevicePixelRatio(ratio);

    return pixmap;
}

void DefappDetailWidget::addItem(const App &item)
{
    qCDebug(DdcDefaultDetailWidget) << item.Id << ", isUser :" << item.isUser;
    appendItemData(item);
    updateListView(m_category->getDefault());
}

void DefappDetailWidget::removeItem(const App &item)
{
    qCDebug(DdcDefaultDetailWidget) << "DefappDetailWidget::removeItem id " << item.Id;
    // update model
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

void DefappDetailWidget::showInvalidText(DStandardItem *modelItem,
                                         const QString &name,
                                         const QString &iconName)
{
    if (name.isEmpty())
        return;

    DViewItemActionList actions;
    QPointer<DViewItemAction> act(
            new DViewItemAction(Qt::AlignVCenter | Qt::AlignLeft, QSize(32, 32), QSize(), false));
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

void DefappDetailWidget::updateListView(const App &defaultApp)
{
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
            // remove user clear button
            if (!isUser && !canDelete)
                continue;

            DViewItemActionList actions;
            modelItem->setActionList(Qt::RightEdge, actions);
            showInvalidText(modelItem, name, iconName);
        } else {
            modelItem->setCheckState(Qt::Unchecked);
            // add user clear button
            if (!isUser && !canDelete)
                continue;

            DViewItemActionList btnActList;
            QPointer<DViewItemAction> delAction(
                    new DViewItemAction(Qt::AlignVCenter | Qt::AlignRight,
                                        QSize(21, 21),
                                        QSize(19, 19),
                                        true));

            delAction->setIcon(
                    DStyleHelper(style()).standardIcon(DStyle::SP_CloseButton, nullptr, this));
            connect(delAction, &QAction::triggered, this, &DefappDetailWidget::onDelBtnClicked);
            btnActList << delAction;
            modelItem->setActionList(Qt::RightEdge, btnActList);
            m_actionMap.insert(delAction, id);
            showInvalidText(modelItem, name, iconName);
        }
    }
}

void DefappDetailWidget::onDefaultAppSet(const App &app)
{
    qCDebug(DdcDefaultDetailWidget) << "SetAppName" << app.Name << this;
    updateListView(app);
}

void DefappDetailWidget::AppsItemChanged(const QList<App> &list)
{
    for (const App &app : list) {
        appendItemData(app);
    }

    connect(this, &DListView::clicked, this, &DefappDetailWidget::onListViewClicked);
    connect(this, &DListView::activated, this, &QListView::clicked);
}

void DefappDetailWidget::onListViewClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString id = this->model()->data(this->currentIndex(), DefAppIdRole).toString();
    App app = getAppById(id);
    if (!isValid(app))
        return;

    qCDebug(DdcDefaultDetailWidget) << "set default app " << app.Name;
    updateListView(app);
    // set default app
    Q_EMIT requestSetDefaultApp(m_categoryName, app);
}

void DefappDetailWidget::onDelBtnClicked()
{
    DViewItemAction *action = qobject_cast<DViewItemAction *>(sender());
    if (!m_actionMap.contains(action))
        return;

    QString id = m_actionMap[action];

    App app = getAppById(id);
    if (!isValid(app) || !(app.isUser || app.CanDelete))
        return;

    qCDebug(DdcDefaultDetailWidget) << "delete app " << app.Id;
    // delete user app
    Q_EMIT requestDelUserApp(m_categoryName, app);
}

void DefappDetailWidget::onClearAll()
{
    int cnt = m_model->rowCount();
    m_model->removeRows(0, cnt);
    m_systemAppCnt = 0;
    m_userAppCnt = 0;
}

App DefappDetailWidget::getAppById(const QString &appId)
{
    auto res = std::find_if(m_category->getappItem().cbegin(),
                            m_category->getappItem().cend(),
                            [=](const App &item) -> bool {
                                return item.Id == appId;
                            });

    if (res != m_category->getappItem().cend()) {
        return *res;
    }

    App app;
    app.Id = nullptr;
    return app;
}

void DefappDetailWidget::appendItemData(const App &app)
{
    qCDebug(DdcDefaultDetailWidget) << "appendItemData=" << app.MimeTypeFit;
    DStandardItem *item = new DStandardItem;
    QString appName = app.DisplayName;
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

bool DefappDetailWidget::isValid(const App &app)
{
    return (!app.Id.isNull() && !app.Id.isEmpty());
}
