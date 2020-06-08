/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "timezonechooser.h"
#include "timezone_map.h"
#include "widgets/searchinput.h"

#include <dplatformwindowhandle.h>
#include <DDialogCloseButton>
#include <DApplicationHelper>

#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTimeZone>
#include <QTimer>
#include <QCompleter>
#include <QKeyEvent>
#include <QDebug>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QStyleFactory>
#include <QAbstractItemView>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace dcc::datetime;

TimeZoneChooser::TimeZoneChooser()
    : QFrame()
    , m_blurEffect(new DBlurEffectWidget(this))
    , m_map(new installer::TimezoneMap(this))
    , m_searchInput(new SearchInput)
    , m_title(new QLabel)
    , m_cancelBtn(new QPushButton(tr("Cancel")))
    , m_confirmBtn(new DSuggestButton(tr("Confirm")))
{
//    setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    setWindowFlags(Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setupSize();

    //设置成设计给出的宽高
    setFixedSize(1000, 760);

    m_searchInput->setMinimumSize(350, 36);

    m_confirmBtn->setEnabled(false);

    DPalette pa = DApplicationHelper::instance()->palette(m_title);
    pa.setBrush(QPalette::WindowText, pa.base());
    DApplicationHelper::instance()->setPalette(m_title, pa);

    QFont font;
    font.setPixelSize(40);
    m_title->setFont(font);

    DPlatformWindowHandle handle(this);
    handle.setWindowRadius(18);

    m_blurEffect->setBlendMode(DBlurEffectWidget::InWindowBlend);
    m_blurEffect->setMaskColor(Qt::gray);


    QHBoxLayout *wbLayout = new QHBoxLayout;
    wbLayout->setMargin(6);
    wbLayout->setSpacing(0);
    wbLayout->addStretch();

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(m_cancelBtn, 0, Qt::AlignHCenter);
    hLayout->addSpacing(20);
    hLayout->addWidget(m_confirmBtn, 0, Qt::AlignHCenter);
    hLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addLayout(wbLayout);
    layout->addWidget(m_title, 0, Qt::AlignHCenter | Qt::AlignTop);
    layout->addSpacing(10);
    layout->addWidget(m_searchInput, 0, Qt::AlignHCenter | Qt::AlignTop);
    layout->addSpacing(10);
    layout->addWidget(m_map, 0, Qt::AlignHCenter);
    layout->addStretch();
    layout->addLayout(hLayout);
    layout->addSpacing(10);
    setLayout(layout);

    connect(m_confirmBtn, &DSuggestButton::clicked, [this] {
        QString zone = m_map->getTimezone();
        Q_EMIT confirmed(zone);
    });

    connect(m_cancelBtn, &QPushButton::clicked, this, [this] {
        Q_EMIT cancelled();

        close();
    });

    connect(m_searchInput, &SearchInput::editingFinished, [this] {
        QString timezone = m_searchInput->text();
        timezone = m_completionCache.value(timezone, timezone);
        m_map->setTimezone(timezone);
    });

    connect(m_searchInput, &SearchInput::textChanged, m_searchInput, &SearchInput::editingFinished);

    connect(m_map, &installer::TimezoneMap::timezoneUpdated, this, [this] {
        m_searchInput->setText("");
        m_searchInput->clearFocus();
        m_confirmBtn->setEnabled(true);
    });

    QTimer::singleShot(0, [this] {
        QStringList completions;
        for (QString timezone : QTimeZone::availableTimeZoneIds()) {
            completions << timezone; // timezone as completion candidate.

            // localized timezone as completion candidate.
            const QString locale = QLocale::system().name();
            QString localizedTimezone = installer::GetLocalTimezoneName(timezone, locale);
            completions << localizedTimezone;

            m_completionCache[localizedTimezone] = timezone;
        }

        QCompleter *completer = new QCompleter(completions, m_searchInput);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setFilterMode(Qt::MatchContains);

        m_searchInput->setCompleter(completer);

        m_popup = completer->popup();
        m_popup->setObjectName("TimezoneCompleter");
        m_popup->setAttribute(Qt::WA_TranslucentBackground);
        m_popup->setWindowFlags(Qt::Popup | Qt::Sheet);
        m_popup->installEventFilter(this);

        DBlurEffectWidget *blurEffect = new DBlurEffectWidget(m_popup);
        blurEffect->setMaskColor(Qt::white);

        QHBoxLayout *layout = new QHBoxLayout;
        layout->setSpacing(0);
        layout->setMargin(0);
        layout->addWidget(blurEffect);
        m_popup->setLayout(layout);

        blurEffect->lower();
    });
}

void TimeZoneChooser::setIsAddZone(const bool isAdd)
{
    m_isAddZone = isAdd;

    if (isAdd) {
        //~ contents_path /datetime/Timezone List/Add Timezone
        m_title->setText(tr("Add Timezone"));
        m_confirmBtn->setText(tr("Add"));
    } else {
        m_title->setText(tr("Change Timezone"));
        m_confirmBtn->setText(tr("Confirm"));
    }
}

void TimeZoneChooser::setCurrentTimeZoneText(const QString &zone)
{
    if (zone.isEmpty()) return;

    const QString locale = QLocale::system().name();
    const QString name = installer::GetLocalTimezoneName(zone, locale);

//    qDebug() << " TimeZoneChooser::SetCurrentTimeZone , zone : " << name;
    //set current timezone text

}

void TimeZoneChooser::setMarkedTimeZone(const QString &timezone)
{
    m_map->setTimezone(timezone);
}

void TimeZoneChooser::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);

    m_blurEffect->resize(event->size());
    m_blurEffect->lower();
}

void TimeZoneChooser::keyReleaseEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Cancel)) {
        Q_EMIT cancelled();

        close();
    }
}

bool TimeZoneChooser::eventFilter(QObject *watched, QEvent *event)
{
    // Qt make popups' position 2px higher than the bottom of its associated widget,
    // and the 2px's hard coded, so I need to move the popup to a proper position so
    // it won't overlap with the SearchInput.
    if (watched == m_popup && event->type() == QEvent::Move) {
        const QMoveEvent *move = static_cast<QMoveEvent *>(event);
        const QPoint destPos = m_searchInput->mapToGlobal(QPoint(0, m_searchInput->height() + 1));
        // Don't panic, it won't cause dead loop.
        if (move->pos() != destPos) {
            m_popup->move(destPos);
        }
    }

    return false;
}

void TimeZoneChooser::showEvent(QShowEvent *event)
{
    QFrame::showEvent(event);

    move(qApp->primaryScreen()->geometry().center() - rect().center());
}

void TimeZoneChooser::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    return;
}

QSize TimeZoneChooser::getFitSize() const
{
    const QDesktopWidget *desktop = QApplication::desktop();
    const QRect primaryRect = desktop->availableGeometry(desktop->primaryScreen());

    double width = primaryRect.width() - 360/* dcc */ - 20 * 2;
    double height = primaryRect.height() - 70/* dock */ - 20 * 2;

    return QSize(static_cast<int>(width), static_cast<int>(height));
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

    const QSize fitSize = getFitSize();
    setFixedSize(fitSize.width(), fitSize.height());

    const float mapWidth = qMin(MapPixWidth, fitSize.width() - 20 * 2.0);
    const float mapHeight = qMin(MapPixHeight, fitSize.height() - 20 * 2/*paddings*/ - 36 * 2/*buttons*/ - 10/*button spacing*/ - 40 * 3.0 /*spacings*/ - 30/*title*/ -  20 * 2/*top bottom margin*/);
    const double widthScale = MapPictureWidth / mapWidth;
    const double heightScale = MapPictureHeight / mapHeight;
    const double scale = qMax(widthScale, heightScale);

    m_map->setFixedSize(MapPictureWidth / scale, MapPictureHeight / scale);

    m_searchInput->setMinimumSize(350, 36);
    m_cancelBtn->setMinimumSize(200, 36);
    m_confirmBtn->setMinimumSize(200, 36);
}
