// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QIcon>
#include "optionwidget.h"
#include "widgets/basiclistdelegate.h"

using namespace dcc;
using namespace dcc::defapp;
using namespace dcc::widgets;
OptionWidget::OptionWidget(const bool state, QWidget *parent)
    :SettingsItem(parent),
      m_delete(new DIconButton(this))
{
    m_checkedIconLabel = new QLabel;
    m_checkedIconLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_optionIcon = new QLabel;
    m_optionIcon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_displayName = new NormalLabel;
    m_displayName->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_execPath = new QLabel;
    m_execPath->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_execPath->setVisible(state);

    m_delete->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //send delete dbus
    connect(m_delete, &DImageButton::clicked, [ = ] {
        Q_EMIT removeItem(m_CategoryItem);
    });

    m_delete->setFixedSize(17, 17);
    m_optionIcon->setFixedSize(17, 17);
    m_checkedIconLabel->setFixedSize(17, 17);

    QHBoxLayout *m_leftOption = new QHBoxLayout;
    m_leftOption->addWidget(m_optionIcon,  0, Qt::AlignCenter);
    m_leftOption->addSpacing(14);
    m_leftOption->addWidget(m_displayName, 0, Qt::AlignCenter);
    m_leftOption->addWidget(m_execPath,        0, Qt::AlignCenter);
    m_leftOption->addStretch();
    m_leftOption->setContentsMargins(10,0,0,0);

    QHBoxLayout *m_rightOption = new QHBoxLayout;
    m_rightOption->setSpacing(0);
    m_rightOption->addStretch();
    m_rightOption->addWidget(m_checkedIconLabel, 0, Qt::AlignCenter);
    m_rightOption->addWidget(m_delete,           0, Qt::AlignCenter);
    m_checkedIconLabel->setVisible(false);
    m_delete->setVisible(false);

    m_mainlayout = new QHBoxLayout;
    m_mainlayout->setSpacing(0);
    m_mainlayout->addLayout(m_leftOption, Qt::AlignLeft);
    m_mainlayout->addStretch();
    m_mainlayout->addLayout(m_rightOption, Qt::AlignRight);
    m_mainlayout->setContentsMargins(0, 0, 10, 0);

    setLayout(m_mainlayout);
    setObjectName("OptionWidget");
}

void OptionWidget::setItem(const App &item)
{
    m_CategoryItem = item;

    if (item.Name.isEmpty()) {
        m_displayName->setText(item.DisplayName);
    } else {
        m_displayName->setText(item.Name);
    }

    m_userCheck = item.isUser;

    const QIcon &icon = QIcon::fromTheme(item.Icon, QIcon::fromTheme("application-x-desktop"));

    const qreal ratio = qApp->devicePixelRatio();

    QPixmap pixmap = icon.pixmap(16 * ratio, 16 * ratio).scaled(16 * ratio, 16 * ratio,
                                                                Qt::KeepAspectRatioByExpanding,
                                                                Qt::FastTransformation);

    pixmap.setDevicePixelRatio(ratio);

    m_optionIcon->setPixmap(pixmap);

    m_execPath->setText(" ("+ item.Exec + ")");
    setAccessibleName(item.Id);
}
void OptionWidget::setDelete(const bool delchecked)     //删除
{
    if (m_userCheck) {
        if (delchecked) {
            m_delete->setVisible(true);
            m_checkedIconLabel->setVisible(false);
        } else {
            m_delete->setVisible(false);
            if (m_checked) {
                m_checkedIconLabel->setVisible(true);
            }
        }
    }
}

void OptionWidget::setMime(const QString &mime)
{
    m_mime = mime;
}

bool OptionWidget::checked() const
{
    return m_checked;
}

void OptionWidget::setChecked(const bool checked)      //判断是否显示设置图标
{
    m_checkedIconLabel->setVisible(checked);
    m_delete->setVisible(false);
    setCheckedIcon(loadPixmap(":/defapp/icons/select.png"));
    m_checked = checked;
    Q_EMIT checkedChanged(m_checked);
}

void OptionWidget::setCheckedIcon(const QPixmap &icon)
{
    //set default checkIcon
    m_checkedIconLabel->setPixmap(icon);
}

bool OptionWidget::userCheck() const
{
    return m_userCheck;
}

QHBoxLayout *OptionWidget::layout()
{
    return m_mainlayout;
}

void OptionWidget::mouseReleaseEvent(QMouseEvent *)
{
    setChecked(true);
    Q_EMIT setDefault();
}
