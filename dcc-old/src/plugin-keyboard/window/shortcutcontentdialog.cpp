//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "shortcutcontentdialog.h"
#include "operation/shortcutmodel.h"
#include "shortcutitem.h"

#include <DTitlebar>

#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
ShortcutContentDialog::ShortcutContentDialog(ShortcutModel *model, QWidget *parent)
    : DAbstractDialog(parent)
    , m_model(model)
    , m_conflict(nullptr)
    , m_info(nullptr)
    , m_shortcutItem(new ShortcutItem)
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save))
{
    setFixedSize(QSize(400, 300));
    QVBoxLayout *mainVLayout = new QVBoxLayout();
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(0);

    QVBoxLayout *listVLayout = new QVBoxLayout();
    listVLayout->setAlignment(Qt::AlignHCenter);
    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr(""));

    mainVLayout->addWidget(titleIcon);

    m_shortcutItem->setShortcut(tr("Please Reset Shortcut"));
    m_shortcutItem->addBackground();
    listVLayout->addWidget(m_shortcutItem, 0, Qt::AlignTop);

    listVLayout->addSpacing(10);
    QPushButton *cancel = m_buttonTuple->leftButton();
    QPushButton *ok = m_buttonTuple->rightButton();
    if (m_buttonTuple->layout()) {
        //第二个控件为space
        if (m_buttonTuple->layout()->itemAt(1) != nullptr && m_buttonTuple->layout()->itemAt(1)->spacerItem() != nullptr) {
            int height = m_buttonTuple->layout()->itemAt(1)->spacerItem()->sizeHint().height();
            m_buttonTuple->layout()->itemAt(1)->spacerItem()->changeSize(20, height);
        }
    }

    cancel->setText(tr("Cancel"));
    ok->setText(tr("Replace"));

    m_bottomTip = new QLabel();
    m_bottomTip->setWordWrap(true);

    listVLayout->addSpacing(10);
    listVLayout->addWidget(m_bottomTip);
    listVLayout->addStretch();
    listVLayout->addWidget(m_buttonTuple, 0, Qt::AlignBottom);
    listVLayout->setContentsMargins(20, 10, 20, 10);

    mainVLayout->addLayout(listVLayout);
    setLayout(mainVLayout);
    setContentsMargins(0, 0, 0, 0);

    connect(ok, &QPushButton::clicked, this, &ShortcutContentDialog::onReplace);
    connect(cancel, &QPushButton::clicked, this, &ShortcutContentDialog::close);
    connect(m_shortcutItem, &ShortcutItem::requestUpdateKey, this, &ShortcutContentDialog::onUpdateKey);
    connect(m_model, &ShortcutModel::keyEvent, this, &ShortcutContentDialog::keyEvent);
}

void ShortcutContentDialog::setBottomTip(ShortcutInfo *conflict)
{
    m_conflict = conflict;

    m_info->replace = conflict;

    if (conflict) {
        QString accels = conflict->accels;
        accels = accels.replace("<", "");
        accels = accels.replace(">", "+");
        accels = accels.replace("_L", "");
        accels = accels.replace("_R", "");
        accels = accels.replace("Control", "Ctrl");

        QString str = tr("This shortcut conflicts with  %1, click on Replace to make this shortcut effective immediately")
                      .arg(QString("<span style=\"color: rgba(255, 90, 90, 1);\">%1 %2</span>").arg(conflict->name).arg(QString("[%1]").arg(accels)));
        m_bottomTip->setText(str);
        m_bottomTip->show();
    } else {
        m_bottomTip->clear();
        m_bottomTip->hide();
    }
}

void ShortcutContentDialog::setInfo(ShortcutInfo *info)
{
    m_info = info;
    m_shortcutItem->setShortcutInfo(info);
}

void ShortcutContentDialog::setShortcut(const QString &shortcut)
{
    m_shortcut = shortcut;
    m_shortcutItem->setShortcut(shortcut);
}

void ShortcutContentDialog::keyEvent(bool press, const QString &shortcut)
{
    if (!press) {

        if (shortcut.isEmpty()) {
            setBottomTip(nullptr);
            return;
        }

        if (shortcut == "BackSpace" || shortcut == "Delete") {
            m_shortcut.clear();
            setBottomTip(nullptr);
            return;
        }

        m_shortcut = shortcut;

        // check conflict
        ShortcutInfo *info = m_model->getInfo(shortcut);

        qInfo() << info;
        qInfo() << m_info;

        if (info && info != m_info && info->accels != m_info->accels) {
            m_shortcutItem->setShortcut(info->accels);
            setBottomTip(info);
            return;
        }
        setBottomTip(nullptr);
        m_shortcutItem->setShortcut(shortcut);
    }
}

void ShortcutContentDialog::onReplace()
{
    if (m_info->accels != m_shortcut) {
        if (m_shortcut.isEmpty()) {
            Q_EMIT requestDisableShortcut(m_info);
        } else {
            m_info->accels = m_shortcut;
            Q_EMIT requestSaveShortcut(m_info);
        }
    }
    accept();
}

void ShortcutContentDialog::onUpdateKey()
{
    Q_EMIT requestUpdateKey(nullptr);
}

