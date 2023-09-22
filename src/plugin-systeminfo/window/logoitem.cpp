// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "logoitem.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QImageReader>
#include <QLabel>

#include <DIconTheme>
DGUI_USE_NAMESPACE

#define FRAME_WIDTH 408
#define NAVBAR_WIDTH 56

namespace DCC_NAMESPACE {

LogoItem::LogoItem(QFrame *parent)
    : SettingsItem(parent)
{
    m_logo = new QLabel;
    m_description = new QLabel;
    m_description->setWordWrap(false);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addStretch();
    layout->addWidget(m_logo);
    layout->setSpacing(5);
    layout->addWidget(m_description);
    layout->addStretch();
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);

    m_description->setVisible(false);
}

void LogoItem::setDescription(const QString &des)
{
    m_description->setScaledContents(true);
    m_description->setText(des);
}

void LogoItem::setDescription(bool isVisible)
{
    m_description->setVisible(isVisible);
}

void LogoItem::setLogo(const QString &logo)
{
    m_logo->setPixmap(DIconTheme::findQIcon(logo).pixmap(100, 20));
}

const QString LogoItem::logo()
{
    return m_logo->text();
}
} // namespace DCC_NAMESPACE
