//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/lineeditwidget.h"
#include "widgets/accessibleinterface.h"

#include <DPasswordEdit>
#include <DLineEdit>

#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

SET_FORM_ACCESSIBLE(ErrorTip,"ErrorTip");
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

    m_title->setAccessibleName("title");
    m_title->setFixedWidth(110);
    m_edit->setContextMenuPolicy(Qt::NoContextMenu);
    m_edit->setAccessibleName("LineEditWidget");

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
        m_edit->setCopyEnabled(false);
        m_edit->setCutEnabled(false);
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

QLineEdit *LineEditWidget::textEdit() const
{
    return m_edit->lineEdit();
}

QString LineEditWidget::text() const
{
    return m_edit->text();
}

void LineEditWidget::setTitleVisible(const bool visible)
{
    m_title->setVisible(visible);
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
