//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "shortcutitem.h"
#include "operation/shortcutmodel.h"
#include "keylabel.h"

#include <DStyle>
#include <DIconTheme>

#include <QLabel>
#include <QMouseEvent>
#include <QLineEdit>
#include <QTimer>
#include <QApplication>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

ShortcutItem::ShortcutItem(QFrame *parent)
    : SettingsItem(parent)
    , m_info(nullptr)
{
    installEventFilter(this);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    setMinimumHeight(36);

    setMouseTracking(true);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(10, 2, 10, 2);
    layout->setSpacing(2);

    m_title = new QLabel();
    m_title->setText("");
    m_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_title->setWordWrap(false);
    m_title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    layout->addWidget(m_title);
    layout->setAlignment(m_title, Qt::AlignLeft);

    m_editBtn = new DIconButton(this);
    m_editBtn->setIcon(DIconTheme::findQIcon("dcc_edit"));
    m_editBtn->hide();
    m_editBtn->setFixedSize(16, 16);
    m_editBtn->setAccessibleName("EDIT_SHORTCUT_BUTTON");
    layout->addWidget(m_editBtn, Qt::AlignLeft);

    layout->addStretch();

    m_delBtn = new DIconButton(this);
    m_delBtn->setIcon(DStyle::StandardPixmap::SP_DeleteButton);
    m_delBtn->hide();
    m_delBtn->setFixedSize(16, 16);
    m_delBtn->setAccessibleName("DEL_SHORTCUT_BUTTON");
    layout->addWidget(m_delBtn, Qt::AlignLeft);

    m_key = new ShortcutKey;
    layout->addWidget(m_key);

    m_shortcutEdit = new QLineEdit;
    m_shortcutEdit->setReadOnly(true);
    layout->addWidget(m_shortcutEdit, 0, Qt::AlignVCenter | Qt::AlignRight);

    m_shortcutEdit->setPlaceholderText(tr("Enter a new shortcut"));
    QFontMetrics fm = m_shortcutEdit->fontMetrics();
    QRect strRect = fm.boundingRect(m_shortcutEdit->placeholderText());
    QStyleOptionFrame opt;
    initStyleOption(&opt);
    int strWidth = (style()->sizeFromContents(QStyle::CT_LineEdit, &opt, strRect.size().
                                           expandedTo(QApplication::globalStrut()), m_shortcutEdit)).width();
    int borderWidth = m_shortcutEdit->minimumSizeHint().width() - fm.maxWidth();

    m_shortcutEdit->setMinimumWidth(strWidth + borderWidth + 8);  //始终有些误差
    m_shortcutEdit->hide();

    setLayout(layout);

    connect(m_editBtn, &DIconButton::clicked, this, &ShortcutItem::onShortcutEdit);
    connect(m_delBtn, &DIconButton::clicked, this, &ShortcutItem::onRemoveClick);
}

ShortcutItem::~ShortcutItem()
{
//    GSettingWatcher::instance()->erase(configName(), this);
}

void ShortcutItem::setShortcutInfo(ShortcutInfo *info)
{
    m_info = info;
    setTitle(m_info->name);
    setShortcut(info->accels);

    if (info->type != ShortcutModel::Custom) {
        setConfigName(info->id + "Config");
//        GSettingWatcher::instance()->bind(configName(), this);
    }
}

void ShortcutItem::setTitle(const QString &title)
{
    m_title->setText(title);
    QTimer::singleShot(0, this, &ShortcutItem::updateTitleSize);
}

void ShortcutItem::setShortcut(const QString &shortcut)
{
    m_shortcutEdit->hide();
    m_key->show();

    QString accels = shortcut;
    accels = accels.replace("<", "");
    accels = accels.replace(">", "-");
    accels = accels.replace("_L", "");
    accels = accels.replace("_R", "");
    accels = accels.replace("Control", "Ctrl");

    m_key->setTextList(accels.split("-"));
    QTimer::singleShot(0, this, &ShortcutItem::updateTitleSize);
}

void ShortcutItem::onEditMode(bool value)
{
    if (value) {
        m_delBtn->show();
        m_editBtn->show();
        m_key->hide();
    } else {
        m_delBtn->hide();
        m_editBtn->hide();
        m_key->show();
    }
    update();
}

void ShortcutItem::onRemoveClick()
{
    Q_EMIT requestRemove(m_info);
}

void ShortcutItem::onShortcutEdit()
{
    Q_EMIT shortcutEditChanged(m_info);
}

void ShortcutItem::updateTitleSize()
{
    show();
    if (m_info->name.isEmpty()) return;
    int v = 0;
    if(m_shortcutEdit->isHidden())
        v = width() - m_key->width() - 32;
    else
        v = width() - m_shortcutEdit->width() - 32;
    if (m_title->fontMetrics().horizontalAdvance(m_info->name) > v) {
        QFontMetrics fontWidth(m_title->font());
        QString elideNote = fontWidth.elidedText(m_info->name, Qt::ElideRight, v);
        m_title->setText(elideNote);
    }
    else {
        m_title->setText(m_info->name);
    }
}

void ShortcutItem::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_delBtn->isVisible())
        return;

    if (!m_shortcutEdit->isVisible() && m_key->rect().contains(m_key->mapFromParent(e->pos()))) {
        m_key->hide();
        m_shortcutEdit->show();
        m_info->item = this;
        m_shortcutEdit->setFocus();

        Q_EMIT requestUpdateKey(m_info);

    } else {
        m_shortcutEdit->hide();
        m_key->show();
    }
    updateTitleSize();
}

void ShortcutItem::resizeEvent(QResizeEvent *event)
{
    SettingsItem::resizeEvent(event);

    int v = width() - m_key->width() - 32;
    // 参与计算的是实际文本长度
    if (m_title->fontMetrics().horizontalAdvance(m_info->name) <= v) {
        m_title->setText(m_info->name);
        m_title->setToolTip("");
    } else {
        m_title->setToolTip(m_info->name);
        QTimer::singleShot(0, this, &ShortcutItem::updateTitleSize);
    }

}

bool ShortcutItem::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Show) {
        if (m_info->type != ShortcutModel::Custom) {
//            setVisible("Hidden" != GSettingWatcher::instance()->getStatus(configName()));
        }
    }

    return QObject::eventFilter(watched, event);
}

QString ShortcutItem::configName() const
{
    QString configName = m_configName;
    for (int i = 0; i < configName.size(); i++) {
        if (configName[i] == "-") {
            QChar upperChar = configName.at(i + 1).toUpper();
            configName.remove(i, 2);
            configName.insert(i, upperChar);
        }
    }
    return configName;
}

void ShortcutItem::setConfigName(const QString &configName)
{
    m_configName = configName;
}
