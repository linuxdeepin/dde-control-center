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

#include "widgets/comboxwidget.h"
#include "widgets/labels/normallabel.h"
#include "window/utils.h"

#include <QHBoxLayout>
#include <QComboBox>
#include <QDebug>
#include <QStringList>
#include <QMouseEvent>
#include <QLabel>
#include <QPainter>

using namespace DCC_NAMESPACE;
namespace dcc {
namespace widgets {

ComboxWidget::ComboxWidget(QFrame *parent)
    : ComboxWidget(new NormalLabel, parent)
{

}

ComboxWidget::ComboxWidget(const QString &title, QFrame *parent)
    : ComboxWidget(new NormalLabel(title), parent)
{

}

ComboxWidget::ComboxWidget(QWidget *widget, QFrame *parent)
    : SettingsItem(parent)
    , m_leftWidget(widget)
    , m_switchComboBox(new AlertComboBox(this))
    , m_str("")
{
    // FIXME: 默认统一控件高度
    setFixedHeight(ComboxWidgetHeight);
    QHBoxLayout *mainLayout = new QHBoxLayout;
    m_titleLabel = qobject_cast<QLabel *>(m_leftWidget);
    if (m_titleLabel) {
        m_str = m_titleLabel->text();
    }

    mainLayout->addWidget(m_leftWidget, 0, Qt::AlignVCenter);
    mainLayout->setStretchFactor(m_leftWidget,3);
    mainLayout->addWidget(m_switchComboBox, 0, Qt::AlignVCenter);
    mainLayout->setStretchFactor(m_switchComboBox,7);
    m_switchComboBox->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    mainLayout->setContentsMargins(10, 0, 10, 0);

    m_leftWidget->setFixedWidth(ComboxTitleWidth);
    m_leftWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setLayout(mainLayout);

    connect(m_switchComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ComboxWidget::onIndexChanged);
    connect(m_switchComboBox, &QComboBox::currentTextChanged, this, &ComboxWidget::onSelectChanged);
    connect(m_switchComboBox, &QComboBox::currentTextChanged, this, [this] {
        Q_EMIT dataChanged(m_switchComboBox->currentData());
    });
}

void ComboxWidget::setComboxOption(const QStringList &options)
{
    m_switchComboBox->blockSignals(true);
    m_switchComboBox->clear();
    for (QString item : options) {
        m_switchComboBox->addItem(item);
    }
    m_switchComboBox->blockSignals(false);
}

void ComboxWidget::setCurrentText(const QString &curText)
{
    m_switchComboBox->blockSignals(true);
    m_switchComboBox->setCurrentText(curText);
    m_switchComboBox->blockSignals(false);
}

void ComboxWidget::setCurrentIndex(const int index)
{
    m_switchComboBox->blockSignals(true);
    m_switchComboBox->setCurrentIndex(index);
    m_switchComboBox->blockSignals(false);
}

void ComboxWidget::setTitle(const QString &title)
{
    QLabel *label = qobject_cast<QLabel *>(m_leftWidget);
    if (label) {
        label->setWordWrap(true);
        label->setText(title);
        m_str = title;
    }

    setAccessibleName(m_str);
}

AlertComboBox *ComboxWidget::comboBox()
{
    return m_switchComboBox;
}

void ComboxWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_switchComboBox->geometry().contains(event->pos())) {
        Q_EMIT clicked();
    }

    return SettingsItem::mouseReleaseEvent(event);
}

void ComboxWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (event->type() == QEvent::Resize) {
        if (m_titleLabel) {
            QFontMetrics fontMetrics(m_titleLabel->font());
            int fontSize = fontMetrics.horizontalAdvance(m_str);
            if (fontSize > m_titleLabel->width()) {
                m_titleLabel->setText(fontMetrics.elidedText(m_str, Qt::ElideRight, m_titleLabel->width()));

                m_titleLabel->setToolTip(m_str);
            } else {
                m_titleLabel->setText(m_str);
                m_titleLabel->setToolTip("");
            }
        }
    }
    SettingsItem::resizeEvent(event);
}

/**
 * @brief 错误提示下拉框
 * @param parent
 */
AlertComboBox::AlertComboBox(QWidget *parent)
    : QComboBox (parent)
    , m_isWarning(false)
{
    connect(this, &AlertComboBox::currentTextChanged, this, &AlertComboBox::onValueChange);
}

AlertComboBox::~AlertComboBox()
{
}

void AlertComboBox::setIsWarning(bool isWarning)
{
    m_isWarning = isWarning;
    update();
}

bool AlertComboBox::isWarning()
{
    return m_isWarning;
}

void AlertComboBox::onValueChange(const QString &text)
{
    if (!m_isWarning)
        return;

    if (!text.isEmpty())
        setIsWarning(false);
}

void AlertComboBox::paintEvent(QPaintEvent *e)
{
    QComboBox::paintEvent(e);
    if (m_isWarning) {
        QPainter painter(this);
        painter.save();
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(241, 57, 50, qRound(0.15 * 255)));
        QRect r = rect().adjusted(2,2,-2,-2);
        painter.drawRoundedRect(r, 8, 8);
        painter.restore();
    }
}

}
}
