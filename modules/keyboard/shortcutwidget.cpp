#include "shortcutwidget.h"
#include "shortcutitem.h"
#include "settingshead.h"
#include "shortcutmodel.h"
#include "translucentframe.h"
#include "settingsheaderitem.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>

using namespace dcc;

namespace dcc {
namespace keyboard{

ShortcutWidget::ShortcutWidget(ShortcutModel *model, QWidget *parent)
    :ContentWidget(parent),
      m_model(model)
{
    m_searchInter = new SearchInter("com.deepin.daemon.Search",
                                    "/com/deepin/daemon/Search",
                                    QDBusConnection::sessionBus(), this);
    m_timer = new QTimer(this);
    m_timer->setInterval(150);

    m_widget = new TranslucentFrame();
    m_searchText = QString();

    m_systemGroup = new SettingsGroup(tr("System"));
    m_windowGroup = new SettingsGroup(tr("Window"));
    m_workspaceGroup = new SettingsGroup(tr("WorkSpace"));
    m_customGroup = new SettingsGroup();
    m_searchGroup = new SettingsGroup();
    m_search = new SearchInput();

    m_head = new SettingsHead();
    m_head->setEditEnable(true);
    m_head->setVisible(false);
    m_head->setTitle(tr("Custom Shortcut"));
    m_customGroup->insertItem(0, m_head);

    m_layout = new QVBoxLayout();
    m_layout->setMargin(0);
    m_layout->setSpacing(10);
    m_layout->addSpacing(10);
    m_layout->addWidget(m_search);
    m_layout->addWidget(m_systemGroup);
    m_layout->addWidget(m_windowGroup);
    m_layout->addWidget(m_workspaceGroup);
    m_layout->addWidget(m_customGroup);

    m_addCustom = new QPushButton(tr("Add Custom Shortcut"));

    m_layout->addWidget(m_addCustom);
    m_layout->addSpacing(20);
    m_widget->setLayout(m_layout);

    setContent(m_widget);
    connect(m_addCustom, SIGNAL(clicked()), this, SIGNAL(customShortcut()));
    connect(m_search, SIGNAL(textChanged(QString)), this, SLOT(onSearch(QString)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    setTitle(tr("Shortcuts"));

    connect(m_model, SIGNAL(addCustomInfo(ShortcutInfo*)), this, SLOT(onCustomAdded(ShortcutInfo*)));
    connect(m_model, &ShortcutModel::listChanged, this, &ShortcutWidget::addShortcut);

    addShortcut(m_model->systemInfo(), ShortcutModel::System);
    addShortcut(m_model->windowInfo(), ShortcutModel::Window);
    addShortcut(m_model->workspaceInfo(), ShortcutModel::Workspace);
    addShortcut(m_model->customInfo(), ShortcutModel::Custom);
}

void ShortcutWidget::addShortcut(QList<ShortcutInfo *> list, ShortcutModel::InfoType type)
{

    if(type == ShortcutModel::System) {
        for (ShortcutItem *item : m_systemList) {
            if (item) {
                m_systemGroup->removeItem(item);
                item->deleteLater();
            }
        }
        m_systemList.clear();
    }
    else if(type == ShortcutModel::Window) {
        for (ShortcutItem *item : m_windowList) {
            if (item) {
                m_windowGroup->removeItem(item);
                item->deleteLater();
            }
        }
        m_windowList.clear();
    }
    else if(type == ShortcutModel::Workspace) {
        for (ShortcutItem *item : m_workspaceList) {
            if (item) {
                m_workspaceGroup->removeItem(item);
                item->deleteLater();
            }
        }
        m_workspaceList.clear();
    }
    else if(type == ShortcutModel::Custom)
    {
        for (ShortcutItem *item : m_customList) {
            if (item) {
                m_customGroup->removeItem(item);
                item->deleteLater();
            }
        }
        m_customList.clear();
    }



    QList<ShortcutInfo*>::iterator it = list.begin();
    for(; it != list.end(); ++it)
    {
        ShortcutItem* item = new ShortcutItem();
        connect(item, SIGNAL(shortcutChangd(bool, ShortcutInfo*, QString)), this, SIGNAL(shortcutChanged(bool, ShortcutInfo*, QString)));
        connect(item, &ShortcutItem::requestDisableShortcut, this, &ShortcutWidget::requestDisableShortcut);
        item->setShortcutInfo((*it));
        item->setTitle((*it)->name);
        (*it)->item = item;
        m_searchInfos[(*it)->toString()] = (*it);

        if(type == ShortcutModel::System) {
            m_systemGroup->appendItem(item);
            m_systemList.append(item);
        }
        else if(type == ShortcutModel::Window) {
            m_windowGroup->appendItem(item);
            m_windowList.append(item);
        }
        else if(type == ShortcutModel::Workspace) {
            m_workspaceGroup->appendItem(item);
            m_workspaceList.append(item);
        }
        else if(type == ShortcutModel::Custom)
        {
           connect(m_head, SIGNAL(editChanged(bool)), item, SLOT(onEditMode(bool)));
            m_customGroup->appendItem(item);
            m_customList.append(item);

            if(m_customGroup->itemCount() > 1)
                m_head->setVisible(true);

            connect(item, SIGNAL(destroyed(QObject*)),this, SLOT(onDestroyItem(QObject*)));
            connect(item, &ShortcutItem::shortcutEditChanged, this, &ShortcutWidget::shortcutEditChanged);
        }
    }
}

SettingsHead *ShortcutWidget::getHead()
{
    return m_head;
}

void ShortcutWidget::modifyStatus(bool status)
{
    if(status)
    {
        m_addCustom->hide();
        m_customGroup->hide();
        m_workspaceGroup->hide();
        m_windowGroup->hide();
        m_systemGroup->hide();
        m_searchGroup->show();
        m_layout->removeWidget(m_addCustom);
        m_layout->removeWidget(m_customGroup);
        m_layout->removeWidget(m_workspaceGroup);
        m_layout->removeWidget(m_windowGroup);
        m_layout->removeWidget(m_systemGroup);
        m_layout->insertWidget(2, m_searchGroup);
    }
    else
    {
        m_addCustom->show();
        m_customGroup->show();
        m_workspaceGroup->show();
        m_windowGroup->show();
        m_systemGroup->show();
        m_searchGroup->hide();
        m_layout->insertWidget(2, m_addCustom);
        m_layout->insertWidget(2, m_customGroup);
        m_layout->insertWidget(2, m_workspaceGroup);
        m_layout->insertWidget(2, m_windowGroup);
        m_layout->insertWidget(2, m_systemGroup);
        m_layout->removeWidget(m_searchGroup);
    }
}

void ShortcutWidget::onSearch(const QString &text)
{
    if(m_searchText.length() == 0 || text.length() == 0)
        modifyStatus(text.length() > 0);
    m_searchText = text;

    if(text.length() > 0)
    {
        onTimeout();
    }
}

void ShortcutWidget::onCustomAdded(ShortcutInfo *info)
{
   if(info)
   {
       ShortcutItem* item = new ShortcutItem();
       connect(item, &ShortcutItem::requestDisableShortcut, this, &ShortcutWidget::requestDisableShortcut);
       connect(item, SIGNAL(shortcutChangd(bool, ShortcutInfo*, QString)), this, SIGNAL(shortcutChanged(bool, ShortcutInfo*, QString)));
       item->setShortcutInfo(info);
       item->setTitle(info->name);
       info->item = item;

       m_searchInfos[info->toString()] = info;

       m_head->setVisible(true);
       connect(m_head, SIGNAL(editChanged(bool)), item, SLOT(onEditMode(bool)));
       m_customGroup->appendItem(item);
       m_customList.append(item);

       connect(item, SIGNAL(destroyed(QObject*)),this, SLOT(onDestroyItem(QObject*)));
       connect(item, &ShortcutItem::shortcutEditChanged, this, &ShortcutWidget::shortcutEditChanged);
   }
}

void ShortcutWidget::onDestroyItem(QObject *obj)
{
    Q_UNUSED(obj);

    m_head->toCancel();

    ShortcutItem* item = qobject_cast<ShortcutItem*>(sender());
    if(item)
    {
        m_customGroup->removeItem(item);
        if(m_customGroup->itemCount() == 1)
            m_head->setVisible(false);

        m_searchInfos.remove(item->curInfo()->toString());
        m_customList.removeOne(item);
        emit delShortcutInfo(item->curInfo());
        item->deleteLater();
    }
}

void ShortcutWidget::onSearchInfo(ShortcutInfo *info, const QString &key)
{
    if(m_searchInfos.keys().contains(key))
    {
        m_searchInfos.remove(key);
        m_searchInfos[info->toString()] = info;
    }
}

void ShortcutWidget::getKeyFinish(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;
    if(reply.isError())
    {
        qDebug()<<reply.error().message();
        watch->deleteLater();
        return;
    }

    QString key = reply.value();
    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_searchInter->SearchString(m_searchText, key), this);
    connect(result, SIGNAL(finished(QDBusPendingCallWatcher*)), this,
            SLOT(onSearchFinish(QDBusPendingCallWatcher*)));

    watch->deleteLater();
}

void ShortcutWidget::onSearchFinish(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QStringList> reply = *watch;
    if(reply.isError())
    {
        qDebug()<<reply.error().message();
        watch->deleteLater();
        return;
    }

    QLayoutItem *child;
    while ((child = m_searchGroup->layout()->takeAt(0)) != 0) {
      ShortcutItem* item =  qobject_cast<ShortcutItem*>(child->widget());
      if(item)
          item->deleteLater();
      delete child;
    }
    QStringList list = reply.value();
    for(int i = 0; i<list.count(); i++)
    {
        ShortcutInfo* info = m_searchInfos[list.at(i)];
        ShortcutItem* item = new ShortcutItem();
        connect(item, SIGNAL(shortcutChangd(bool, ShortcutInfo*, QString)), this, SIGNAL(shortcutChanged(bool, ShortcutInfo*, QString)));
        item->setShortcutInfo((info));
        item->setTitle(info->name);
        m_searchGroup->appendItem(item);
    }

    watch->deleteLater();
}

void ShortcutWidget::onTimeout()
{
    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_searchInter->NewSearchWithStrList(m_searchInfos.keys()), this);
    connect(result, SIGNAL(finished(QDBusPendingCallWatcher*)), this,
            SLOT(getKeyFinish(QDBusPendingCallWatcher*)));
}

void ShortcutWidget::onRemoveItem(const QString &id, int type)
{
    Q_UNUSED(type)

    for (auto item(m_customList.begin()); item != m_customList.end(); ++item) {
        ShortcutItem *it = *item;
        Q_ASSERT(it);
        if (it->curInfo()->id == id) {
            m_customGroup->removeItem(it);
            m_customList.removeOne(it);
            emit delShortcutInfo(it->curInfo());
            it->deleteLater();
            return;
        }
    }
}

}
}
