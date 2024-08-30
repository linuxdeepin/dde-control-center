//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "cooperationsettingsdialog.h"

#include <DTitlebar>
#include <DFontSizeManager>
#include <DFileChooserEdit>
#include <DSwitchButton>

#include <QEvent>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

CooperationSettingsDialog::CooperationSettingsDialog(QWidget *parent)
    : DAbstractDialog(parent)
    , m_mainLayout(new QVBoxLayout(this))
    , m_mousekeyboardSwitch(new SwitchWidget(this))
    , m_shearSwitch(new SwitchWidget(this))
    , m_storageItem(new FileChooseWidget(this))
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save, this))
{
    initWidget();
    initConnect();
    QWidget::installEventFilter(this);
    m_storageItem->edit()->lineEdit()->installEventFilter(this);
}

CooperationSettingsDialog::~CooperationSettingsDialog()
{

}

void CooperationSettingsDialog::onSetWindowEnabled(const bool isEnabled)
{
    this->setEnabled(isEnabled);
}

void CooperationSettingsDialog::setOpenSharedDevices(bool on)
{
    m_mousekeyboardSwitch->setChecked(on);
}

void CooperationSettingsDialog::setOpenSharedClipboard(bool on)
{
    m_shearSwitch->setChecked(on);
}

void CooperationSettingsDialog::setFilesStoragePath(const QString &path)
{
    m_storagePath = path;
    m_storageItem->edit()->setText(path);
}

void CooperationSettingsDialog::closeEvent(QCloseEvent *event)
{
    QDialog::closeEvent(event);
}

bool CooperationSettingsDialog::eventFilter(QObject *o, QEvent *e)
{
    // 实现鼠标点击编辑框，确定按钮激活，统一网络模块处理，捕捉FocusIn消息
    if (e->type() == QEvent::FocusIn) {
        if (dynamic_cast<QLineEdit *>(o)) {
            setButtonEnabled();
        }
    }

    if (o == this && QEvent::WindowDeactivate == e->type()) {
        clearFocus();
        setFocus();
        return true;
    }
    return false;
}

void CooperationSettingsDialog::initWidget()
{
    setFixedSize(QSize(480, 362));
//    m_mainLayout->setAlignment(Qt::AlignVCenter);

    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr("Collaboration Settings"));

    // 共享鼠标键盘
    m_mousekeyboardSwitch->setTitle(tr("Share mouse and keyboard"));
    m_mousekeyboardSwitch->addBackground();
    m_mousekeyboardSwitch->setContentsMargins(10, 0, 10, 0);

    // 提示信息
    m_mousekeyboardTips = new DTipLabel(tr("Share your mouse and keyboard across devices"), this);
    m_mousekeyboardTips->setForegroundRole(DPalette::TextTips);
    m_mousekeyboardTips->setWordWrap(true);
    m_mousekeyboardTips->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_mousekeyboardTips->adjustSize();
    m_mousekeyboardTips->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_mousekeyboardTips->setContentsMargins(20, 0, 0, 0);

    // 剪切板共享
    m_shearSwitch->setTitle(tr("Share clipboard"));
    m_shearSwitch->addBackground();
    m_shearSwitch->setContentsMargins(10, 0, 10, 0);

    // 共享存储
    m_storageItem->setTitle(tr("Storage path for shared files"));
    m_storageItem->edit()->setDialogDisplayPosition(DFileChooserEdit::DialogDisplayPosition::FollowParentWindow);
    m_storageItem->setContentsMargins(10, 0, 10, 0);
    m_storageTips = new DTipLabel(tr("Share the copied content across devices"), this);
    m_storageTips->setForegroundRole(DPalette::TextTips);
    m_storageTips->setWordWrap(true);
    m_storageTips->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_storageTips->adjustSize();
    m_storageTips->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_storageTips->setContentsMargins(20, 0, 0, 0);

    // 下方按钮
    QPushButton *cancelBtn = m_buttonTuple->leftButton();
    QPushButton *acceptBtn = m_buttonTuple->rightButton();

    cancelBtn->setText(tr("Cancel", "button"));
    acceptBtn->setText(tr("Confirm", "button"));
    setButtonDisabled();
    m_buttonTuple->setContentsMargins(75, 0, 75, 0);

    m_mainLayout->addWidget(titleIcon, Qt::AlignTop);
    m_mainLayout->addSpacing(20);
    m_mainLayout->addWidget(m_mousekeyboardSwitch, 0, Qt::AlignVCenter);
    m_mainLayout->addWidget(m_mousekeyboardTips, 0, Qt::AlignVCenter);
    m_mainLayout->addSpacing(15);

    m_mainLayout->addWidget(m_shearSwitch, 0, Qt::AlignVCenter);
    m_mainLayout->addSpacing(15);
    m_mainLayout->addWidget(m_storageItem, 0, Qt::AlignVCenter);
    m_mainLayout->addWidget(m_storageTips, 0, Qt::AlignVCenter);
    m_mainLayout->addSpacing(30);

    m_mainLayout->addWidget(m_buttonTuple);
    m_mainLayout->addSpacing(10);

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(10, 0, 10, 0);
    setLayout(m_mainLayout);

    this->activateWindow();
    this->setFocus();
}

void CooperationSettingsDialog::initConnect()
{
    connect(m_storageItem->edit()->lineEdit(), &QLineEdit::textChanged, [this](const QString& filePath){
        if (filePath != m_storagePath) {
            setFilesStoragePath(filePath);
        }
        setButtonTupleState(true);
    });
    connect(m_shearSwitch, &SwitchWidget::checkedChanged, this, &CooperationSettingsDialog::setButtonEnabled);
    connect(m_mousekeyboardSwitch, &SwitchWidget::checkedChanged, this, &CooperationSettingsDialog::setButtonEnabled);
    connect(m_buttonTuple->leftButton(), &QPushButton::clicked, this, &CooperationSettingsDialog::close);
    connect(m_buttonTuple->rightButton(), &QPushButton::clicked, this, &CooperationSettingsDialog::accept);
}

void CooperationSettingsDialog::setButtonTupleState(bool state)
{
    m_buttonTuple->rightButton()->setChecked(state);
}

void CooperationSettingsDialog::setButtonEnabled()
{
    m_buttonTuple->leftButton()->setEnabled(true);
    m_buttonTuple->rightButton()->setEnabled(true);
}

void CooperationSettingsDialog::setButtonDisabled()
{
    m_buttonTuple->leftButton()->setDisabled(true);
    m_buttonTuple->rightButton()->setDisabled(true);
}
