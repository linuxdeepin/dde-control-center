// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "jumpsettingbutton.h"
#include "xdgactivation.h"

#include <QHBoxLayout>
#include <QProcess>

#include <DFontSizeManager>
#include <DGuiApplicationHelper>
#include <DPlatformTheme>
#include <DPaletteHelper>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

JumpSettingButton::JumpSettingButton(QWidget *parent)
    : QFrame(parent)
    , m_hover(false)
    , m_autoShowPage(true)
    , m_iconButton(new CommonIconButton(this))
    , m_descriptionLabel(new DLabel(this))
{
    initUI();
}

JumpSettingButton::JumpSettingButton(const QIcon& icon, const QString& description, QWidget* parent)
    : QFrame(parent)
    , m_hover(false)
    , m_autoShowPage(true)
    , m_iconButton(new CommonIconButton(this))
    , m_descriptionLabel(new DLabel(this))
{
    initUI();

    m_iconButton->setIcon(icon);
    m_descriptionLabel->setText(description);
}

JumpSettingButton::~JumpSettingButton()
{

}

void JumpSettingButton::initUI()
{
    setFixedHeight(36);
    setForegroundRole(QPalette::BrightText);
    m_iconButton->setFixedSize(16, 16);
    m_iconButton->setForegroundRole(QPalette::BrightText);

    m_descriptionLabel->setElideMode(Qt::ElideRight);
    m_descriptionLabel->setForegroundRole(foregroundRole());
    DFontSizeManager::instance()->bind(m_descriptionLabel, DFontSizeManager::T6);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 0, 10, 0);
    layout->addWidget(m_iconButton);
    layout->addWidget(m_descriptionLabel);
    layout->addStretch();
}

void JumpSettingButton::setIcon(const QIcon &icon)
{
    m_iconButton->setIcon(icon, Qt::black, Qt::white);
}

void JumpSettingButton::setDescription(const QString& description)
{
    m_descriptionLabel->setText(description);
}

bool JumpSettingButton::event(QEvent* e)
{
    switch (e->type()) {
    case QEvent::Leave:
    case QEvent::Enter:
        m_hover = e->type() == QEvent::Enter;
        update();
        break;
    case QEvent::Hide:
        m_hover = false;
        update();
        break;
    default:
        break;
    }
    return QWidget::event(e);
}

void JumpSettingButton::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e)
    QPainter p(this);
    QPalette palette = this->palette();
    QColor bgColor, textColor;
    if (m_hover) {
        textColor = palette.highlightedText().color();
        bgColor = palette.color(QPalette::Active, QPalette::Highlight);
    } else {
        textColor = palette.brightText().color();
        bgColor = palette.brightText().color();
        bgColor.setAlphaF(0.05);
    }
    palette.setBrush(QPalette::BrightText, textColor);
    m_iconButton->setPalette(palette);
    m_descriptionLabel->setPalette(palette);

    p.setBrush(bgColor);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect(), 8, 8);
    return QFrame::paintEvent(e);
}

void JumpSettingButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (underMouse()) {
        Q_EMIT clicked();
        if (m_autoShowPage && !m_dccModule.isEmpty()) {
            auto *activation = new tray::XdgActivation(this);
            connect(activation, &tray::XdgActivation::tokenReady, this, [this, activation](const QString &token) {
                QStringList args {"--by-user", "org.deepin.dde.control-center"};
                if (!token.isEmpty())
                    args << "-e" << "XDG_ACTIVATION_TOKEN=" + token;
                args << "--" << "-p" << m_dccModule;
                QProcess::startDetached("dde-am", args);
                activation->deleteLater();
            }, Qt::SingleShotConnection);
            activation->requestToken();
            Q_EMIT showPageRequestWasSended();
        }
        return;
    }
    return QWidget::mouseReleaseEvent(event);
}


void JumpSettingButton::setDccModule(const QString &module)
{
    m_dccModule = module;
}
