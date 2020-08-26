/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "accountfingeitem.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

using namespace dcc;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

AccounntFingeItem::AccounntFingeItem(QWidget *parent)
    : SettingsItem(parent)
    , m_layout(new QHBoxLayout)
    , m_title(new QLabel)
    , m_removeBtn(new DIconButton(this))
    , m_editBtn(new DIconButton(this))
    , m_editTitle(new DLineEdit(this))
    , m_fingerName("")
{
    setFixedHeight(36);

    m_editBtn->setIcon(QIcon::fromTheme("dcc_edit"));
    m_editBtn->setFlat(true);//设置背景透明
    m_editBtn->setVisible(false);

    m_editTitle->setClearButtonEnabled(false);
    m_editTitle->setVisible(false);
    m_editTitle->lineEdit()->setFrame(false);

    m_removeBtn->setFlat(true);
    m_removeBtn->setIcon(DStyle::StandardPixmap::SP_DeleteButton);
    m_removeBtn->setFixedSize(QSize(24, 24));
    m_removeBtn->setIconSize(QSize(16, 16));
    m_removeBtn->setVisible(false);
    DStyle::setFocusRectVisible(m_removeBtn, false);

    m_layout->setContentsMargins(20, 5, 10, 5);
    m_layout->addWidget(m_title, 0, Qt::AlignLeft);
    m_layout->addWidget(m_editBtn, 0, Qt::AlignLeft);
    m_layout->addWidget(m_editTitle, 0 , Qt::AlignLeft);
    m_layout->addStretch();
    m_layout->addWidget(m_removeBtn, 0, Qt::AlignVCenter);
    setLayout(m_layout);

    connect(m_removeBtn, &DIconButton::clicked, this, &AccounntFingeItem::removeClicked);
    connect(m_editBtn, &DIconButton::clicked, this, [this] {
        Q_EMIT editClicked(m_editTitle->isVisible());
        if (m_editTitle->isVisible()) {
            m_editTitle->lineEdit()->setText(m_title->text());
            m_editTitle->lineEdit()->selectAll();
            m_editTitle->lineEdit()->setFocus();
        }
    });
    connect(m_editTitle->lineEdit(), &QLineEdit::textChanged, this, [this] {
        m_editTitle->setAlert(false);
        m_editTitle->hideAlertMessage();
    });
    connect(m_editTitle->lineEdit(), &QLineEdit::editingFinished, this, [this] {
        if (onNameEditFinished(m_editTitle)) {
            Q_EMIT editTextFinished(m_editTitle->text());
        }
        m_editTitle->lineEdit()->clearFocus();
        setEditTitle(false);
    });
}

void AccounntFingeItem::setTitle(const QString &title)
{
    if (title.isEmpty()) {
        m_layout->removeWidget(m_title);
    } else {
        m_title->setText(title);
    }
    m_fingerName = title;
}

void AccounntFingeItem::alertTitleRepeat()
{
    m_editTitle->setAlert(true);
    m_editTitle->showAlertMessage(tr("The name already exists"), parentWidget()->parentWidget());
    m_editTitle->lineEdit()->selectAll();
}

void AccounntFingeItem::appendItem(QWidget *widget)
{
    m_layout->addWidget(widget, 0, Qt::AlignLeft);
}

void AccounntFingeItem::setShowIcon(bool state)
{
    m_removeBtn->setVisible(state);
    m_editBtn->setVisible(state);
}

void AccounntFingeItem::setEditTitle(bool state)
{
    m_title->setVisible(!state);
    m_editBtn->setVisible(!state);
    m_editTitle->setVisible(state);
}

void AccounntFingeItem::setHideTitle(bool state)
{
    m_title->setVisible(state);
    m_editBtn->setVisible(state);
    m_editTitle->setVisible(state);
}

bool AccounntFingeItem::validateName(const QString &password)
{
    QString validate_policy = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") +
                              QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ") + QString("_");
    for (const QChar &p : password) {
        if (!validate_policy.contains(p)) {
            ushort uNum = p.unicode();
            if (uNum >= 0x4E00 && uNum <= 0x9FA5) {
                continue; // 这个字符是中文
            } else {
                return false;
            }
        }
    }
    return true;
}

bool AccounntFingeItem::onNameEditFinished(DLineEdit *edit)
{
    QString editName = edit->lineEdit()->text();
    if (editName.isEmpty())
        return false;
    //正则表达式判断是否由字母、数字、中文、下划线组成
    bool regResult = editName.contains(QRegExp("(^[\\w\u4e00-\u9fa5]+$)"));
    if (editName.size() > 15) {
        edit->setAlert(true);
        if (!regResult) {
            edit->showAlertMessage(tr("Use letters, numbers and underscores only, and no more than 15 characters"), parentWidget()->parentWidget(), 2000);
        } else {
            edit->showAlertMessage(tr("No more than 15 characters"), parentWidget()->parentWidget(), 2000);
        }
        edit->lineEdit()->selectAll();
        return false;
    } else {
        if (!regResult) {
            edit->setAlert(true);
            edit->showAlertMessage(tr("Use letters, numbers and underscores only"), parentWidget()->parentWidget(), 2000);
            edit->lineEdit()->selectAll();
            return false;
        }
    }
    return true;
}
