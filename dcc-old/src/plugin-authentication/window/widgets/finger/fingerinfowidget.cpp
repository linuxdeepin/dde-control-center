//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "fingerinfowidget.h"

#include <DHiDPIHelper>
#include <DGuiApplicationHelper>

#include <QVBoxLayout>
#include <QDebug>
#include <QTimer>
#include <QFont>

DWIDGET_USE_NAMESPACE

FingerInfoWidget::FingerInfoWidget(QWidget *parent)
    : QWidget(parent)
    , m_view(new DPictureSequenceView)
    , m_tipLbl(new QLabel(this))
    , m_titleLbl(new DCC_NAMESPACE::TitleLabel(this))
    , m_isFinished(false)
    , m_titleTimer(new QTimer(this))
    , m_msgTimer(new QTimer(this))
    , m_liftTimer(new QTimer(this))
{
    m_titleTimer->setSingleShot(true);
    m_titleTimer->setInterval(2000);
    m_msgTimer->setSingleShot(true);
    m_msgTimer->setInterval(2000);
    m_liftTimer->setSingleShot(true);
    m_liftTimer->setInterval(1000);
    connect(m_titleTimer, &QTimer::timeout, this, [this] {
        m_titleLbl->setText(m_defTitle);
    });
    connect(m_msgTimer, &QTimer::timeout, this, [this] {
        m_tipLbl->setText(m_defTip);
    });
    connect(m_liftTimer, &QTimer::timeout, this, [this] {
        if (m_pro > 0 && m_pro < 35) {
            m_defTitle = tr("Place your finger");
            m_defTip = tr("Place your finger firmly on the sensor until you're asked to lift it");
        } else if (m_pro >= 35 && m_pro < 100) {
            m_defTitle = tr("Scan the edges of your fingerprint");
            m_defTip = tr("Place the edges of your fingerprint on the sensor");
        } else {
            m_liftTimer->stop();
            return;
        }

        m_titleLbl->setText(m_defTitle);
        m_tipLbl->setText(m_defTip);
    });

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

    m_view->setSingleShot(true);

    m_titleLbl->setAlignment(Qt::AlignCenter);
    m_tipLbl->setWordWrap(true);
    m_tipLbl->setAlignment(Qt::AlignCenter);

    QVBoxLayout *layout = new QVBoxLayout;
    m_view->setMinimumHeight(90);
    layout->addWidget(m_view, 0, Qt::AlignTop);
    layout->addSpacing(20);
    layout->addWidget(m_titleLbl, 0, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(m_tipLbl, 0, Qt::AlignHCenter);
    setLayout(layout);

    setProsses(0);
}

void FingerInfoWidget::setStatueMsg(const QString &title, const QString &msg, bool reset)
{
    m_reset = reset;
    m_msgTimer->stop();
    m_titleTimer->stop();

    m_titleLbl->setText(title);
    m_tipLbl->setText(msg);

    if (!m_reset) {
        m_msgTimer->start();
        m_titleTimer->start();

        if (m_pro == 0) {
            m_view->setPictureSequence(QStringList() << QString(":/icons/deepin/builtin/icons/%1/icons/finger/fingerprint_light.svg").arg(m_theme));
        } else {
            m_view->setPictureSequence(QStringList() << QString(":/icons/deepin/builtin/icons/%1/icons/finger/fingerprint_animation_light_%2.svg")
                                       .arg(m_theme).arg(m_pro/2));
        }
    }
}

void FingerInfoWidget::setProsses(int pro)
{
    m_pro = pro;
    if (m_pro == 0) {
        m_isStageOne = true;
        m_view->setPictureSequence(QStringList() << QString(":/icons/deepin/builtin/icons/%1/icons/finger/fingerprint_light.svg").arg(m_theme));
        m_defTitle = tr("Place your finger");
        m_defTip = tr("Place your finger firmly on the sensor until you're asked to lift it");
    } else {
        int idx = m_pro / 2;
        idx = idx > 50 ? 50 : idx;
        m_view->setPictureSequence(QStringList() << QString(":/icons/deepin/builtin/icons/%1/icons/finger/fingerprint_animation_light_%2.svg")
                                   .arg(m_theme).arg(idx));
        if (m_pro > 0 && m_pro < 35) {
            m_defTitle = tr("Lift your finger");
            m_defTip = tr("Lift your finger and place it on the sensor again");
            m_liftTimer->start();
        } else if (m_pro >= 35 && m_pro < 100) {
            if (m_isStageOne == true) {
                m_isStageOne = false;
                m_defTitle = tr("Scan the edges of your fingerprint");
                m_defTip = tr("Adjust the position to scan the edges of your fingerprint");
            } else {
                m_defTitle = tr("Scan the edges of your fingerprint");
                m_defTip = tr("Lift your finger and do that again");
                m_liftTimer->start();
            }
        } else {
            m_defTitle = tr("Fingerprint added");
            m_defTip = tr("");
        }
    }
    m_msgTimer->stop();
    m_titleTimer->stop();
    m_titleLbl->setText(m_defTitle);
    auto font = m_titleLbl->font();
    font.setWeight(QFont::DemiBold);
    m_titleLbl->setFont(font);
    m_tipLbl->setText(m_defTip);
}

void FingerInfoWidget::reEnter()
{
    m_isFinished = false;

    setProsses(0);
}

void FingerInfoWidget::finished()
{
    m_isFinished = true;
    stopLiftTimer();
    setProsses(100);
}

void FingerInfoWidget::stopLiftTimer()
{
    if (nullptr != m_liftTimer && m_liftTimer->isActive())
        m_liftTimer->stop();
}
