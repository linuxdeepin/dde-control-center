// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/spinboxwidget.h"
#include "window/utils.h"

#include <QSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QEvent>

DWIDGET_USE_NAMESPACE
using namespace dcc;
using namespace dcc::widgets;

SpinBoxWidget::SpinBoxWidget(QWidget *parent)
    : SettingsItem(parent)
    , m_defaultVal(0)
    , m_title(new QLabel)
    , m_spinBox(new DSpinBox)
    , m_resetBtn(new DIconButton(this))
{
    setAccessibleName("SpinBoxWidget");
    m_title->setAccessibleName("SpinBoxWidget_title");
    m_spinBox->setAccessibleName("spinBox");
    m_resetBtn->setAccessibleName("resetBtn");
    m_spinBox->setContextMenuPolicy(Qt::NoContextMenu);
    m_spinBox->setButtonSymbols(QAbstractSpinBox::PlusMinus);

    m_title->setMinimumWidth(DCC_NAMESPACE::LeftTitleWitdh);
    m_resetBtn->setIcon(QIcon::fromTheme("dcc_reset"));
    m_resetBtn->setIconSize(QSize(16, 16));
    m_resetBtn->setFixedSize(36, 36);

    QHBoxLayout *rightLayout = new QHBoxLayout;
    rightLayout->addWidget(m_spinBox, 0);
    rightLayout->addWidget(m_resetBtn, 0);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title, 3);
    mainLayout->addLayout(rightLayout, 7);

    connect(m_resetBtn, &DIconButton::clicked, [this] {
        m_spinBox->setValue(m_defaultVal);
    });
    setLayout(mainLayout);
    m_spinBox->installEventFilter(this);
    m_spinBox->setFocusPolicy(Qt::ClickFocus);
}

void SpinBoxWidget::setDefaultVal(int defaultVal)
{
    m_defaultVal = defaultVal;
    m_spinBox->setValue(m_defaultVal);
}

bool SpinBoxWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel && m_spinBox == obj) {
        if (!m_spinBox->hasFocus())
            return true;
    }
    return QObject::eventFilter(obj, event);
}

void SpinBoxWidget::setTitle(const QString &title)
{
    m_title->setText(title);
    m_title->setWordWrap(true);
}
