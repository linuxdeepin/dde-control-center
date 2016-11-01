#include "defcategorywidget.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"


using namespace dcc;

DefCategoryWidget::DefCategoryWidget() {
    m_userGroup = new SettingsGroup;
    m_centeralLayout->addWidget(m_userGroup);
    m_centeralLayout->addStretch(0);

    setTitle(tr("Default Applications"));
}

void DefCategoryWidget::setCategory(Category * const category)
{
    if (!category) {
        return;
    }
    connect(category, &Category::itemsAdded,     this, &DefCategoryWidget::addItems);
    connect(category, &Category::itemsRemoved,   this, &DefCategoryWidget::removeItems);
    connect(category, &Category::defaultChanged, this, &DefCategoryWidget::onDefaultAppSet);
    const QList<QStringList> it = category->getList();
    const QString id = category->getDefault();
    for (const QStringList list : it) {
        addItem(list[0],list[1],list[2]);
    }
    onDefaultAppSet(id);
}

void DefCategoryWidget::addItems(const QStringList &list)
{
    for (int i = 0; i < list.count(); i++) {
        addItem(list[0],list[1],list[2]);
    }
}

void DefCategoryWidget::removeItems(const QStringList &list)
{
    for (int i = 0; i < list.count(); i++) {
        removeItem(list[1]);
    }
}

//添加列表
void DefCategoryWidget::addItem(const QString &name, const QString &id, const QString &mime) {
    m_optionWidget = new OptionWidget;
    m_optionWidget->setName(name);
    m_optionWidget->setDisplayName(name);
    m_optionWidget->setId(id);
    m_optionWidget->setMime(mime);
    m_optionWidget->setFixedHeight(30);
    //    m_optionWidget->setIcon();
    m_userGroup->appendItem(m_optionWidget);
    connect(m_optionWidget, &OptionWidget::set, this, &DefCategoryWidget::onSetRunner);
    connect(m_optionWidget, &OptionWidget::deleteClicked, this, &DefCategoryWidget::removeItem);
}


void DefCategoryWidget::removeItem(const QString &id) {
    if (!findObject(id)) {
        return;
    }
    m_userGroup->removeItem(findObject(id));
    findObject(id)->deleteLater();
}

OptionWidget *DefCategoryWidget::findObject(const QString &id)
{
    for (int cc = m_userGroup->layout()->count()-1; cc >= 0; --cc) {
        m_userGroupLayout = m_userGroup->layout()->itemAt(cc);
        m_optionWidget = qobject_cast<OptionWidget *>(m_userGroupLayout->widget());
        if (m_optionWidget != 0) {
            //doing something for m_optionWidget
            if(m_optionWidget->id() == id) {
                return m_optionWidget;
            }
        }
    }
    return nullptr;
}

void DefCategoryWidget::onSetRunner(const QString &mime, const QString &id) {
    for (int cc = m_userGroup->layout()->count()-1; cc >= 0; --cc) {
        m_userGroupLayout = m_userGroup->layout()->itemAt(cc);
        m_optionWidget = qobject_cast<OptionWidget *>(m_userGroupLayout->widget());
        if (m_optionWidget != 0) {
            //doing something for m_optionWidget
            if(m_optionWidget->id()==id) {
                m_optionWidget->setChecked(true);
                //set dbus
                emit setDefaultApp(id,mime);
            }
            m_optionWidget->setChecked(false);
        }
    }
}

void DefCategoryWidget::onDefaultAppSet(const QString &id) {
    for (int cc = m_userGroup->layout()->count()-1; cc >= 0; --cc) {
        m_userGroupLayout = m_userGroup->layout()->itemAt(cc);
        m_optionWidget = qobject_cast<OptionWidget *>(m_userGroupLayout->widget());
        if (m_optionWidget != 0) {
            //doing something for m_optionWidget
            if(m_optionWidget->id() == id) {
                m_optionWidget->setChecked(true);
                qDebug()<<"已设置 "<<id;
            } else {
                m_optionWidget->setChecked(false);
            }
        }
    }
}

void DefCategoryWidget::onEdit()
{
    for (int cc = m_userGroup->layout()->count()-1; cc >= 0; --cc) {
        m_userGroupLayout = m_userGroup->layout()->itemAt(cc);
        m_optionWidget = qobject_cast<OptionWidget *>(m_userGroupLayout->widget());
        if (m_optionWidget != 0) {
            m_optionWidget->setDelete();
        }
    }
}
