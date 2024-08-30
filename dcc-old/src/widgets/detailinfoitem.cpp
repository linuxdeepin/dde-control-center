//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/detailinfoitem.h"
#include "src/plugin-update/operation/common.h"
#include "widgets/accessibleinterface.h"

#include <DFontSizeManager>
#include <DLabel>

#include <QVBoxLayout>
#include <QScrollArea>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE
using namespace DCC_NAMESPACE;
SET_FORM_ACCESSIBLE(DetailInfoItem,"DetailInfoItem");
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

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &DetailInfoItem::onThemeChanged);
}

void DetailInfoItem::initUi()
{
    QVBoxLayout *mainlayout = new QVBoxLayout;

    mainlayout->setSpacing(0);
    mainlayout->setMargin(0);

    QHBoxLayout *hboxlayout = new QHBoxLayout;

    QPalette titleClolrPal;
    titleClolrPal.setColor(QPalette::WindowText, QColor(titleColor));

    QPalette grayColorPal;
    grayColorPal.setColor(QPalette::WindowText, QColor(grayColor));

    m_title->setFixedWidth(220);
    DFontSizeManager::instance()->bind(m_title, DFontSizeManager::T7, QFont::DemiBold);
    m_title->setForegroundRole(DPalette::TextTitle);

    DFontSizeManager::instance()->bind(m_dateLabel, DFontSizeManager::T8);
    m_dateLabel->setForegroundRole(DPalette::TextTips);

    hboxlayout->addWidget(m_title, 0, Qt::AlignLeft | Qt::AlignTop);
    hboxlayout->addWidget(m_dateLabel, 0, Qt::AlignRight | Qt::AlignTop);

    DFontSizeManager::instance()->bind(m_dataLable, DFontSizeManager::T8);
    m_dataLable->setForegroundRole(DPalette::TextTips);
    m_dataLable->setWordWrap(true);
    m_dataLable->setOpenExternalLinks(true);

    QHBoxLayout *bomboxlayout = new QHBoxLayout;
    m_linkDataLabel->setText(tr("For more details, visit:"));
    DFontSizeManager::instance()->bind(m_linkDataLabel, DFontSizeManager::T8);
    m_linkDataLabel->setForegroundRole(DPalette::TextTips);
    m_linkDataLabel->setMaximumWidth(250);

    m_linkLable->setOpenExternalLinks(true);
    DFontSizeManager::instance()->bind(m_linkLable, DFontSizeManager::T8);
    m_linkLable->setForegroundRole(DPalette::LinkVisited);
    bomboxlayout->addWidget(m_linkDataLabel, 0, Qt::AlignLeft);
    bomboxlayout->addWidget(m_linkLable, 10, Qt::AlignLeft);

    m_linkDataLabel->setVisible(false);
    m_linkLable->setVisible(false);

    mainlayout->addLayout(hboxlayout);
    mainlayout->addSpacing(5);
    mainlayout->addWidget(m_dataLable);
    mainlayout->addSpacing(5);
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
    if (data.isEmpty()) {
        m_linkDataLabel->setVisible(false);
        m_linkLable->setVisible(false);
        return;
    }

    QString value = QString("<a href=\"%1\">%2").arg(data, data);
    m_linkLable->setText(value);
    m_linkDataLabel->setVisible(true);
    m_linkLable->setVisible(true);
}

void DetailInfoItem::setDetailData(QString data)
{
    m_dataLable->clear();
    m_dataLable->setText(htmlToCorrectColor(data));
}

void DetailInfoItem::onThemeChanged()
{
    m_dataLable->setText(htmlToCorrectColor(m_dataLable->text()));
}