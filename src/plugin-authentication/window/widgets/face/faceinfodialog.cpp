//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#include "faceinfodialog.h"

#include <DFontSizeManager>
#include <DTitlebar>

#include <QObject>
#include <QBoxLayout>
#include <QElapsedTimer>
#include <QPointer>
#include <QTimer>
#include <QLabel>

DWIDGET_USE_NAMESPACE

FaceInfoDialog::FaceInfoDialog(CharaMangerModel *model, QWidget *parent)
    : DAbstractDialog(parent)
    , m_faceModel(model)
    , m_faceLabel(new FaceInfoWidget(this))
    , m_mainLayout(new QVBoxLayout(this))
{
    initWidget();

    connect(m_faceModel, &CharaMangerModel::enrollInfoState, this, &FaceInfoDialog::close);
    connect(m_faceModel, &CharaMangerModel::enrollStatusTips, this, &FaceInfoDialog::refreshExplainTips);

    QWidget::installEventFilter(this);
}

FaceInfoDialog::~FaceInfoDialog()
{
}

void FaceInfoDialog::initWidget()
{
    setFixedSize(QSize(454, 542));
    m_mainLayout->setAlignment(Qt::AlignHCenter);

    DTitlebar *titleIcon = new DTitlebar(this);
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
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


