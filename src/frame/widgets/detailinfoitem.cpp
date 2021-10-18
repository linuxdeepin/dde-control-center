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
    , m_explainTitle(new DLabel(this))
    , m_linkDataLabel(new DLabel(this))
    , m_dataLable(new DLabel(this))
    , m_linkLable(new DLabel(this))
    , m_title(new DLabel(this))
{
    initUi();
}

void DetailInfoItem::initUi()
{
    QVBoxLayout *mainlayout = new QVBoxLayout;
    QHBoxLayout *hboxlayout = new QHBoxLayout;

    QPalette titleClolrPal;
    titleClolrPal.setColor(QPalette::WindowText, QColor(titleColor));

    QPalette grayColorPal;
    grayColorPal.setColor(QPalette::WindowText, QColor(grayColor));


    m_title->setText("桌面版1040");
    m_title->setFixedWidth(220);
    DFontSizeManager::instance()->bind(m_title, DFontSizeManager::T7, QFont::DemiBold);
    m_title->setForegroundRole(DPalette::TextTitle);

    m_dateLabel->setText("2021.10.1");
    DFontSizeManager::instance()->bind(m_dateLabel, DFontSizeManager::T8);
    m_dateLabel->setForegroundRole(DPalette::TextTips);

    hboxlayout->addWidget(m_title, 0, Qt::AlignLeft);
    hboxlayout->addWidget(m_dateLabel, 0, Qt::AlignRight);

//    m_explainTitle->setText("本次更新的内容为:");
//  DFontSizeManager::instance()->bind(m_explainTitle, DFontSizeManager::T8);
//  m_explainTitle->setForegroundRole(DPalette::TextTips);

    DFontSizeManager::instance()->bind(m_dataLable, DFontSizeManager::T8);
    m_dataLable->setForegroundRole(DPalette::TextTips);
    m_dataLable->setWordWrap(true);

    QHBoxLayout *bomboxlayout = new QHBoxLayout;
    m_linkDataLabel->setText("有关本次更新的详细内容可访问:");
    DFontSizeManager::instance()->bind(m_linkDataLabel, DFontSizeManager::T8);
    m_linkDataLabel->setForegroundRole(DPalette::TextTips);
    m_linkDataLabel->setMaximumWidth(250);

    m_linkLable->setText("<a href=\"http://www.baidu.com/\">http://www.baidu.com");
    m_linkLable->setOpenExternalLinks(true);
    DFontSizeManager::instance()->bind(m_linkLable, DFontSizeManager::T8);
    m_linkLable->setForegroundRole(DPalette::LinkVisited);
    //  m_linkLable->setMaximumWidth(150);
    bomboxlayout->addWidget(m_linkDataLabel, 0, Qt::AlignLeft);
    bomboxlayout->addWidget(m_linkLable, 10, Qt::AlignLeft);

    mainlayout->addLayout(hboxlayout);
    mainlayout->addWidget(m_explainTitle);
    mainlayout->addWidget(m_dataLable);
    mainlayout->addLayout(bomboxlayout);

    setLayout(mainlayout);
}

void DetailInfoItem::setDate(QString date)
{
    m_dateLabel->clear();
    m_dateLabel->setText(date);
}

void DetailInfoItem::setTitle(QString title)
{
    m_title->clear();
    m_title->setText(title);
}

void DetailInfoItem::setExplaintTitle(QString title)
{
    m_explainTitle->clear();
    m_explainTitle->setText(title);
}

void DetailInfoItem::setLinkData(QString data)
{
    m_linkLable->clear();
    QString value = QString("<a href=\"%1\">%2").arg(data, data);
    m_linkLable->setText(value);
}

void DetailInfoItem::setDetailData(QString data)
{
    m_dataLable->clear();
    m_dataLable->setText(data);
}

}
}
