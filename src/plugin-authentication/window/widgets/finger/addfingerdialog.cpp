//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "addfingerdialog.h"
#include "charamangermodel.h"

#include <DTitlebar>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCloseEvent>
#include <QTimer>
#include <QDebug>
#include <DSuggestButton>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

#define test false

AddFingerDialog::AddFingerDialog(const QString &thumb, QWidget *parent)
    : DAbstractDialog(parent)
    , m_timer(new QTimer(parent))
    , m_mainLayout(new QVBoxLayout(this))
    , m_btnHLayout(new QHBoxLayout(this))
    , m_fingeWidget(new FingerInfoWidget(this))
    , m_thumb(thumb)
    , m_cancelBtn(new QPushButton(this))
    , m_spaceWidget(new QWidget(this))
    , m_addBtn(new DTK_WIDGET_NAMESPACE::DSuggestButton(this))
{
    initWidget();
    initData();
    QWidget::installEventFilter(this);
}

AddFingerDialog::~AddFingerDialog()
{
}

void AddFingerDialog::initWidget()
{
    setFixedSize(QSize(382,446));
    m_mainLayout->setAlignment(Qt::AlignHCenter);

    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle("");

    m_mainLayout->addWidget(titleIcon, Qt::AlignTop | Qt::AlignRight);
    m_mainLayout->addSpacing(65); // UI 暂定 整体115
    m_mainLayout->addWidget(m_fingeWidget, 0, Qt::AlignTop | Qt::AlignHCenter);

    m_btnHLayout->addWidget(m_cancelBtn, Qt::AlignHorizontal_Mask);
    QHBoxLayout *btnHLayout = new QHBoxLayout(this);
    btnHLayout->addSpacing(20);
    m_spaceWidget->setMaximumWidth(20);
    m_spaceWidget->setLayout(btnHLayout);
    m_btnHLayout->addWidget(m_spaceWidget, Qt::AlignHorizontal_Mask);
    m_btnHLayout->addWidget(m_addBtn, Qt::AlignHorizontal_Mask);
    m_btnHLayout->setContentsMargins(10, 0, 10, 10);

    m_mainLayout->setSpacing(0);
    m_mainLayout->addLayout(m_btnHLayout);

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    setLayout(m_mainLayout);

    this->activateWindow();
    this->setFocus();
}

void AddFingerDialog::initData()
{
    m_cancelBtn->setText((tr("Cancel")));
    m_addBtn->setEnabled(false);
    m_addBtn->setVisible(false);
    m_spaceWidget->setVisible(false);
    connect(m_cancelBtn, &QPushButton::clicked, this, &AddFingerDialog::close);
    connect(m_addBtn, &DSuggestButton::clicked, this, [=] {
        auto text = m_addBtn->text();
        if (text == tr("Done")) {
            this->close();
        } else if (text == tr("Scan Again")) {
            Q_EMIT requestEnrollThumb();
        }
    });
}

void AddFingerDialog::setFingerModel(CharaMangerModel *model)
{
    m_model = model;
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &AddFingerDialog::enrollOverTime);
    connect(m_model, &CharaMangerModel::enrollCompleted, this, &AddFingerDialog::enrollCompleted);
    connect(m_model, &CharaMangerModel::enrollStagePass, this, &AddFingerDialog::enrollStagePass);
    connect(m_model, &CharaMangerModel::enrollFailed, this, &AddFingerDialog::enrollFailed);
    connect(m_model, &CharaMangerModel::enrollDisconnected, this, &AddFingerDialog::enrollDisconnected);
    connect(m_model, &CharaMangerModel::enrollRetry, this, &AddFingerDialog::enrollRetry);
    connect(m_model, &CharaMangerModel::lockedChanged, this, [=](bool locked) {
        if (locked) {
//            close();
        }
    });
    m_timer->start(1000 * 60);//1min
}

void AddFingerDialog::setUsername(const QString &name)
{
    m_username = name;
}

void AddFingerDialog::enrollCompleted()
{
    if (!m_isEnrolling) {
        return;
    }

    m_isEnrolling = false;
    m_fingeWidget->finished();
    m_addBtn->show();
    m_addBtn->setText(tr("Done"));
    m_addBtn->setEnabled(true);
    m_cancelBtn->setVisible(false);
    m_cancelBtn->setEnabled(false);
    m_spaceWidget->setVisible(false);
    m_timer->stop();
    Q_EMIT requestStopEnroll(m_username);
}

void AddFingerDialog::enrollStagePass(int pro)
{
    if (!m_isEnrolling) {
        return;
    }

    m_addBtn->setEnabled(false);
    m_fingeWidget->setProsses(pro);
    m_timer->start(1000 * 60);//1min
}

void AddFingerDialog::enrollFailed(QString title, QString msg)
{
    if (!m_isEnrolling) {
        return;
    }
    m_isEnrolling = false;
    m_fingeWidget->stopLiftTimer();
    m_fingeWidget->setStatueMsg(title, msg, true);
    m_addBtn->show();
    m_addBtn->setText(tr("Scan Again"));
    m_addBtn->setEnabled(true);
    m_spaceWidget->setVisible(true);
    m_timer->stop();

    Q_EMIT requestStopEnroll(m_username);
}
void AddFingerDialog::enrollDisconnected()
{
    Q_EMIT requestStopEnroll(m_username);

    m_isEnrolling = false;
    m_fingeWidget->setStatueMsg(tr("Scan Suspended"), tr("Scan Suspended"), true);
    m_addBtn->show();
    m_addBtn->setText(tr("Scan Again"));
    m_addBtn->setEnabled(true);
    m_spaceWidget->setVisible(false);
    m_timer->stop();

    //会出现末知情况，需要与后端确认中断时是否可以停止
    Q_EMIT requestStopEnroll(m_username);
}

void AddFingerDialog::enrollFocusOut()
{
    Q_EMIT requestStopEnroll(m_username);

    m_isEnrolling = false;
    m_fingeWidget->setStatueMsg(tr("Scan Suspended"), tr(""), true);
    m_addBtn->show();
    m_addBtn->setText(tr("Scan Again"));
    m_cancelBtn->setEnabled(false);
    m_addBtn->setEnabled(false);
    m_spaceWidget->setVisible(true);
    m_timer->stop();

    //会出现末知情况，需要与后端确认中断时是否可以停止
    Q_EMIT requestStopEnroll(m_username);
}

void AddFingerDialog::enrollOverTime()
{
    Q_EMIT requestStopEnroll(m_username);

    m_isEnrolling = false;
    m_fingeWidget->setStatueMsg(tr("Scan Suspended"), "", true);
    m_addBtn->show();
    m_addBtn->setText(tr("Scan Again"));
    m_addBtn->setEnabled(true);
    m_spaceWidget->setVisible(true);
    m_timer->stop();

    //会出现末知情况，需要与后端确认中断时是否可以停止
    Q_EMIT requestStopEnroll(m_username);
}

void AddFingerDialog::enrollRetry(QString title, QString msg)
{
    if (!m_isEnrolling) {
        return;
    }

    m_addBtn->setEnabled(false);
    m_timer->start(1000 * 60);//1min
    m_fingeWidget->setStatueMsg(title, msg, false);
}

void AddFingerDialog::setInitStatus()
{
    m_isEnrolling = true;
    m_addBtn->setEnabled(false);
    m_addBtn->setVisible(false);
    m_spaceWidget->setVisible(false);
    m_timer->start(1000 * 60);//1min
    m_fingeWidget->reEnter();
}

void AddFingerDialog::closeEvent(QCloseEvent *event)
{
    if (m_isEnrolling) {
        Q_EMIT requestStopEnroll(m_username);
    }
    Q_EMIT requesetCloseDlg(m_username);
    QDialog::closeEvent(event);
}

void AddFingerDialog::keyPressEvent(QKeyEvent *event)
{
//    switch (event->key()) {
//        case Qt::Key_Escape:
//            break;
//        default:
//            QDialog::keyPressEvent(event);
//            break;
//    }
    if (event->key() != Qt::Key_Escape) {
        QDialog::keyPressEvent(event);
    }
}

bool AddFingerDialog::eventFilter(QObject *o, QEvent *e)
{
    if (o == this) {
       if (QEvent::WindowDeactivate == e->type()) {
            clearFocus();
            if (m_isEnrolling) {
                enrollFocusOut();
                QTimer::singleShot(1000, this, [=] {
                    m_cancelBtn->setEnabled(true);
                    m_addBtn->setEnabled(true);
                });
            }
            setFocus();
            return true ;
       }
    }
    return false ;
}
