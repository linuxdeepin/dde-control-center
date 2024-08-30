//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "authenticationinfoitem.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

#include <DGuiApplicationHelper>
#include <DPaletteHelper>
#include <DIconButton>
#include <DLineEdit>
#include <DStyleHelper>
#include <DIconTheme>

using namespace DCC_NAMESPACE;

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

AuthenticationInfoItem::AuthenticationInfoItem(QWidget *parent)
    : SettingsItem(parent)
    , m_layout(new QHBoxLayout)
    , m_title(new QLabel)
    , m_removeBtn(new DIconButton(this))
    , m_editBtn(new DIconButton(this))
    , m_editTitle(new DLineEdit(this))
    , m_itemName("")
    , m_currentpa(DPaletteHelper::instance()->palette(this))
{
    setFixedHeight(36);

    m_editBtn->setIcon(DIconTheme::findQIcon("dcc_edit"));
    m_editBtn->setFlat(true);//设置背景透明
    m_editBtn->setVisible(false);

    m_editTitle->setClearButtonEnabled(false);
    m_editTitle->setVisible(false);
    m_editTitle->lineEdit()->setFrame(false);

    m_removeBtn->setFlat(true);
    m_removeBtn->setIcon(DStyle::StandardPixmap::SP_DeleteButton);
    m_removeBtn->setFixedSize(QSize(24, 24));
    m_removeBtn->setIconSize(QSize(16, 16));
    m_removeBtn->setVisible(false);
    DStyle::setFocusRectVisible(m_removeBtn, false);

    m_layout->setContentsMargins(10, 5, 10, 5);
    m_layout->addWidget(m_title, 0, Qt::AlignLeft);
    m_layout->addWidget(m_editBtn, 0, Qt::AlignLeft);
    m_layout->addWidget(m_editTitle, 0 , Qt::AlignLeft);
    m_layout->addStretch();
    m_layout->addWidget(m_removeBtn, 0, Qt::AlignVCenter);
    setLayout(m_layout);

    connect(m_removeBtn, &DIconButton::clicked, this, &AuthenticationInfoItem::removeClicked);
    connect(m_editBtn, &DIconButton::clicked, this, [this] {
        m_editBtn->hide();
        Q_EMIT editClicked(m_editTitle->isVisible());
        if (m_editTitle->isVisible()) {
            m_editTitle->lineEdit()->setText(m_title->text());
            m_editTitle->lineEdit()->selectAll();
            m_editTitle->lineEdit()->setFocus();
        }
    });
    connect(m_editTitle->lineEdit(), &QLineEdit::textChanged, this, [this] {
        m_editTitle->setAlert(false);
        m_editTitle->hideAlertMessage();
    });
    connect(m_editTitle->lineEdit(), &QLineEdit::editingFinished, this, [this] {
        if (onNameEditFinished()) {
            Q_EMIT editTextFinished(m_editTitle->text());
        }
        m_editTitle->lineEdit()->clearFocus();
        setEditTitle(false);
    });

    connect(Dtk::Gui::DGuiApplicationHelper::instance(), &Dtk::Gui::DGuiApplicationHelper::themeTypeChanged,
        this, [=](Dtk::Gui::DGuiApplicationHelper::ColorType themeType) {
        Q_UNUSED(themeType);
        DPaletteHelper::instance()->resetPalette(this);
        m_currentpa = DPaletteHelper::instance()->palette(this);
    });
}

void AuthenticationInfoItem::setTitle(const QString &title)
{
    title.isEmpty() ? m_layout->removeWidget(m_title) : m_title->setText(title);
    m_itemName = title;
}

void AuthenticationInfoItem::appendItem(QWidget *widget)
{
    m_layout->addWidget(widget, 0, Qt::AlignLeft);
}

void AuthenticationInfoItem::setShowIcon(bool state)
{
    m_removeBtn->setVisible(state);
}

void AuthenticationInfoItem::setEditTitle(bool state)
{
    m_title->setVisible(!state);
    m_editTitle->setVisible(state);
}

void AuthenticationInfoItem::setHideTitle(bool state)
{
    m_title->setVisible(state);
    m_editTitle->setVisible(state);
}

bool AuthenticationInfoItem::validateName(const QString &password)
{
    QString validate_policy = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") +
                              QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ") + QString("_");
    for (const QChar &p : password) {
        if (!validate_policy.contains(p)) {
            ushort uNum = p.unicode();
            // 片段非validate_policy中的字符是否为汉子
            if (uNum < 0x4E00 || uNum > 0x9FA5) {
                return false;
            }
        }
    }
    return true;
}

bool AuthenticationInfoItem::onNameEditFinished()
{
    QString editName = m_editTitle->lineEdit()->text();
    if (editName.isEmpty())
        return false;
    //正则表达式判断是否由字母、数字、中文、下划线组成
    bool regResult = editName.contains(QRegularExpression("(^[\\w\u4e00-\u9fa5]+$)"));
    if (editName.size() > 15) {
        QString errMsg = regResult ? tr("No more than 15 characters") : tr("Use letters, numbers and underscores only, and no more than 15 characters");
        showAlertMessage(errMsg);
        return false;
    } else {
        if (!regResult) {
            QString errMsg = tr("Use letters, numbers and underscores only");
            showAlertMessage(errMsg);
            return false;
        }
    }
    return true;
}

void AuthenticationInfoItem::showAlertMessage(const QString &errMsg)
{
    m_editTitle->setAlert(true);
    m_editTitle->showAlertMessage(errMsg, parentWidget()->parentWidget(), 2000);
    m_editTitle->lineEdit()->selectAll();
}

void AuthenticationInfoItem::enterEvent(QEvent *event)
{
    DPalette pa = DPaletteHelper::instance()->palette(this);
    DStyleHelper styleHelper;
    styleHelper = DStyleHelper(this->style());

    QBrush brush;
    if (styleHelper.dstyle()) {
        brush = styleHelper.dstyle()->generatedBrush(DStyle::SS_HoverState, pa.itemBackground(), DPalette::Normal, DPalette::ItemBackground);
    }
    pa.setBrush(DPalette::Window, Qt::transparent);
    pa.setBrush(DPalette::ItemBackground, brush);
    DPaletteHelper::instance()->setPalette(this, pa);

    if (m_editTitle->isVisible())
        m_editBtn->hide();
    else
        m_editBtn->show();

    QFrame::enterEvent(event);
}

void AuthenticationInfoItem::leaveEvent(QEvent *event)
{
    DPaletteHelper::instance()->setPalette(this, m_currentpa);
    m_editBtn->hide();
    QFrame::leaveEvent(event);
}

AuthenticationLinkButtonItem::AuthenticationLinkButtonItem(QWidget *parent)
    : SettingsItem(parent)
    , m_currentpa(DPaletteHelper::instance()->palette(this))
{
    setFixedHeight(36);

    connect(Dtk::Gui::DGuiApplicationHelper::instance(), &Dtk::Gui::DGuiApplicationHelper::themeTypeChanged,
        this, [=](Dtk::Gui::DGuiApplicationHelper::ColorType themeType) {
        Q_UNUSED(themeType);
        DPaletteHelper::instance()->resetPalette(this);
        m_currentpa = DPaletteHelper::instance()->palette(this);
    });
}

void AuthenticationLinkButtonItem::enterEvent(QEvent *event)
{
    DPalette pa = DPaletteHelper::instance()->palette(this);
    DStyleHelper styleHelper;
    styleHelper = DStyleHelper(this->style());

    QBrush brush;
    if (styleHelper.dstyle()) {
        brush = styleHelper.dstyle()->generatedBrush(DStyle::SS_HoverState, pa.itemBackground(), DPalette::Normal, DPalette::ItemBackground);
    }
    pa.setBrush(DPalette::Window, Qt::transparent);
    pa.setBrush(DPalette::ItemBackground, brush);
    DPaletteHelper::instance()->setPalette(this, pa);

    QFrame::enterEvent(event);
}

void AuthenticationLinkButtonItem::leaveEvent(QEvent *event)
{
    DPaletteHelper::instance()->setPalette(this, m_currentpa);
    QFrame::leaveEvent(event);
}

void AuthenticationLinkButtonItem::mousePressEvent(QMouseEvent *event)
{
    SettingsItem::mousePressEvent(event);

    Q_EMIT mousePressed();
}
