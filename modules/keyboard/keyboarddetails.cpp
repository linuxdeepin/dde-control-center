#include "keyboarddetails.h"
#include "settingsgroup.h"
#include "settingshead.h"
#include "checkitem.h"
#include "translucentframe.h"
#include <QVBoxLayout>
#include <QDebug>

using namespace dcc;

namespace dcc {
namespace keyboard{
KeyboardDetails::KeyboardDetails(QWidget *parent)
    :ContentWidget(parent),
      m_bEdit(false)
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

    layout->addSpacing(10);
    layout->addWidget(m_group);

    QPushButton* addBtn = new QPushButton(tr("Add Keyboard Layout"));
    layout->addWidget(addBtn);

    content->setLayout(layout);

    setContent(content);

    connect(addBtn, SIGNAL(clicked()), this, SIGNAL(layoutAdded()));
    connect(m_head, SIGNAL(editChanged(bool)), this ,SLOT(onEdit(bool)));
}

void KeyboardDetails::setModel(KeyboardModel *model)
{
    m_model = model;

#ifndef DCC_DISABLE_KBLAYOUT
    connect(model, &KeyboardModel::userLayoutChanged, this, &KeyboardDetails::onAddKeyboard);
    connect(model, &KeyboardModel::curLayoutChanged, this, &KeyboardDetails::onDefault);
#endif

    QMap<QString, QString> map = model->userLayout();

    for (auto i(map.begin()); i != map.end(); ++i) {
        onAddKeyboard(i.key(), i.value());
    }
}

void KeyboardDetails::onAddKeyboard(const QString &id, const QString &value)
{
    if(m_maps.contains(id))
        return;

    CheckItem* checkItem = new CheckItem();
    connect(m_head, SIGNAL(editChanged(bool)), checkItem, SLOT(onEditMode(bool)));
    connect(checkItem, &CheckItem::checkedChanged, this, &KeyboardDetails::requestCurLayoutAdded);
    connect(checkItem, SIGNAL(destroySelf(CheckItem*)), this, SLOT(onRemoveLayout(CheckItem*)));

    checkItem->setTitle(value);
    checkItem->onEditMode(m_bEdit);

    m_group->appendItem(checkItem);
    m_maps[id] = checkItem;

    m_head->setEditEnable(m_maps.size() > 1);

    onDefault(m_model->curLayout());
}

void KeyboardDetails::onEdit(bool value)
{
    m_bEdit = value;
}

void KeyboardDetails::onRemoveLayout(CheckItem *item)
{
    if(item)
    {
        m_group->removeItem(item);
        emit delUserLayout(item->title());
        m_maps.remove(m_model->userLayout().key(item->title()));
        item->deleteLater();
    }

    if (m_maps.size() < 2)
    {
        m_head->setEditEnable(false);
    }
}

void KeyboardDetails::onDefault(const QString &value)
{
    for (auto i(m_maps.begin()); i != m_maps.end(); ++i) {
       CheckItem *item = i.value();
       item->setChecked(item->title() == value);
    }
}

}
}
