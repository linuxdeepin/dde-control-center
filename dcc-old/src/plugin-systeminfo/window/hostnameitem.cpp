// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "hostnameitem.h"

#include "dtkwidget_global.h"

#include <dguiapplicationhelper.h>

#include <DDesktopServices>
#include <DLabel>
#include <DIconTheme>

#include <QDir>
#include <QEvent>
#include <QKeyEvent>
#include <QToolButton>
#include <QValidator>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

// if there is lid, it is laptop
inline bool islaptop()
{
    return QDir("/proc/acpi/button/lid").exists();
}

HostNameEdit::HostNameEdit(QWidget *parent)
    : DLineEdit(parent)
{
    lineEdit()->setAcceptDrops(false);
    lineEdit()->setContextMenuPolicy(Qt::NoContextMenu);
    lineEdit()->installEventFilter(this);
}

bool HostNameEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == lineEdit() && event->type() == QEvent::KeyPress) {
        QKeyEvent *e = dynamic_cast<QKeyEvent *>(event);
        if (e->matches(QKeySequence::Copy) || e->matches(QKeySequence::Cut)
            || e->matches(QKeySequence::Paste)) {
            return true;
        }

        if (e->key() >= 0x20 && e->key() <= 0x0a1) {
            // 首先判断键盘事件带的字符串是否为符合"^[A-Za-z0-9-]+$"规则
            QRegularExpression regx("^[A-Za-z0-9-]+$");
            QRegularExpressionValidator v(regx);
            QString text = e->text();
            int pos = 0;
            if (QValidator::Acceptable != v.validate(text, pos)) {
                DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
            }
        }
    }
    return DLineEdit::eventFilter(obj, event);
}

HostNameItem::HostNameItem(QWidget *parent)
    : SettingsItem(parent)
    , m_computerLabel(new QLabel(this))
    , m_hostNameLabel(new DLabel(this))
    , m_hostNameBtn(new QToolButton(this))
    , m_hostNameLineEdit(new HostNameEdit(this))
    , m_iconName{ islaptop() ? "icon_about_laptop" : "icon_about_pc" }
{
    initUI();
}

void HostNameItem::setHostName(const QString &name)
{
    m_hostname = name;
    const QString &text = getElidedText(name);
    m_hostNameLabel->setText(text);
}

void HostNameItem::onSetError(const QString &error)
{
    m_hostNameLineEdit->setVisible(true);
    m_hostNameLabel->setVisible(false);
    m_hostNameBtn->setVisible(false);
    m_hostNameLineEdit->setAlert(true);
    m_hostNameLineEdit->showAlertMessage(error, this);
    m_alertMessage = error;
    DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
}

void HostNameItem::resizeEvent(QResizeEvent *event)
{
    if (!m_hostNameLineEdit)
        return;

    if (m_hostNameLineEdit->isAlert()) {
        m_hostNameLineEdit->hideAlertMessage();
        m_hostNameLineEdit->showAlertMessage(m_alertMessage, this);
    }

    if (m_hostNameLineEdit->lineEdit() && !m_hostnameEdit.isEmpty()) {
        m_hostNameLineEdit->lineEdit()->setText(m_hostnameEdit);
        QString txt = getElidedText(m_hostnameEdit);
        m_hostNameLabel->setText(txt);
    }
    SettingsItem::resizeEvent(event);
}

void HostNameItem::initUI()
{
    // 添加主机名称
    QVBoxLayout *hostNameLayout = new QVBoxLayout(this);
    hostNameLayout->setSpacing(0);
    hostNameLayout->setMargin(0);
    m_computerLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    m_computerLabel->setPixmap(DIconTheme::findQIcon(m_iconName).pixmap(200, 200));

    hostNameLayout->addWidget(m_computerLabel);

    QHBoxLayout *editHostLayout = new QHBoxLayout;
    m_hostNameLabel->setForegroundRole(DPalette::TextTips);

    m_hostNameBtn->setIcon(DIconTheme::findQIcon("dcc_edit"));
    m_hostNameBtn->setIconSize(QSize(12, 12));
    m_hostNameBtn->setFixedSize(36, 36);

    QRegExp regx("^[A-Za-z0-9-]{0,64}$");
    QValidator *validator = new QRegExpValidator(regx, m_hostNameLineEdit);
    m_hostNameLineEdit->lineEdit()->setValidator(validator);
    m_hostNameLineEdit->setAlertMessageAlignment(Qt::AlignHCenter);
    m_hostNameLineEdit->lineEdit()->setAlignment(Qt::AlignHCenter);
    m_hostNameLineEdit->setFixedHeight(36);
    m_hostNameLineEdit->lineEdit()->setTextMargins(0, 0, 0, 0);
    m_hostNameLineEdit->hide();

    editHostLayout->addStretch();
    editHostLayout->addWidget(m_hostNameLabel);
    editHostLayout->addWidget(m_hostNameBtn);
    editHostLayout->addWidget(m_hostNameLineEdit);
    editHostLayout->addStretch();

    hostNameLayout->addLayout(editHostLayout);

    setContentsMargins(0, 0, 0, 30);
    // 点击编辑按钮
    connect(m_hostNameBtn, &QToolButton::clicked, this, &HostNameItem::onToolButtonButtonClicked);
    connect(m_hostNameLineEdit, &DLineEdit::focusChanged, this, &HostNameItem::onFocusChanged);
    connect(m_hostNameLineEdit, &DLineEdit::textEdited, this, &HostNameItem::onTextEdited);
    connect(m_hostNameLineEdit, &DLineEdit::alertChanged, this, &HostNameItem::onAlertChanged);
    connect(m_hostNameLineEdit->lineEdit(),
            &QLineEdit::editingFinished,
            this,
            &HostNameItem::onEditingFinished);
    connect(DGuiApplicationHelper::instance(),
            &DGuiApplicationHelper::themeTypeChanged,
            this,
            [this] {
                m_computerLabel->setPixmap(DIconTheme::findQIcon(m_iconName).pixmap(200, 200));
            });
}

QString HostNameItem::getElidedText(const QString &hostname)
{
    return m_hostNameLabel->fontMetrics().elidedText(hostname,
                                                     Qt::ElideRight,
                                                     m_computerLabel->width(),
                                                     0);
}

void HostNameItem::onEditingFinished()
{
    QString hostName = m_hostNameLineEdit->lineEdit()->text();
    if (hostName == m_hostname || hostName.simplified().isEmpty()) {
        m_hostNameLineEdit->lineEdit()->clearFocus();
        m_hostnameEdit.clear();
        m_hostNameLineEdit->setVisible(false);
        m_hostNameLabel->setVisible(true);
        m_hostNameBtn->setVisible(true);
        if (m_hostNameLineEdit->isAlert()) {
            m_hostNameLineEdit->setAlert(false);
            m_hostNameLineEdit->hideAlertMessage();
        }
        return;
    }

    if (!hostName.isEmpty()) {
        if ((hostName.startsWith('-') || hostName.endsWith('-')) && hostName.size() <= 63) {
            m_hostNameLineEdit->setAlert(true);
            m_hostNameLineEdit->showAlertMessage(tr("It cannot start or end with dashes"), this);
            m_alertMessage = tr("It cannot start or end with dashes");
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        } else {
            m_hostNameLineEdit->setAlert(false);
            m_hostNameLineEdit->hideAlertMessage();
        }

        if (!m_hostNameLineEdit->isAlert()) {
            m_hostNameLineEdit->lineEdit()->clearFocus();
            m_hostnameEdit.clear();
            m_hostNameLineEdit->setVisible(false);
            m_hostNameLabel->setVisible(true);
            m_hostNameBtn->setVisible(true);
            Q_EMIT hostNameChanged(hostName);
        }
    }
}

void HostNameItem::onAlertChanged()
{
    // 输入框保持透明背景
    QPalette palette = m_hostNameLineEdit->lineEdit()->palette();
    palette.setColor(QPalette::Button, Qt::transparent);
    m_hostNameLineEdit->lineEdit()->setPalette(palette);
}

void HostNameItem::onTextEdited(const QString &hostName)
{
    m_hostnameEdit = hostName;
    if (!hostName.isEmpty()) {
        if (hostName.size() > 63) {
            m_hostNameLineEdit->lineEdit()->backspace();
            m_hostNameLineEdit->setAlert(true);
            m_hostNameLineEdit->showAlertMessage(tr("1~63 characters please"), this);
            m_alertMessage = tr("1~63 characters please");
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        } else if (m_hostNameLineEdit->isAlert()) {
            m_hostNameLineEdit->setAlert(false);
            m_hostNameLineEdit->hideAlertMessage();
        }
    } else if (m_hostNameLineEdit->isAlert()) {
        m_hostNameLineEdit->setAlert(false);
        m_hostNameLineEdit->hideAlertMessage();
    }
}

void HostNameItem::onFocusChanged(const bool onFocus)
{
    QString hostName = m_hostNameLineEdit->lineEdit()->text();
    if (!onFocus && hostName.isEmpty()) {
        m_hostnameEdit.clear();
        m_hostNameLineEdit->setVisible(false);
        m_hostNameLabel->setVisible(true);
        m_hostNameBtn->setVisible(true);
    } else if (!onFocus && !hostName.isEmpty()) {
        if ((hostName.startsWith('-') || hostName.endsWith('-')) && hostName.size() <= 63) {
            m_hostNameLineEdit->setAlert(true);
            m_hostNameLineEdit->showAlertMessage(tr("It cannot start or end with dashes"), this);
            m_alertMessage = tr("It cannot start or end with dashes");
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        } else if (hostName.size() > 63) {
            m_hostNameLineEdit->setAlert(true);
            m_hostNameLineEdit->showAlertMessage(tr("1~63 characters please"), this);
            m_alertMessage = tr("1~63 characters please");
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        }
    }
}

void HostNameItem::onToolButtonButtonClicked()
{
    m_hostNameBtn->setVisible(false);
    m_hostNameLabel->setVisible(false);
    m_hostNameLineEdit->setVisible(true);
    m_hostNameLineEdit->setAlert(false);
    m_hostNameLineEdit->setText(m_hostname);
    m_hostNameLineEdit->hideAlertMessage();
    m_hostNameLineEdit->lineEdit()->setFocus();
    m_hostNameLineEdit->lineEdit()->selectAll();
}
