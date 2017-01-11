#include "shortcutwidget.h"
#include "shortcutitem.h"
#include "settingshead.h"
#include "translucentframe.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>

using namespace dcc;

namespace dcc {
namespace keyboard{

ShortcutWidget::ShortcutWidget(QWidget *parent)
    :ContentWidget(parent)
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
    m_custonGroup = new SettingsGroup();
    m_searchGroup = new SettingsGroup();
    m_search = new SearchInput();

    m_layout = new QVBoxLayout();
    m_layout->setMargin(0);
    m_layout->setSpacing(10);
    m_layout->addWidget(m_search);
    m_layout->addWidget(m_systemGroup);
    m_layout->addWidget(m_windowGroup);
    m_layout->addWidget(m_workspaceGroup);
    m_layout->addWidget(m_custonGroup);

    m_addCustom = new QPushButton(tr("Add Custom Shortcut"));

    m_layout->addWidget(m_addCustom);
    m_widget->setLayout(m_layout);

    setContent(m_widget);
    connect(m_addCustom, SIGNAL(clicked()), this, SIGNAL(customShortcut()));
    connect(m_search, SIGNAL(textChanged(QString)), this, SLOT(onSearch(QString)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    setTitle(tr("Shortcuts"));
}

void ShortcutWidget::addShortcut(QList<ShortcutInfo *> list, ShortcutWidget::InfoType type)
{
    QList<ShortcutInfo*>::iterator it = list.begin();
    for(; it != list.end(); ++it)
    {
        ShortcutItem* item = new ShortcutItem();
        connect(item, SIGNAL(shortcutChangd(bool, ShortcutInfo*, QString)), this, SIGNAL(shortcutChanged(bool, ShortcutInfo*, QString)));
        connect(item, &ShortcutItem::requestDisableShortcut, this, &ShortcutWidget::requestDisableShortcut);
        item->setShortcutInfo((*it));
        m_searchInfos[(*it)->name+(*it)->accels] = (*it);

        if(type == ShortcutWidget::System)
            m_systemGroup->appendItem(item);
        else if(type == ShortcutWidget::Window)
            m_windowGroup->appendItem(item);
        else if(type == ShortcutWidget::Workspace)
            m_workspaceGroup->appendItem(item);
        else if(type == ShortcutWidget::Custom)
        {
            if (m_custonGroup->layout()->count() == 0) {
                m_head = new SettingsHead();
                m_head->setEditEnable(true);
                m_head->setTitle(tr("Custom Shortcut"));
                m_custonGroup->insertItem(0, m_head);
            }
            connect(m_head, SIGNAL(editChanged(bool)), item, SLOT(onEditMode(bool)));
            m_custonGroup->appendItem(item);
            connect(item, SIGNAL(destroyed(QObject*)),this, SLOT(onDestroyItem(QObject*)));
        }
    }
}

void ShortcutWidget::modifyStatus(bool status)
{
    if(status)
    {
        m_addCustom->hide();
        m_custonGroup->hide();
        m_workspaceGroup->hide();
        m_windowGroup->hide();
        m_systemGroup->hide();
        m_searchGroup->show();
        m_layout->removeWidget(m_addCustom);
        m_layout->removeWidget(m_custonGroup);
        m_layout->removeWidget(m_workspaceGroup);
        m_layout->removeWidget(m_windowGroup);
        m_layout->removeWidget(m_systemGroup);
        m_layout->addWidget(m_searchGroup);
    }
    else
    {
        m_addCustom->show();
        m_custonGroup->show();
        m_workspaceGroup->show();
        m_windowGroup->show();
        m_systemGroup->show();
        m_searchGroup->hide();
        m_layout->addWidget(m_systemGroup);
        m_layout->addWidget(m_windowGroup);
        m_layout->addWidget(m_workspaceGroup);
        m_layout->addWidget(m_custonGroup);
        m_layout->addWidget(m_addCustom);
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
       connect(item, SIGNAL(shortcutChangd(bool, ShortcutInfo*, QString)), this, SIGNAL(shortcutChanged(bool, ShortcutInfo*, QString)));
       item->setShortcutInfo(info);
       if (m_custonGroup->layout()->count() == 0) {
           m_head = new SettingsHead();
           m_head->setEditEnable(true);
           m_head->setTitle(tr("Custom Shortcut"));
           m_custonGroup->insertItem(0, m_head);
       }
       connect(m_head, SIGNAL(editChanged(bool)), item, SLOT(onEditMode(bool)));
       m_custonGroup->appendItem(item);
       connect(item, SIGNAL(destroyed(QObject*)),this, SLOT(onDestroyItem(QObject*)));
   }
}

void ShortcutWidget::onDestroyItem(QObject *obj)
{
    Q_UNUSED(obj);

    ShortcutItem* item = qobject_cast<ShortcutItem*>(sender());
    if(item)
    {
        m_custonGroup->removeItem(item);
        if(m_custonGroup->itemCount() == 1)
        {
            m_custonGroup->removeItem(m_head);
            m_head->deleteLater();
            m_head = nullptr;
        }
        m_searchInfos.remove(item->curInfo()->name + item->curInfo()->accels);
        emit delShortcutInfo(item->curInfo());
        item->deleteLater();
    }
}

void ShortcutWidget::onSearchInfo(ShortcutInfo *info, const QString &key)
{
    if(m_searchInfos.keys().contains(key))
    {
        m_searchInfos.remove(key);
        m_searchInfos[info->name + info->accels] = info;
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
}
}
