#include "theme.h"
#include "settingsheaderitem.h"
#include "labels/largelabel.h"
#include "../../widget/themeitempic.h"
#include "../../model/thememodel.h"
#include "optionitem.h"
#include "personalization/personalizationmodel.h"
#include "../../widget/themeitem.h"

using namespace dcc;
using namespace dcc::personalization;
using namespace dcc::widgets;

Theme::Theme(const QString &title, QWidget *parent)
    : TranslucentFrame(parent)
{
    m_mainLayout = new QVBoxLayout;
    m_mainGroup = new SettingsGroup(title);
    m_mainLayout->addWidget(m_mainGroup);

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    setLayout(m_mainLayout);
    setAccessibleName(title);
}

void Theme::setModel(ThemeModel *const model)
{
    m_model = model;
    connect(m_model, &ThemeModel::defaultChanged, this, &Theme::setDefault);
    connect(m_model, &ThemeModel::itemAdded,    this, &Theme::onAddItem);
    connect(m_model, &ThemeModel::picAdded, this, &Theme::onSetPic);
    connect(m_model, &ThemeModel::itemRemoved, this, &Theme::onRemoveItem);

    QMap<QString, QJsonObject> itemList = m_model->getList();

    for (auto it(itemList.begin()); it != itemList.end(); ++it) {
        onAddItem(it.value());
    }

    setDefault(m_model->getDefault());

    QMap<QString, QString> picList = m_model->getPicList();

    for (auto it(picList.begin()); it != picList.end(); ++it) {
        onSetPic(it.key(), it.value());
    }
}

void Theme::onAddItem(const QJsonObject &json)
{
    if (m_valueMap.values().contains(json))
        return;
    ThemeItem *theme = new ThemeItem(json);
    m_mainGroup->appendItem(theme);
    m_valueMap.insert(theme, json);
    connect(theme, &ThemeItem::selectedChanged, this, &Theme::onItemClicked);
}

void Theme::setDefault(const QString &name)
{
    for (ThemeItem *item : m_valueMap.keys()) {
        item->setSelected(item->id() == name);
    }
}

void Theme::onItemClicked(const bool selected)
{
    if (selected) {
        ThemeItem *item = qobject_cast<ThemeItem *>(sender());
        Q_ASSERT(m_valueMap.contains(item));
        emit requestSetDefault(m_valueMap[item]);
    }
}

void Theme::onSetPic(const QString &id, const QString &picPath)
{
    for (ThemeItem *item : m_valueMap.keys()) {
        if (item->id() == id) {
            item->setPic(picPath);
            return;
        }
    }
}

void Theme::onRemoveItem(const QString &id)
{
    for (int i = 0; i != m_valueMap.count(); i++) {
        ThemeItem *item = m_valueMap.keys().at(i);
        if (item->id() == id) {
            m_mainGroup->removeItem(item);
            m_valueMap.remove(item);
            delete item;
            return;
        }
    }
}
