/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
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

#include "fingerwidget.h"

#include <DHiDPIHelper>
#include <DApplicationHelper>

#include <QVBoxLayout>
#include <QDebug>
#include <QTimer>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::accounts;

FingerWidget::FingerWidget(QWidget *parent)
    : QWidget(parent)
    , m_view(new DPictureSequenceView)
    , m_tipLbl(new QLabel(this))
    , m_titleLbl(new TitleLabel(this))
    , m_isFinished(false)
    , m_titleTimer(new QTimer(this))
    , m_msgTimer(new QTimer(this))
{
    m_defTitle = tr("Place your finger");
    m_titleTimer->setSingleShot(true);
    m_titleTimer->setInterval(1000);
    m_msgTimer->setSingleShot(true);
    m_msgTimer->setInterval(2000);
    connect(m_titleTimer, &QTimer::timeout, this, [this]{
        m_titleLbl->setText(m_defTitle);
    });
    connect(m_msgTimer, &QTimer::timeout, this, [this]{
        m_tipLbl->setText(m_defTip);
    });

//    QString theme;
    DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::instance()->themeType();
    switch (type) {
    case DGuiApplicationHelper::UnknownType:
        break;
    case DGuiApplicationHelper::LightType:
        m_theme = QString("light");
        break;
    case DGuiApplicationHelper::DarkType:
        m_theme = QString("dark");
        break;
    }

    for(uint i = 0; i != 58; i++)
    {
        QString path = QString(":/accounts/themes/%1/icons/finger/entering/fingerprint_%2.png").arg(m_theme).arg(i, 2, 10, QChar('0'));
        m_enteringList << path;
    }

    for(uint i = 0; i != 30; i++)
    {
        QString path = QString(":/accounts/themes/%1/icons/finger/finished/success_%2.png").arg(m_theme).arg(i, 2, 10, QChar('0'));
        m_finishedList << path;
    }

    m_view->setPictureSequence(m_enteringList);
    m_view->setSingleShot(true);

    m_tipLbl->setWordWrap(true);
    m_tipLbl->setMinimumHeight(80);

    QVBoxLayout *layout = new QVBoxLayout;

    layout->addSpacing(80);
    layout->addWidget(m_view, 0, Qt::AlignCenter);
    layout->addWidget(m_titleLbl, 0, Qt::AlignHCenter);
    layout->addWidget(m_tipLbl, 0, Qt::AlignHCenter);
    setLayout(layout);

    connect(m_view, &DPictureSequenceView::playEnd, this, [=] {
        if (m_isFinished)
            m_view->setPictureSequence(QStringList() << QString(":/accounts/themes/%1/icons/finger/finished/success_30.png").arg(m_theme));
        else
            Q_EMIT playEnd();
    });
    setProsses(0);
}

void FingerWidget::setStatueMsg(const QString &title, const QString &msg, bool reset)
{
    m_reset = reset;
    m_msgTimer->stop();
    m_titleTimer->stop();

    m_titleLbl->setText(title);
    m_tipLbl->setText(msg);

    if (reset) {
        m_msgTimer->start();
        m_titleTimer->start();
         m_view->setPictureSequence(QStringList() << QString(":/accounts/themes/%1/icons/finger/finished/success_00.png").arg(m_theme));
    }
}

void FingerWidget::setProsses(int pro)
{
    m_pro = pro;
    m_view->setPictureSequence(QStringList() <<QString(":/accounts/themes/%1/icons/finger/finished/success_%2.png")
                               .arg(m_theme).arg(m_pro/3, 2, 10, QChar('0')));

    if (pro > 35) {
        m_defTip = tr("Place your finger firmly on the sensor until you're asked to lift it");
    } else {
        m_defTip = tr("Place the edges of your fingerprint on the sensor");
    }
    m_msgTimer->stop();
    m_titleTimer->stop();
    m_titleLbl->setText(m_defTitle);
    m_tipLbl->setText(m_defTip);

    if(m_pro == 0)
        next();
}

void FingerWidget::reEnter()
{
    m_isFinished = false;

    setProsses(0);
    m_view->setPictureSequence(m_enteringList);
}

void FingerWidget::next()
{
    m_isFinished = false;
    m_view->setPictureSequence(m_enteringList);
    m_view->play();
}

void FingerWidget::finished()
{
    m_isFinished = true;

    setStatueMsg(tr("Fingerprint added"), tr(""));
    m_view->setPictureSequence(m_finishedList);
    m_view->play();
}

void FingerWidget::paintEvent(QPaintEvent *event)
{
//    QWidget::paintEvent(event);
//    QPainter p(this);
//    p.setPen(Qt::NoPen);
//    if (!m_reset) {
//        p.setBrush(QColor(197, 220, 243));
//        p.drawRect(rect());
//    }
//    update();

//    QPainter painter(this);
//    QPainterPath path;

//    path.addRoundedRect(rect(), 5, 5);
//    painter.fillPath(path, QColor(255, 255, 255, 0.2 * 255));
}
