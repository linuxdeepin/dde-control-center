#include "defcategorywidget.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include "widgets/optionwidget.h"
#include "model/category.h"
#include "defappmodel.h"
#include "widgets/defcategoryaddwidget.h"
#include "settingshead.h"

using namespace dcc;
using namespace dcc::defapp;
using namespace dcc::widgets;

DefCategoryWidget::DefCategoryWidget(const QString &name, QWidget *parent)
    : TranslucentFrame(parent)
{
    m_centralLayout = new QVBoxLayout;
    m_userGroup = new SettingsGroup;
    m_addWidget  = new DefCategoryAddWidget(this);
    m_headWidget = new SettingsHead;
    m_addWidget->setAccessibleName(name);
    m_headWidget->setTitle(name);
    m_userGroup->insertItem(0, m_headWidget);
    m_userGroup->insertItem(1, m_addWidget);
    m_centralLayout->addWidget(m_userGroup);

    m_centralLayout->setMargin(0);

    setLayout(m_centralLayout);
    setObjectName("DefCategoryWidget");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(m_headWidget, &SettingsHead::editChanged, this, &DefCategoryWidget::slotEditMode);
    connect(m_addWidget, &DefCategoryAddWidget::requestFrameAutoHide, this, &DefCategoryWidget::requestFrameAutoHide);
    connect(m_addWidget, &DefCategoryAddWidget::requestCreateFile, this, &DefCategoryWidget::requestCreateFile);
}

void DefCategoryWidget::setCategory(Category *const category)
{
    m_category = category;
    connect(category, &Category::itemsChanged, this, &DefCategoryWidget::AppsItemChanged);
    connect(category, &Category::userItemChanged, this, &DefCategoryWidget::UserItemChanged);
    connect(category, &Category::defaultChanged, this, &DefCategoryWidget::onDefaultAppSet);
    connect(category, &Category::AdduserItem, this, &DefCategoryWidget::addUserItem);
    connect(category, &Category::categoryNameChanged, this, &DefCategoryWidget::setCategoryName);

    QList<QJsonObject> it = category->getappItem();
    for (const QJsonObject list : it) {
        addItem(list);
    }
    it = category->getuserItem();
    for (const QJsonObject list : it) {
        addUserItem(list);
    }
    onDefaultAppSet(category->getDefault());
    setCategoryName(category->getName());

    if(m_userMap.empty()) {
        m_headWidget->setEditEnable(false);
    }
}

void DefCategoryWidget::setDefault()
{
    OptionWidget *item = qobject_cast<OptionWidget *>(sender());
    emit requestSetDefaultApp(m_category->getName(), m_valueMap.value(item));
    onDefaultAppSet(m_valueMap.value(item));
}

void DefCategoryWidget::addItem(const QJsonObject &item)
{
    m_optionWidget = new OptionWidget;
    m_optionWidget->setItem(item);
    m_optionWidget->setMime(m_category->getName());
    m_optionWidget->setFixedHeight(36);
    m_userGroup->insertItem(1, m_optionWidget);
    m_mainMap.insert(m_optionWidget->id(), m_optionWidget);
    m_valueMap.insert(m_optionWidget, item);
    connect(m_optionWidget, &OptionWidget::setDefault, this, &DefCategoryWidget::setDefault);
    connect(m_optionWidget, &OptionWidget::removeItem, this, &DefCategoryWidget::removeItem);
}

void DefCategoryWidget::addUserItem(const QJsonObject &item)
{
    m_optionWidget = new OptionWidget(true);
    m_optionWidget->setItem(item);
    m_optionWidget->setMime(m_category->getName());
    m_optionWidget->setUserCheck(true);
    m_optionWidget->setFixedHeight(36);
    m_userGroup->insertItem(1, m_optionWidget);
    m_userMap.insert(m_optionWidget->id(), m_optionWidget);
    m_valueMap.insert(m_optionWidget, item);
    connect(m_optionWidget, &OptionWidget::setDefault, this, &DefCategoryWidget::setDefault);
    connect(m_optionWidget, &OptionWidget::removeItem, this, &DefCategoryWidget::removeItem);

    m_headWidget->setEditEnable(true);
}

void DefCategoryWidget::removeItem(const QJsonObject &item)
{
    OptionWidget *w = m_valueMap.key(item);
    m_userGroup->removeItem(w);
    w->deleteLater();
    m_userMap.remove(m_userMap.key(m_valueMap.key(item)));
    m_valueMap.remove(m_valueMap.key(item));
    emit requestDelUserApp(m_categoryName, item);

    if(m_userMap.empty()) {
        m_headWidget->setEditEnable(false);
    }
}

void DefCategoryWidget::onDefaultAppSet(const QJsonObject &json)
{
    const QString &id = json["Id"].toString();

    for (OptionWidget *item : m_valueMap.keys()) {
        item->setChecked(item->id() ==id);
    }
}

void DefCategoryWidget::slotEditMode(bool edit)
{
    if (m_userMap.size() != 0) {
        for (OptionWidget *item : m_userMap.values()) {
            item->setDelete(edit);
        }
    }
}

void DefCategoryWidget::setCategoryName(const QString &name)
{
    m_categoryName = name;
    m_addWidget->setCategory(name);
}

void DefCategoryWidget::AppsItemChanged(const QList<QJsonObject> &list)
{
    for (OptionWidget *item : m_mainMap.values()) {
        m_userGroup->removeItem(item);
        m_valueMap.remove(item);
        item->deleteLater();
    }
    for (const QJsonObject item : list) {
        addItem(item);
    }
    onDefaultAppSet(m_category->getDefault());
}


void DefCategoryWidget::UserItemChanged(const QList<QJsonObject> &list)
{
    for (OptionWidget *item : m_userMap.values()) {
        m_userMap.remove(m_userMap.key(item));
        m_valueMap.remove(item);
        m_userGroup->removeItem(item);
        item->deleteLater();
    }

    for (const QJsonObject item : list) {
        addUserItem(item);
    }
    onDefaultAppSet(m_category->getDefault());
}
