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

#include "widgets/lineeditwidget.h"

#include <dpasswordedit.h>

#include <QVBoxLayout>

namespace dcc {
namespace widgets {

ErrorTip::ErrorTip(QWidget *parent) :
    DArrowRectangle(DArrowRectangle::ArrowTop, parent),
    m_label(new QLabel)
{
    m_label->setObjectName("New_Account_errorTip");
    m_label->setAccessibleName(m_label->objectName());
    m_label->setStyleSheet("padding: 5px 10px; color: #f9704f");
    m_label->setWordWrap(true);
    setContent(m_label);
}

void ErrorTip::setText(QString text)
{
    m_label->setText(text);
    m_label->setAccessibleDescription(text);
    m_label->adjustSize();
    resizeWithContent();
}

void ErrorTip::clear()
{
    m_label->clear();
    hide();
}

bool ErrorTip::isEmpty() const
{
    return m_label->text().isEmpty();
}

void ErrorTip::appearIfNotEmpty()
{
    if (!isEmpty() && !isVisible())
        QWidget::show();
}

LineEditWidget::LineEditWidget(QFrame *parent)
    : SettingsItem(parent)
    , m_title(new QLabel)
    , m_edit(new DLineEdit)
    , m_errTip(new ErrorTip(this))
{
    m_title->setFixedWidth(110);
    m_edit->setContextMenuPolicy(Qt::NoContextMenu);

    m_mainLayout = new QHBoxLayout;
    m_mainLayout->addWidget(m_title, 0, Qt::AlignVCenter);
    m_mainLayout->addWidget(m_edit, 0, Qt::AlignVCenter);

    setLayout(m_mainLayout);
    setObjectName("LineEditWidget");

    connect(m_edit, &DLineEdit::textChanged, this, &LineEditWidget::hideAlertMessage);
}

LineEditWidget::LineEditWidget(bool isPasswordMode, QWidget *parent)
    : SettingsItem(parent)
    , m_title(new QLabel)
    , m_errTip(new ErrorTip(this))
{
    if (isPasswordMode) {
        m_edit = new DPasswordEdit;
    } else {
        m_edit = new DLineEdit;
    }
    m_title->setFixedWidth(110);
    m_edit->setContextMenuPolicy(Qt::NoContextMenu);

    m_mainLayout = new QHBoxLayout;
    m_mainLayout->addWidget(m_title, 0, Qt::AlignVCenter);
    m_mainLayout->addWidget(m_edit, 0, Qt::AlignVCenter);

    setLayout(m_mainLayout);
    setObjectName("LineEditWidget");

    connect(m_edit, &DLineEdit::textChanged, this, &LineEditWidget::hideAlertMessage);
}

void LineEditWidget::addRightWidget(QWidget *widget)
{
    m_mainLayout->addWidget(widget);
}

void LineEditWidget::setReadOnly(const bool state)
{
    m_edit->lineEdit()->setReadOnly(state);
}

void LineEditWidget::setIsErr(const bool err)
{
    dTextEdit()->setAlert(err);
}

void LineEditWidget::showAlertMessage(const QString &message)
{
    if (message.isEmpty()) return;

    const QPoint &p = m_edit->mapToGlobal(m_edit->rect().bottomLeft());
    m_errTip->setText(message);
    m_errTip->show(p.x(), p.y());
}

void LineEditWidget::hideAlertMessage()
{
    setIsErr(false);
    m_errTip->hide();
}

void LineEditWidget::setTitle(const QString &title)
{
    m_title->setText(title);
    m_title->setWordWrap(true);

    setAccessibleName(title);
    m_edit->setAccessibleName(title);
}

void LineEditWidget::setText(const QString &text)
{
    m_edit->setText(text);
}

void LineEditWidget::setPlaceholderText(const QString &text)
{
    m_edit->lineEdit()->setPlaceholderText(text);
}

void LineEditWidget::mousePressEvent(QMouseEvent *e)
{
    SettingsItem::mousePressEvent(e);

    if (e->button() != Qt::LeftButton) {
        return;
    }

    m_edit->setFocus();
}

}
}
