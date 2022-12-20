// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "customedit.h"
#include "widgets/translucentframe.h"
#include "customitem.h"
#include "window/modules/keyboard/waylandgrab.h"
#include "window/utils.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QGuiApplication>

#include <DIconButton>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

keyboard::CustomEdit::CustomEdit(ShortcutModel *model, QWidget *parent):
    ContentWidget(parent),
    m_model(model),
    m_commandGroup(new SettingsGroup),
    m_name(new LineEditWidget),
    m_command(new LineEditWidget),
    m_short(new CustomItem(this)),
    m_tip(new QLabel),
    m_conflict(nullptr),
    m_waylandGrab(nullptr)
{
    if (QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive)) {
        m_waylandGrab = new WaylandGrab(this->topLevelWidget());
    }
    m_tip->setVisible(false);
    m_tip->setWordWrap(true);

    setTitle(tr("Shortcuts"));
    TranslucentFrame *widget = new TranslucentFrame;
    QVBoxLayout *mainlayout = new QVBoxLayout;
    QHBoxLayout *buttonlayout = new QHBoxLayout;

    mainlayout->setMargin(0);
    buttonlayout->setMargin(0);
    mainlayout->setSpacing(0);
    buttonlayout->setSpacing(1);

    m_command->setPlaceholderText(tr("Required"));
    DIconButton *pushbutton = new DIconButton(this);
    pushbutton->setIcon(DStyleHelper(style()).standardIcon(DStyle::SP_SelectElement, nullptr));
    pushbutton->setBackgroundRole(QPalette::ColorRole::Highlight);
    pushbutton->setIconSize(QSize(24, 24));
    m_command->addRightWidget(pushbutton);

    m_commandGroup->appendItem(m_name);
    m_commandGroup->appendItem(m_command);
    m_commandGroup->appendItem(m_short);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QPushButton *okButton = new QPushButton(tr("Save"));

    buttonlayout->addWidget(cancelButton);
    buttonlayout->addWidget(okButton);

    mainlayout->addSpacing(10);
    mainlayout->addWidget(m_commandGroup);
    mainlayout->addWidget(m_tip);
    mainlayout->addSpacing(10);
    mainlayout->addLayout(buttonlayout);
    mainlayout->addStretch();

    widget->setLayout(mainlayout);

    setContent(widget);

    connect(cancelButton, &QPushButton::clicked, this, [this]{
        if (this->parentWidget()) {
            this->parentWidget()->setFocus();
        }
        Q_EMIT CustomEdit::back();
    });
    connect(m_name->dTextEdit(), &DLineEdit::textChanged, this, [=](const QString &text) {
        okButton->setEnabled(true);
        m_nameChanged = m_info->name != text;

        // 如果用户输入的快捷键名称和已有快捷键名称重复（冲突），置灰保存按钮
        // 防止设置的快捷键名称和已有快捷键名称显示重复
        for (auto info : model->infos()) {
            if (info->name == text && m_nameChanged) {
                m_nameRepeated = true;
                okButton->setEnabled(false);
                return;
            }
        }
        m_nameRepeated = false;
        okButton->setEnabled(!m_nameRepeated && (m_nameChanged || m_commandChanged || m_accelsChanged));
    });
    connect(m_command->dTextEdit(), &DLineEdit::textChanged, this, [=](const QString &text) {
        m_commandChanged = m_info->command != text;
        okButton->setEnabled(!m_nameRepeated && (m_nameChanged || m_commandChanged || m_accelsChanged));
    });
    connect(m_short, &CustomItem::changeAlert, this, [=] {
        m_accelsChanged = m_info->accels != m_short->text();
        okButton->setEnabled(!m_nameRepeated && (m_nameChanged || m_commandChanged || m_accelsChanged));
    });
    connect(pushbutton, &DIconButton::clicked, this, &CustomEdit::onOpenFile);
    connect(m_short, &CustomItem::requestUpdateKey, this, &CustomEdit::onUpdateKey);
    connect(okButton, &QPushButton::clicked, this, [this]{
        if (this->parentWidget()) {
            this->parentWidget()->setFocus();
        }
        Q_EMIT CustomEdit::onSaveAccels();
    });

    connect(model, &ShortcutModel::keyEvent, this, &CustomEdit::keyEvent);
}

void keyboard::CustomEdit::setShortcut(keyboard::ShortcutInfo *info)
{
    m_info = info;

    m_short->setTitle(tr("Shortcut"));
    m_short->setShortcut(info->accels);

    m_name->setTitle(tr("Name"));
    m_command->setTitle(tr("Command"));

    m_name->setText(m_info->name);
    m_command->setText(m_info->command);
}

void keyboard::CustomEdit::setBottomTip(keyboard::ShortcutInfo *conflict)
{
    m_conflict = conflict;
    if (conflict) {
        QString accels = conflict->accels;
        accels = accels.replace("<", "[");
        accels = accels.replace(">", "+");
        accels = accels.replace("_L","");
        accels = accels.replace("_R", "");
        accels = accels.replace("Control", "Ctrl");

        QString str = tr("This shortcut conflicts with %1, click on Add to make this shortcut effective immediately")
                .arg(QString("<span style=\"color: rgba(255, 90, 90, 1);\">%1 %2</span>").arg(conflict->name).arg(QString("[%1]").arg(accels)));
        m_tip->setText(str);
        m_tip->setVisible(true);
    } else {
        m_tip->setVisible(false);
    }
}

void keyboard::CustomEdit::keyEvent(bool press, const QString &shortcut)
{
    m_short->setShortcut(shortcut);

    if (!press) {

        if (shortcut.isEmpty()) {
            m_short->setShortcut(m_info->accels);
            setBottomTip(nullptr);
            return;
        }

        if (shortcut == "BackSpace" || shortcut == "Delete") {
            m_short->setShortcut("");
            setBottomTip(nullptr);
            return;
        }

        // check conflict
        ShortcutInfo *info = m_model->getInfo(shortcut);
        if (info && info != m_info && info->accels != m_info->accels) {
            setBottomTip(info);
            return;
        }
        setBottomTip(nullptr);
    }
}

void keyboard::CustomEdit::onOpenFile()
{
    Q_EMIT requestFrameAutoHide(false);

    QString file = QFileDialog::getOpenFileName(this, "", "/usr/bin");
    m_command->setText(file);

    Q_EMIT requestFrameAutoHide(true);
}

void keyboard::CustomEdit::onSaveAccels()
{
    if (m_name->text().isEmpty())
        m_name->setIsErr(true);

    if (m_command->text().isEmpty())
        m_command->setIsErr(true);

    if (m_name->text().isEmpty() || m_command->text().isEmpty() || m_short->text().isEmpty())
        return;

    if (m_conflict) {
        m_info->replace = m_conflict;
    }

    m_info->name = m_name->text();
    m_info->command = m_command->text();
    m_info->accels = m_short->text();

    Q_EMIT requestSaveShortcut(m_info);

    Q_EMIT back();
}

void keyboard::CustomEdit::onUpdateKey()
{
    Q_EMIT requestUpdateKey(nullptr);
}

void keyboard::CustomEdit::onGrab(ShortcutInfo *info)
{
    if (m_waylandGrab)
        m_waylandGrab->onGrab(info);
}

void keyboard::CustomEdit::keyPressEvent(QKeyEvent *e)
{
    if (!QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive)
            || !m_waylandGrab || !m_waylandGrab->getZxgm()) {
        return;
    }
    m_waylandGrab->setKeyValue(WaylandkeyMap[e->key()]);
    QString lastKey = m_waylandGrab->getLastKey();
    QString keyValue = m_waylandGrab->getKeyValue();

    m_waylandGrab->setRecordState(true);
    keyEvent(true, m_waylandGrab->getRecordState() ? lastKey + keyValue : keyValue);
    if (e->key() == Qt::Key_Control || e->key() == Qt::Key_Alt
            || e->key() == Qt::Key_Shift || e->key() == Qt::Key_Super_L || e->key() == Qt::Key_Super_R) {
        lastKey += ("<" + keyValue.remove(keyValue.indexOf("_"), 2) + ">");
        m_waylandGrab->setLastKey(lastKey);
    }
    QWidget::keyPressEvent(e);
}

void keyboard::CustomEdit::keyReleaseEvent(QKeyEvent *e)
{
    if (!QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive)
            || !m_waylandGrab || !m_waylandGrab->getZxgm() || !m_waylandGrab->getRecordState()) {
        return;
    }
    QString lastKey = m_waylandGrab->getLastKey();
    const QString keyValue = m_waylandGrab->getKeyValue();
    if (!lastKey.isEmpty()) {
        if (WaylandkeyMap[Qt::Key_Control] == keyValue
                || WaylandkeyMap[Qt::Key_Alt] == keyValue || WaylandkeyMap[Qt::Key_Shift] == keyValue) {
            keyEvent(false, "");
        } else if (WaylandkeyMap[Qt::Key_Super_L] == keyValue || WaylandkeyMap[Qt::Key_Super_R] == keyValue) {
            keyEvent(false, "Super_L");
        } else {
            keyEvent(false, lastKey + keyValue);
        }
    } else {
        keyEvent(false, "");
    }
    m_waylandGrab->setLastKey("");
    m_waylandGrab->setRecordState(false);
    m_waylandGrab->onUnGrab();
    QWidget::keyReleaseEvent(e);
}

void keyboard::CustomEdit::mousePressEvent(QMouseEvent *e)
{
    if (!QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive)
            || !m_waylandGrab || !m_waylandGrab->getZxgm()) {
        return;
    }
    setFocus();
    if (!m_waylandGrab->getRecordState()) {
        m_waylandGrab->onUnGrab();
    }
    QWidget::mousePressEvent(e);
}
