/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "widgets/comboboxwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/optionitem.h"
#include "widgets/lineeditwidget.h"

#include <QVBoxLayout>
#include <QDebug>

using namespace dcc::widgets;

ComboBoxWidget::ComboBoxWidget(QFrame *parent)
    : NextPageWidget(parent),

      m_custom(new LineEditWidget),
      m_optionsGroup(new SettingsGroup),
      m_contentPage(nullptr),
      m_lastSelectedItem(nullptr)
{
    connect(this, &NextPageWidget::clicked, this, &ComboBoxWidget::onNextPageClicked);
    connect(m_custom->textEdit(), &QLineEdit::editingFinished, [=] {
        const QString txt = m_custom->textEdit()->text();
        NextPageWidget::setValue(txt);
        Q_EMIT dataChanged(txt);
    });
}

void ComboBoxWidget::appendOption(const QString &name, const QVariant &value)
{
    OptionItem *item = new OptionItem;
    item->setContentsMargins(20, 0, 10, 0);
    item->setTitle(name);

    connect(item, &OptionItem::selectedChanged, this, &ComboBoxWidget::onItemClicked);

    m_optionsGroup->appendItem(item);
    m_options[item] = value;
}

void ComboBoxWidget::removeOption(const QVariant &value)
{
    OptionItem *item = m_options.key(value, nullptr);
    if (!item) {
        return;
    }
    m_options.remove(item);
    m_optionsGroup->removeItem(item);
    if (m_lastSelectedItem == item) {
        m_lastSelectedItem = m_options.firstKey();
        m_lastSelectedItem->blockSignals(true);
        m_lastSelectedItem->setSelected(true);
        m_lastSelectedItem->blockSignals(false);
        NextPageWidget::setValue(m_lastSelectedItem->title());
        Q_EMIT dataChanged(m_options.first());
    }
    item->deleteLater();
}

void ComboBoxWidget::setCurrent(const QVariant &value)
{
    OptionItem *item = m_options.key(value);
    if (!item)
        return NextPageWidget::setValue(QString());

    item->blockSignals(true);
    item->setSelected(true);
    item->blockSignals(false);
    NextPageWidget::setValue(item->title());

    m_lastSelectedItem = item;
}

void ComboBoxWidget::setEditable(const bool editable)
{
    if (editable)
    {
        m_custom->setTitleVisible(false);
        m_optionsGroup->appendItem(m_custom);
    }
}

void ComboBoxWidget::onNextPageClicked()
{
    Q_ASSERT(m_contentPage.isNull());

    m_contentPage = new ContentWidget;

    TranslucentFrame *mainWidget = new TranslucentFrame;
    QVBoxLayout *layout = new QVBoxLayout(mainWidget);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addSpacing(10);
    layout->addWidget(m_optionsGroup);

    m_contentPage->setContent(mainWidget);
    m_optionsGroup->setVisible(true);

    connect(m_contentPage, &ContentWidget::destroyed, this, &ComboBoxWidget::onContentDesktory);

    Q_EMIT requestPage(m_contentPage.data());
}

void ComboBoxWidget::onContentDesktory()
{
    m_optionsGroup->setVisible(false);
    m_optionsGroup->setParent(this);
}

void ComboBoxWidget::onItemClicked()
{
    OptionItem *item = static_cast<OptionItem *>(sender());
    Q_ASSERT(item);

    if (item == m_lastSelectedItem)
    {
        Q_EMIT m_contentPage->back();
        return;
    }

    NextPageWidget::setValue(item->title());

    if (m_options.contains(item))
        Q_EMIT dataChanged(m_options.value(item));
    Q_EMIT m_contentPage->back();

    if (m_lastSelectedItem)
    {
        m_lastSelectedItem->blockSignals(true);
        m_lastSelectedItem->setSelected(false);
        m_lastSelectedItem->blockSignals(false);
    }

    if (!item->selected())
    {
        item->blockSignals(true);
        item->setSelected(true);
        item->blockSignals(false);
    }

    m_lastSelectedItem = item;
}
