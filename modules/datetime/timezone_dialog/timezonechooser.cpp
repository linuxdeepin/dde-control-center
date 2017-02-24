/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "timezonechooser.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTimeZone>
#include <QTimer>
#include <QCompleter>
#include <QKeyEvent>
#include <QDebug>

#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>

#include "timezone_map.h"
#include "searchinput.h"

using namespace dcc::widgets;

namespace dcc {
namespace datetime {

TimeZoneChooser::TimeZoneChooser()
    : BlurredFrame(),
      m_map(new installer::TimezoneMap(this)),
      m_searchInput(new SearchInput),
      m_title(new QLabel(tr("Change Timezone"))),
      m_cancelBtn(new QPushButton(tr("Cancel"))),
      m_confirmBtn(new QPushButton(tr("Confirm")))
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setupSize();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addStretch();
    layout->addWidget(m_title, 0, Qt::AlignHCenter);
    layout->addSpacing(40);
    layout->addWidget(m_searchInput, 0, Qt::AlignHCenter);
    layout->addSpacing(40);
    layout->addWidget(m_map, 0, Qt::AlignHCenter);
    layout->addSpacing(40);
    layout->addWidget(m_cancelBtn, 0, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(m_confirmBtn, 0, Qt::AlignHCenter);
    layout->addStretch();

    setLayout(layout);

    connect(m_confirmBtn, &QPushButton::clicked, [this] {
        QString zone = m_map->getTimezone();
        emit confirmed(zone);

        hide();
    });

    connect(m_cancelBtn, &QPushButton::clicked, this, [this] {
        hide();
        emit cancelled();
    });

    connect(m_searchInput, &SearchInput::editingFinished, [this] {
        QString timezone = m_searchInput->text();
        timezone = m_completionCache.value(timezone, timezone);
        m_map->setTimezone(timezone);
    });

    QTimer::singleShot(0, [this] {
        QStringList completions;
        for (QString timezone : QTimeZone::availableTimeZoneIds()) {
            completions << timezone; // whole timezone as completion candidate.

            QStringList zoneParts = timezone.split("/");
            if (zoneParts.length() == 2) {
                QString cityName = zoneParts.at(1);
                completions << cityName; // timezone city name as completion candidate.
                m_completionCache[cityName] = timezone;
            }
        }

        QCompleter *completer = new QCompleter(completions, m_searchInput);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setCaseSensitivity(Qt::CaseInsensitive);

        m_searchInput->setCompleter(completer);
    });
}

void TimeZoneChooser::keyReleaseEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Cancel)) {
        hide();
        emit cancelled();
    }
}

QSize TimeZoneChooser::getFitSize() const
{
    const QDesktopWidget *desktop = QApplication::desktop();
    const QRect primaryRect = desktop->availableGeometry(desktop->primaryScreen());

    double width = primaryRect.width() - 360/* dcc */ - 20 * 2;
    double height = primaryRect.height() - 70/* dock */ - 20 * 2;

    return QSize(width, height);
}

void TimeZoneChooser::setupSize()
{
    static const double MapPixWidth = 978.0;
    static const double MapPixHeight = 500.0;
    static const double MapPictureWidth = 978.0;
    static const double MapPictureHeight = 500.0;

    QFont font = m_title->font();
    font.setPointSizeF(16.0);
    m_title->setFont(font);

    setRadius(4);

    const QSize fitSize = getFitSize();
    setFixedSize(fitSize.width(), fitSize.height());

    const float mapWidth = qMin(MapPixWidth, fitSize.width() - 20 * 2.0);
    const float mapHeight = qMin(MapPixHeight, fitSize.height() - 20 * 2/*paddings*/ - 36 * 2/*buttons*/ - 10/*button spacing*/ - 40 * 3.0 /*spacings*/ - 30/*title*/ -  20 * 2/*top bottom margin*/);
    const double widthScale = MapPictureWidth / mapWidth;
    const double heightScale = MapPictureHeight / mapHeight;
    const double scale = qMax(widthScale, heightScale);

    m_map->setFixedSize(MapPictureWidth / scale, MapPictureHeight / scale);

    m_searchInput->setFixedWidth(250);
    m_cancelBtn->setFixedWidth(250);
    m_confirmBtn->setFixedWidth(250);
}

} // namespace datetime
} // namespace dcc
