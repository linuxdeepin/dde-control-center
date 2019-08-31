/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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

#include "checkitem.h"
#include "modules/keyboard/keylabel.h"

#include <DIconButton>
#include <DStyle>

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPushButton>
#include <QLabel>

using namespace dcc;
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;

CheckItem::CheckItem(QFrame *parent)
    : SettingsItem(parent)
    , m_checked(false)
    , m_bEdit(false)
    , m_circle(0)
    , m_multiple(false)
{
    m_hLayout = new QHBoxLayout();
    m_hLayout->setContentsMargins(20, 0, 10, 0);
    m_title = new QLabel();
    m_checkBtn = new DIconButton(this);
    m_checkBtn->setFlat(true);

    m_hLayout->setSpacing(1);
    m_hLayout->addWidget(m_title);
    m_hLayout->setAlignment(m_title, Qt::AlignVCenter);
    m_hLayout->addStretch();
    m_hLayout->addWidget(m_checkBtn);
    m_hLayout->addSpacerItem(new QSpacerItem(10, 36));
    m_hLayout->setAlignment(m_checkBtn, Qt::AlignVCenter);
    setLayout(m_hLayout);
    setFixedHeight(36);

    connect(m_checkBtn, &DIconButton::clicked, this, &CheckItem::onClick);
}

void CheckItem::setKeyLabelEffect(const QStringList &shortCutKeys)
{
    for (KeyLabel *label : m_list) {
        m_hLayout->removeWidget(label);
        label->deleteLater();
    }

    m_list.clear();
    m_hLayout->removeWidget(m_title);
    for (const QString &key : shortCutKeys) {
        KeyLabel *label = new KeyLabel(key);
        m_list << label;
        m_hLayout->insertWidget(0, label);
    }

    adjustSize();
}

void CheckItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

QString CheckItem::title() const
{
    return m_title->text();
}

void CheckItem::setChecked(bool checked)
{
    if (m_checked == checked) {
        return;
    }

    m_checked = checked;
    if (m_checked) {
        m_checkBtn->setIcon(DStyle::StandardPixmap::SP_IndicatorChecked);
        m_checkBtn->show();
    } else {
        m_checkBtn->hide();
    }
}

void CheckItem::setMultipleMode(bool multiple)
{
    m_multiple = multiple;
}

bool CheckItem::checked() const
{
    return m_checked;
}

void CheckItem::onEditMode(bool value)
{
    m_bEdit = value;
    if (m_checked) {
        return;
    }

    if (m_bEdit) {
        m_checkBtn->setIcon(DStyle::StandardPixmap::SP_DeleteButton);
        m_checkBtn->show();
    } else {
        m_checkBtn->hide();
    }
}

void CheckItem::onClick()
{
    if (!m_checked) {
        Q_EMIT destroySelf(this);
    }

    if (m_multiple) {
        setChecked(!m_checked);
        Q_EMIT checkedChanged(m_title->text());
        return;
    }
}

void CheckItem::mousePressEvent(QMouseEvent *)
{
    if (m_bEdit) {
        return;
    }
    m_circle++;
}

void CheckItem::mouseReleaseEvent(QMouseEvent *)
{
    if (m_bEdit) {
        return;
    }

    if (m_multiple) {
        setChecked(!m_checked);
        Q_EMIT checkedChanged(m_title->text());
        return;
    }

    if (m_circle != 1) {
        m_circle = 0;
        return;
    }

    setChecked(true);
    m_circle = 0;

    Q_EMIT checkedChanged(m_title->text());
}
