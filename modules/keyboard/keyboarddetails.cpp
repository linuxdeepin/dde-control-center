#include "keyboarddetails.h"
#include "settingsgroup.h"
#include "settingshead.h"
#include "checkitem.h"
#include "translucentframe.h"
#include <QVBoxLayout>

using namespace dcc;

namespace dcc {
namespace keyboard{
KeyboardDetails::KeyboardDetails(QWidget *parent)
    :ContentWidget(parent),
      m_bEdit(false),
      m_default(nullptr)
{
    TranslucentFrame* content = new TranslucentFrame();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(20);
    layout->setMargin(0);

    m_group = new SettingsGroup();
    m_head = new SettingsHead();
    m_head->setTitle(tr("Keyboard Layout"));
    m_head->setEditEnable(false);
    setTitle(tr("Keyboard Layout"));

    m_group->appendItem(m_head);

    layout->addWidget(m_group);

    QPushButton* addBtn = new QPushButton(tr("Add Keyboard Layout"));
    layout->addWidget(addBtn);

    content->setLayout(layout);

    setContent(content);

    connect(addBtn, SIGNAL(clicked()), this, SIGNAL(layoutAdded()));
    connect(m_head, SIGNAL(editChanged(bool)), this ,SLOT(onEdit(bool)));
}

void KeyboardDetails::setDefaultString(const MetaData &layout)
{
    if(m_default)
    {
        m_default->setTitle(layout.text());
        m_default->setChecked(true);
    }
}

void KeyboardDetails::onAddKeyboard(const MetaData &value)
{
    if(m_maps.contains(value.key()))
        return;

    CheckItem* checkItem = new CheckItem();
    connect(m_head, SIGNAL(editChanged(bool)), checkItem, SLOT(onEditMode(bool)));
    connect(checkItem, SIGNAL(checkedChanged(bool)), this, SLOT(setCurrentLayout(bool)));
    connect(checkItem, SIGNAL(destroySelf(CheckItem*)), this, SLOT(onRemoveLayout(CheckItem*)));

    checkItem->setTitle(value.text());
    checkItem->onEditMode(m_bEdit);

    m_group->appendItem(checkItem);
    m_maps[value.key()] = value.text();
    if(value.selected())
    {
        m_default = checkItem;
        m_default->setChecked(true);
    }

    m_head->setEditEnable(m_maps.size() > 1);
}

void KeyboardDetails::onEdit(bool value)
{
    m_bEdit = value;
}

void KeyboardDetails::setCurrentLayout(bool value)
{
    CheckItem* item = qobject_cast<CheckItem*>(this->sender());
    if(m_default == item)
        return;

    if(value && item)
    {
        m_default->setChecked(false);
        item->setChecked(true);
        m_default = item;

        emit curLayout(m_maps.key(item->title()));
    }
}

void KeyboardDetails::onRemoveLayout(CheckItem *item)
{
    if(item)
    {
        m_group->removeItem(item);
        emit delUserLayout(m_maps.key(item->title()));
        m_maps.remove(m_maps.key(item->title()));
    }

    if (m_maps.size() < 2)
    {
        m_head->setEditEnable(false);
    }
}

}
}
