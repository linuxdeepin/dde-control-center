/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#include "faceinfodialog.h"

#include <DFontSizeManager>
#include <DTitlebar>

#include <QBoxLayout>
#include <QElapsedTimer>
#include <QLabel>
#include <QObject>
#include <QPointer>
#include <QTimer>

DWIDGET_USE_NAMESPACE

FaceInfoDialog::FaceInfoDialog(CharaMangerModel *model, QWidget *parent)
    : DAbstractDialog(parent)
    , m_faceModel(model)
    , m_faceLabel(new FaceInfoWidget(this))
    , m_mainLayout(new QVBoxLayout(this))
{
    initWidget();

    connect(m_faceModel, &CharaMangerModel::enrollInfoState, this, &FaceInfoDialog::close);
    connect(m_faceModel,
            &CharaMangerModel::enrollStatusTips,
            this,
            &FaceInfoDialog::refreshExplainTips);

    QWidget::installEventFilter(this);
}

FaceInfoDialog::~FaceInfoDialog() { }

void FaceInfoDialog::initWidget()
{
    setFixedSize(QSize(454, 542));
    m_mainLayout->setAlignment(Qt::AlignHCenter);

    DTitlebar *titleIcon = new DTitlebar(this);
    titleIcon->setFrameStyle(QFrame::NoFrame); // 无边框
    titleIcon->setBackgroundTransparent(true); // 透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr("Enroll Face"));

    m_mainLayout->addWidget(titleIcon, Qt::AlignTop | Qt::AlignRight);
    m_mainLayout->addSpacing(60);
    m_mainLayout->addWidget(m_faceLabel, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(30);

    // 提示语
    m_explainTips = new QLabel(tr("Position your face inside the frame"));
    m_explainTips->setWordWrap(true);
    m_explainTips->setAlignment(Qt::AlignTop);
    DFontSizeManager::instance()->bind(m_explainTips, DFontSizeManager::T6);
    m_mainLayout->addWidget(m_explainTips, 0, Qt::AlignHCenter);

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    setLayout(m_mainLayout);

    this->activateWindow();
    this->setFocus();
}

void FaceInfoDialog::refreshExplainTips(QString tips)
{
    m_explainTips->setText(tips);
}

void FaceInfoDialog::closeEvent(QCloseEvent *event)
{
    Q_EMIT requestCloseDlg();
    QDialog::closeEvent(event);
}

bool FaceInfoDialog::eventFilter(QObject *o, QEvent *e)
{
    if (o == this && QEvent::WindowDeactivate == e->type()) {
        clearFocus();
        setFocus();
        return true;
    }
    return false;
}
