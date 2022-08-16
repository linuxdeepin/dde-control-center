/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     shenwenqi <shenwenqi@uniontech.com>

 * Maintainer: shenwenqi <shenwenqi@uniontech.com>
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

#include "widgets/detailinfoitem.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <DFontSizeManager>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

namespace dcc {
namespace widgets {

DetailInfoItem::DetailInfoItem(QWidget *parent)
    : SettingsItem(parent)
    , m_dateLabel(new DLabel(this))
    , m_linkDataLabel(new DLabel(this))
    , m_dataLabel(new DLabel(this))
    , m_linkLabel(new DLabel(this))
    , m_title(new DLabel(this))
{
    initUi();
}

void DetailInfoItem::initUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->setSpacing(5);
    mainLayout->setMargin(0);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;

    QPalette titleColorPal;
    titleColorPal.setColor(QPalette::WindowText, QColor(titleColor));

    QPalette grayColorPal;
    grayColorPal.setColor(QPalette::WindowText, QColor(grayColor));

    m_title->setFixedWidth(220);
    DFontSizeManager::instance()->bind(m_title, DFontSizeManager::T7, QFont::DemiBold);
    m_title->setForegroundRole(DPalette::TextTitle);

    DFontSizeManager::instance()->bind(m_dateLabel, DFontSizeManager::T8);
    m_dateLabel->setForegroundRole(DPalette::TextTips);

    hBoxLayout->addWidget(m_title, 0, Qt::AlignLeft | Qt::AlignTop);
    hBoxLayout->addWidget(m_dateLabel, 0, Qt::AlignRight | Qt::AlignTop);

    DFontSizeManager::instance()->bind(m_dataLabel, DFontSizeManager::T8);
    m_dataLabel->setForegroundRole(DPalette::TextTips);
    m_dataLabel->setWordWrap(true);
    m_dataLabel->setOpenExternalLinks(true);

    QHBoxLayout *bomBoxLayout = new QHBoxLayout;
    m_linkDataLabel->setText(tr("For more details, visit:"));
    DFontSizeManager::instance()->bind(m_linkDataLabel, DFontSizeManager::T8);
    m_linkDataLabel->setForegroundRole(DPalette::TextTips);
    m_linkDataLabel->setMaximumWidth(250);

    m_linkLabel->setOpenExternalLinks(true);
    DFontSizeManager::instance()->bind(m_linkLabel, DFontSizeManager::T8);
    m_linkLabel->setForegroundRole(DPalette::LinkVisited);
    bomBoxLayout->addWidget(m_linkDataLabel, 0, Qt::AlignLeft);
    bomBoxLayout->addWidget(m_linkLabel);
    m_linkLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_linkLabel->setScaledContents(true);
    m_linkLabel->adjustSize();
    m_linkLabel->setWordWrap(true);

    mainLayout->addLayout(hBoxLayout);
    mainLayout->addWidget(m_dataLabel);
    mainLayout->addLayout(bomBoxLayout);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

void DetailInfoItem::setDate(QString date)
{
    if (date.isEmpty()) {
        m_dateLabel->setVisible(false);
        return;
    }
    m_dateLabel->setVisible(true);
    m_dateLabel->clear();
    m_dateLabel->setText(date);
}

void DetailInfoItem::setTitle(QString title)
{
    m_title->clear();
    m_title->setText(title);
}

void DetailInfoItem::setLinkData(QString data)
{
    if (data.isEmpty()) {
        m_linkDataLabel->setVisible(false);
        m_linkLabel->setVisible(false);
        return;
    }
    m_linkDataLabel->setVisible(true);
    m_linkLabel->setVisible(true);
    m_linkLabel->clear();
    QString value = QString("<a href=\"%1\">%2").arg(data, data);
    m_linkLabel->setText(value);
}

void DetailInfoItem::setDetailData(QString data)
{
    if (data.isEmpty()) {
        m_dataLabel->setVisible(false);
        return;
    }
    m_dataLabel->setVisible(true);
    m_dataLabel->clear();
    m_dataLabel->setText(data);
}

}
}
