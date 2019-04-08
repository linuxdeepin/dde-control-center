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

#include "addfingerpage.h"
#include "widgets/translucentframe.h"
#include <QVBoxLayout>
#include <QDebug>

using namespace dcc;
using namespace dcc::accounts;
using namespace dcc::widgets;

AddFingerPage::AddFingerPage(const QString &thumb, QWidget *parent)
    : ContentWidget(parent)
    , m_buttonTuple(new ButtonTuple)
    , m_fingerWidget(new FingerWidget)
    , m_frequency(1)
    , m_thumb(thumb)
{
    setTitle(tr("Add Fingerprint"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addSpacing(20);

    layout->addWidget(m_fingerWidget);
    layout->addSpacing(10);
    layout->addWidget(m_buttonTuple);
    layout->addStretch();

    m_buttonTuple->leftButton()->setText(tr("Re-enter"));
    m_buttonTuple->rightButton()->setText(tr("Done"));

    m_buttonTuple->setVisible(false);

    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(layout);

    setContent(w);

    connect(m_buttonTuple->leftButton(), &QPushButton::clicked, this, &AddFingerPage::reEnrollStart);
    connect(m_buttonTuple->rightButton(), &QPushButton::clicked, this, &AddFingerPage::saveThumb);
    connect(m_fingerWidget, &FingerWidget::playEnd, this, &AddFingerPage::onViewPlayEnd);
}

void AddFingerPage::setFingerModel(FingerModel *model)
{
    m_model = model;

    connect(model, &FingerModel::enrollStatusChanged, this, &AddFingerPage::onEnrollStatusChanged);
    onEnrollStatusChanged(model->enrollStatus());
}

void AddFingerPage::setUsername(const QString &name)
{
    m_username = name;
}

void AddFingerPage::saveThumb()
{
    Q_EMIT requestSaveThumb(m_username);

    Q_EMIT back();
}

void AddFingerPage::reEnrollStart()
{
    m_buttonTuple->setVisible(false);

    Q_EMIT requestReEnrollStart(m_thumb);

    m_fingerWidget->reEnter();
}

void AddFingerPage::onEnrollStatusChanged(FingerModel::EnrollStatus status)
{
    if (status == FingerModel::EnrollStatus::Ready) {
        onViewPlayEnd();
        return;
    }

    if (status == FingerModel::EnrollStatus::Next) {
        m_fingerWidget->next();
        m_fingerWidget->setFrequency(tr("Identifying fingerprint"));
        ++m_frequency;
        return;
    }

    if (status == FingerModel::EnrollStatus::Retry) {
        m_fingerWidget->setFrequency(tr("Failed to identify fingerprint, place your finger on the fingerprint reader, swipe upwards and release"));
        return;
    }

    if (status == FingerModel::EnrollStatus::Finished) {
        m_fingerWidget->finished();
        m_fingerWidget->setFrequency(tr("Added successfully"));
        m_buttonTuple->setVisible(true);

        m_frequency = 1;
        return;
    }
}

void AddFingerPage::onViewPlayEnd()
{
    m_fingerWidget->setFrequency(tr("Place your finger on the fingerprint reader, swipe upwards, then repeat"));
}
