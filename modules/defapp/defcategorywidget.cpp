#include "defcategorywidget.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"


using namespace dcc;

DefCategoryWidget::DefCategoryWidget(const QString &name)
{

    m_userGroup = new SettingsGroup;
    m_headWidget = new DefCategoryHeadWidget(name);
    m_addWidget  = new DefCategoryAddWidget;

    m_userGroup->insertItem(0, m_headWidget);
    m_userGroup->insertItem(1, m_addWidget);
    m_centeralLayout->addWidget(m_userGroup);

    connect(m_addWidget,  &DefCategoryAddWidget::addUserItem,  this, &DefCategoryWidget::addUserApp);
    connect(m_headWidget, &DefCategoryHeadWidget::editChanged, this, &DefCategoryWidget::slotEditMode);

    connect(m_addWidget, &DefCategoryAddWidget::requestFrameAutoHide, this, &DefCategoryWidget::requestFrameAutoHide);
}

void DefCategoryWidget::setCategory(Category *const category)
{
    m_category = category;
    connect(category, &Category::itemsChanged, this, &DefCategoryWidget::AppsItemChanged);
    connect(category, &Category::userItemChanged, this, &DefCategoryWidget::UserItemChanged);
    connect(category, &Category::defaultChanged, this, &DefCategoryWidget::onDefaultAppSet);

    QList<QJsonObject> it = category->getappItem();
    for (const QJsonObject list : it) {
        addItem(list);
    }
    it = category->getuserItem();
    for (const QJsonObject list : it) {
        addUserItem(list);
    }

    onDefaultAppSet(category->getDefault());
    m_categoryName = category->getName();
    m_addWidget->setCategory(m_categoryName);
}


//添加列表
void DefCategoryWidget::addItem(const QJsonObject &item)
{
    m_optionWidget = new OptionWidget;
    m_optionWidget->setItem(item);
    m_optionWidget->setMime(m_category->getName());
    m_optionWidget->setFixedHeight(30);
    m_userGroup->insertItem(1, m_optionWidget);
    connect(m_optionWidget, &OptionWidget::setDefault, this, &DefCategoryWidget::onDefaultAppSet);
    connect(m_optionWidget, &OptionWidget::removeItem, this, &DefCategoryWidget::removeItem);
}

void DefCategoryWidget::addUserItem(const QJsonObject &item)
{
    m_optionWidget = new OptionWidget;
    m_optionWidget->setItem(item);
    m_optionWidget->setMime(m_category->getName());
    m_optionWidget->setUserCheck(true);
    m_optionWidget->setFixedHeight(30);
    m_userGroup->insertItem(1, m_optionWidget);
    connect(m_optionWidget, &OptionWidget::setDefault, this, &DefCategoryWidget::onDefaultAppSet);
    connect(m_optionWidget, &OptionWidget::removeItem, this, &DefCategoryWidget::removeItem);
}

void DefCategoryWidget::removeItem(const QJsonObject &item)
{
    OptionWidget *widget = getObject(item["Id"].toString());
    if (!widget) {
        return;
    }
    m_userGroup->removeItem(widget);
    widget->deleteLater();
    emit delUserApp(widget->getItem());
    if (itemCount() == 2) {
        m_headWidget->setedit(true);
    }
}

OptionWidget *DefCategoryWidget::getObject(const QString &id)
{
    for (int cc = m_userGroup->layout()->count() - 1; cc >= 0; --cc) {
        m_userGroupLayout = m_userGroup->layout()->itemAt(cc);
        m_optionWidget = qobject_cast<OptionWidget *>(m_userGroupLayout->widget());
        if (m_optionWidget != nullptr) {
            //doing something for m_optionWidget
            if (m_optionWidget->id() == id) {
                return m_optionWidget;
            }
        }
    }
    return nullptr;
}

int DefCategoryWidget::itemCount()
{
    return m_userGroup->layout()->count();
}

void DefCategoryWidget::onDefaultAppSet(const QString &id)
{
    qDebug() << "id " << id;
    for (int cc = m_userGroup->layout()->count() - 1; cc >= 0; --cc) {
        m_userGroupLayout = m_userGroup->layout()->itemAt(cc);
        m_optionWidget = qobject_cast<OptionWidget *>(m_userGroupLayout->widget());
        if (m_optionWidget != 0) {
            //doing something for m_optionWidget
            if (m_optionWidget->id() == id) {
                m_optionWidget->setChecked(true);
                emit setDefaultApp(m_category->getName(), m_optionWidget->getItem());
                qDebug() << "已设置 " << id;
            } else {
                m_optionWidget->setChecked(false);
            }
        }
    }
}

void DefCategoryWidget::slotEditMode(const bool editable)
{
    //点击删除以后，禁用添加按钮
    m_addWidget->setEnabled(!editable);
    for (int cc = m_userGroup->layout()->count() - 1; cc >= 0; --cc) {
        m_userGroupLayout = m_userGroup->layout()->itemAt(cc);
        m_optionWidget = qobject_cast<OptionWidget *>(m_userGroupLayout->widget());
        if (m_optionWidget != 0) {
            m_optionWidget->setDelete(editable);
        }
    }
}

void DefCategoryWidget::AppsItemChanged()
{
    //销毁所有对象，再重新添加。测试中
    for (int cc = m_userGroup->layout()->count() - 1; cc >= 0; --cc) {
        if(cc == 0 || cc == m_userGroup->layout()->count() - 1) {
            break;
        }
        m_userGroupLayout = m_userGroup->layout()->itemAt(cc);
        m_optionWidget = qobject_cast<OptionWidget *>(m_userGroupLayout->widget());
        if (m_optionWidget != 0) {
            if (!m_optionWidget->userCheck()) {
                m_userGroup->removeItem(m_optionWidget);
                m_optionWidget->deleteLater();
                qDebug() << "销毁" << m_optionWidget->id();
            }
        }
    }

    QList<QJsonObject> it = m_category->getappItem();
    for (const QJsonObject list : it) {
        addItem(list);
    }

    onDefaultAppSet(m_category->getDefault());
}

void DefCategoryWidget::UserItemChanged()
{
    //销毁所有对象，再重新添加。测试中
    for (int cc = m_userGroup->layout()->count() - 1; cc >= 0; --cc) {
        if(cc == 0 || cc == m_userGroup->layout()->count() - 1) {
            break;
        }
        m_userGroupLayout = m_userGroup->layout()->itemAt(cc);
        m_optionWidget = qobject_cast<OptionWidget *>(m_userGroupLayout->widget());
        if (m_optionWidget != 0) {
            if (m_optionWidget->userCheck()) {
                m_userGroup->removeItem(m_optionWidget);
                m_optionWidget->deleteLater();
                qDebug() << "销毁用户添加的" << m_optionWidget->id();
            }
        }
    }
    QList<QJsonObject> it = m_category->getuserItem();
    for (const QJsonObject list : it) {
        addUserItem(list);
    }
    onDefaultAppSet(m_category->getDefault());
}
