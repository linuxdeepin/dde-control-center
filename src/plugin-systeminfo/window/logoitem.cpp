//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "logoitem.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QImageReader>
#include <QIcon>


#define FRAME_WIDTH     408
#define NAVBAR_WIDTH    56

QPixmap loadPixmap(const QString &path)
{
    qreal ratio = 1.0;
    QPixmap pixmap;

    const qreal devicePixelRatio = qApp->devicePixelRatio();

    if (!qFuzzyCompare(ratio, devicePixelRatio)) {
        QImageReader reader;
        reader.setFileName(qt_findAtNxFile(path, devicePixelRatio, &ratio));
        if (reader.canRead()) {
            reader.setScaledSize(reader.size() * (devicePixelRatio / ratio));
            pixmap = QPixmap::fromImage(reader.read());
            pixmap.setDevicePixelRatio(devicePixelRatio);
        }
    } else {
        pixmap.load(path);
    }

    return pixmap;
}

namespace DCC_NAMESPACE {

LogoItem::LogoItem(QFrame *parent)
    : SettingsItem(parent)
{
    m_logo = new QLabel;
    m_description = new QLabel;
    m_description->setWordWrap(true);
    m_description->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    m_description->setMinimumWidth(FRAME_WIDTH - NAVBAR_WIDTH - 20);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_logo);
    layout->addWidget(m_description);
    layout->setSpacing(15);
    layout->setContentsMargins(0, 25, 0, 20);

    layout->setAlignment(m_logo, Qt::AlignCenter);
    layout->setAlignment(m_description, Qt::AlignCenter);
    setLayout(layout);

    m_description->setVisible(false);
}

void LogoItem::setDescription(const QString &des)
{
    m_description->setScaledContents(true);
    m_description->setText(des);
    QRect r = m_description->fontMetrics().boundingRect(QRect(0, 0, m_description->minimumWidth(), 100), Qt::TextWordWrap, des);
    m_description->setMinimumHeight(r.height());
}

void LogoItem::setDescription(bool isVisible)
{
    m_description->setVisible(isVisible);
}

void LogoItem::setLogo(const QString &logo)
{
    m_logo->setPixmap(loadPixmap(logo));
}

const QString LogoItem::logo()
{
     return m_logo->text();
}

}
