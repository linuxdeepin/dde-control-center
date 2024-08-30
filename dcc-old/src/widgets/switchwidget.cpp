//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/switchwidget.h"
#include "widgets/utils.h"
#include "widgets/accessibleinterface.h"

#include <DSwitchButton>

#include <QHBoxLayout>
#include <QMouseEvent>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
SET_LABEL_ACCESSIBLE(SwitchLabel,"SwitchLabel");
SwitchLabel::SwitchLabel(QWidget *parent, Qt::WindowFlags f)
    : QLabel (parent,f)
{
    setAccessibleName("SwitchLabel");
}

void SwitchLabel::resizeEvent(QResizeEvent *event)
{
    if(m_sourceText.isEmpty())
        m_sourceText = this->text();

    m_actualSize = event->size();
    QFontMetrics fontMetrics(this->font());

    QString str = m_sourceText;
    int len = fontMetrics.horizontalAdvance(m_sourceText);
    if(len > m_actualSize.width()) {
        str = fontMetrics.elidedText(str, Qt::ElideRight, m_actualSize.width());
        this->setText(str);
        this->setToolTip(m_sourceText);
    }
    else {
        this->setText(m_sourceText);
        this->setToolTip("");
    }

    QLabel::resizeEvent(event);
}

SET_FORM_ACCESSIBLE(SwitchWidget,"SwitchWidget");
SwitchWidget::SwitchWidget(const QString &title, QWidget *parent)
    : SettingsItem(parent)
    , m_leftWidget(new SwitchLabel)
    , m_switchBtn(new DSwitchButton)
{
    m_switchBtn->setAccessibleName(title);
    qobject_cast<SwitchLabel*>(m_leftWidget)->setText(title);
    init();
}

SwitchWidget::SwitchWidget(QWidget *parent, QWidget *widget)
    : SettingsItem(parent)
    , m_leftWidget(widget)
    , m_switchBtn(new DSwitchButton)
{
    if (!m_leftWidget)
        m_leftWidget = new SwitchLabel();

    init();
}

void SwitchWidget::init()
{
    setFixedHeight(SwitchWidgetHeight);
    QHBoxLayout *lableLayout = new QHBoxLayout;
    lableLayout->addWidget(m_leftWidget);
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(10, 0, 10, 0);

    m_mainLayout->addLayout(lableLayout, 0);
    m_mainLayout->addWidget(m_switchBtn, 0, Qt::AlignVCenter);
    setLayout(m_mainLayout);

    connect(m_switchBtn, &DSwitchButton::toggled, this, &SwitchWidget::checkedChanged);
}

void SwitchWidget::setChecked(const bool checked)
{
    m_switchBtn->blockSignals(true);
    m_switchBtn->setChecked(checked);
    m_switchBtn->blockSignals(false);
}

QString SwitchWidget::title() const
{
    QLabel *label = qobject_cast<QLabel *>(m_leftWidget);
    if (label) {
        return label->text();
    }

   return QString();
}

void SwitchWidget::setTitle(const QString &title)
{
    SwitchLabel *label = qobject_cast<SwitchLabel *>(m_leftWidget);
    if (label) {
        label->setWordWrap(true);
        label->setText(title);
        label->setWordWrap(true);
    }
    setAccessibleName(title);
    m_switchBtn->setAccessibleName(title);
}

bool SwitchWidget::checked() const
{
    return m_switchBtn->isChecked();
}

void SwitchWidget::setLeftWidget(QWidget *widget)
{
    if (!widget)
        return;
    QLayout *lableLayout = m_mainLayout->itemAt(0)->layout();
    lableLayout->removeWidget(m_leftWidget);
    m_leftWidget->deleteLater();
    m_leftWidget = widget;
    lableLayout->addWidget(m_leftWidget);
}

void SwitchWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_switchBtn->geometry().contains(event->pos())) {
        Q_EMIT clicked();
    }

    return SettingsItem::mouseReleaseEvent(event);
}
