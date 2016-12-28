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

#include "timezone_map.h"
#include "searchinput.h"

using namespace dcc::widgets;

namespace dcc {
namespace datetime {

TimeZoneChooser::TimeZoneChooser()
    : BlurredFrame(),
      m_map(new installer::TimezoneMap),
      m_searchInput(new SearchInput),
      m_cancelBtn(new QPushButton(tr("Cancel"))),
      m_confirmBtn(new QPushButton(tr("Confirm")))
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    m_searchInput->setFixedWidth(300);

    m_cancelBtn->setFixedWidth(200);
    m_confirmBtn->setFixedWidth(200);

    m_map->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addStretch();
    layout->addWidget(m_searchInput, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
    layout->addWidget(m_map, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
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

    connect(m_cancelBtn, &QPushButton::clicked, this, &TimeZoneChooser::hide);

    connect(m_searchInput, &SearchInput::editingFinished, [this] {
        m_map->setTimezone(m_searchInput->text());
    });

    QTimer::singleShot(0, [this] {
        QStringList completions;
        for (QByteArray arry : QTimeZone::availableTimeZoneIds()) {
            completions << arry;
        }

        QCompleter *completer = new QCompleter(completions, m_searchInput);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setCaseSensitivity(Qt::CaseInsensitive);

        m_searchInput->setCompleter(completer);
    });
}

} // namespace datetime
} // namespace dcc
