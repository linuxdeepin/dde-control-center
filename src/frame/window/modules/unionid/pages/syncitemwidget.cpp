/*
 * Copyright (C) 2017 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     liumaochuan <liumaochuan@uniontech.com>
 *
 * Maintainer: liumaochuan <liumaochuan@uniontech.com>
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

#include "syncitemwidget.h"
#include "newstyle.h"
#include <QHBoxLayout>
#include <QLabel>
#include <DFontSizeManager>
#include "window/utils.h"

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

syncItemwiget::syncItemwiget(QString icon, QString name, bool checked,QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *hlayout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    DGuiApplicationHelper::ColorType type  = DGuiApplicationHelper::instance()->themeType();

    QString themeIcon = icon;
    QString themeType;

    if (type == DGuiApplicationHelper::LightType) {
        themeType = "light";
        themeIcon.append("_light_24px.svg");
    } else if (type == DGuiApplicationHelper::DarkType){
        themeType = "dark";
        themeIcon.append("_dark_24px.svg");
    }

    m_qstrIconName = icon;
    m_iconLabel = new QLabel;
    m_iconLabel->setPixmap(QIcon(QString(":/themes/%1/icons/").arg(themeType) + themeIcon).pixmap(ListViweIconSize));
    m_iconLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *nameLabel = new QLabel;
    nameLabel->setText(name);
    DFontSizeManager::instance()->bind(nameLabel, DFontSizeManager::T6,QFont::Normal);

    m_checkBox = new QCheckBox;
    m_checkBox->setStyle(new newStyle());
    m_checkBox->setMinimumSize(QSize(14,14));
    m_checkBox->setChecked(checked);

    QFrame *line1 = new QFrame;
    line1->setFrameShape(QFrame::HLine);
    line1->setMinimumSize(QSize(400,1));
    line1->setContentsMargins(0,0,0,0);

    hlayout->addWidget(m_iconLabel);
    hlayout->addWidget(nameLabel);
    hlayout->addStretch();
    hlayout->addWidget(m_checkBox);
    hlayout->setContentsMargins(0,0,20,0);

    vlayout->addLayout(hlayout);
    vlayout->addWidget(line1);
    vlayout->setContentsMargins(0,0,0,0);

    setLayout(vlayout);
    setMinimumSize(QSize(400,56));
    setContentsMargins(0,0,0,0);

    connect(m_checkBox,&QCheckBox::stateChanged,this,&syncItemwiget::onStateChanged);
    connect(DGuiApplicationHelper::instance(),&DGuiApplicationHelper::themeTypeChanged,this,&syncItemwiget::onThemeChanged);
}

QCheckBox *syncItemwiget::getCheckBox()
{
    return m_checkBox;
}

void syncItemwiget::setSyncType(SyncType syncType)
{
    m_syncType = syncType;
}

void syncItemwiget::onThemeChanged()
{
    DGuiApplicationHelper::ColorType type  = DGuiApplicationHelper::instance()->themeType();

    QString themeIcon = m_qstrIconName;
    QString themeType;

    if (type == DGuiApplicationHelper::LightType) {
        themeType = "light";
        themeIcon.append("_light_24px.svg");
    } else if (type == DGuiApplicationHelper::DarkType){
        themeType = "dark";
        themeIcon.append("_dark_24px.svg");
    }

    m_iconLabel->clear();
    m_iconLabel->setPixmap(QIcon(QString(":/themes/%1/icons/").arg(themeType) + themeIcon).pixmap(ListViweIconSize));
    m_iconLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

void syncItemwiget::onStateChanged(int state)
{
    bool bIsState = false;

    if (state == Qt::Checked) {
        bIsState = true;
    }

    Q_EMIT toTellStateChanged(bIsState,m_syncType);
}
