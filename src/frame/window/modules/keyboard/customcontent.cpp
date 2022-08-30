// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "customcontent.h"
#include "modules/keyboard/keyboardcontrol.h"
#include "modules/keyboard/keyboardwork.h"
#include "modules/keyboard/shortcutmodel.h"
#include "modules/keyboard/keyboardmodel.h"
#include "modules/keyboard/customitem.h"
#include "widgets/translucentframe.h"
#include "widgets/buttontuple.h"
#include "widgets/lineeditwidget.h"
#include "widgets/settingsgroup.h"
#include "window/utils.h"
#include "waylandgrab.h"

#include <dfilechooseredit.h>

#include <DLineEdit>

#include <QMap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QPushButton>
#include <QGuiApplication>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::keyboard;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;

CustomContent::CustomContent(ShortcutModel *model, QWidget *parent)
    : QWidget(parent)
    , m_conflict(nullptr)
    , m_model(model)
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save))
    , m_waylandGrab(nullptr)
{
    if (QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive)) {
        m_waylandGrab = new WaylandGrab(this->parent());
    }

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(10, 35, 10, 0);
    //~ contents_path /keyboard/Shortcuts/Add Custom Shortcut
    //~ child_page Shortcuts
    QLabel *shortCutTitle = new QLabel(tr("Add Custom Shortcut"));
    DFontSizeManager::instance()->bind(shortCutTitle, DFontSizeManager::T5, QFont::DemiBold); // 设置label字体
    shortCutTitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(shortCutTitle);
    mainLayout->addSpacing(30);

    QLabel *shortCutName = new QLabel(tr("Name"));
    QHBoxLayout *shortCutNameLayout = new QHBoxLayout;
    shortCutNameLayout->addWidget(shortCutName);
    shortCutNameLayout->setContentsMargins(10, 0, 0, 0);
    mainLayout->addLayout(shortCutNameLayout);

    m_shortCutNameEdit = new DLineEdit;
    m_shortCutNameEdit->lineEdit()->setPlaceholderText(tr("Required"));
    connect(m_shortCutNameEdit, &DLineEdit::textChanged, this, [this] {
        if (!m_shortCutNameEdit->text().isEmpty()) {
            m_shortCutNameEdit->setAlert(false);
        }
    });

    mainLayout->addWidget(m_shortCutNameEdit);
    mainLayout->addSpacing(4);

    QLabel *shortCutCmd = new QLabel(tr("Command"));
    QHBoxLayout *shortCutCmdLayout = new QHBoxLayout;
    shortCutCmdLayout->addWidget(shortCutCmd);
    shortCutCmdLayout->setContentsMargins(10, 0, 0, 0);
    mainLayout->addLayout(shortCutCmdLayout);

    // TODO: DFileChooserEdit控件包含有button按钮，点击触发QFileDialog文件弹窗，其为exec模态显示。
    // 该显示方式方式会触发程序异常崩溃，具体详情为当控制中心存在exec()模态显示的对话框界面时，若通过dbus调用切换菜单时，会导致程序崩溃。
    // 模态对话框处于事件监听阻塞状态没有被主动关闭，此时触发切换其他界面则会使阻塞的模态对话框关闭异常而导致程序崩溃。
    // 目前该DFileChooserEdit控件类会触发上诉所描述问题，暂时未解决，待后续完善。。
    m_shortCutCmdEdit = new DFileChooserEdit(this);
    m_shortCutCmdEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_shortCutNameEdit->setAccessibleName("SHORTCUT_NAME_EDIT");
    m_shortCutCmdEdit->setAccessibleName("SHORTCUT_CMD_EDIT");

    connect(m_shortCutCmdEdit, &DFileChooserEdit::textChanged, this, [this] {
        if (!m_shortCutCmdEdit->text().isEmpty()) {
            m_shortCutCmdEdit->setAlert(false);
        }
    });

    connect(m_shortCutNameEdit, &DFileChooserEdit::textChanged, this, [this] {
        bool exist = false;
        QList<ShortcutInfo *> lstInfo;
        lstInfo.append(m_model->customInfo());
        lstInfo.append(m_model->assistiveToolsInfo());
        lstInfo.append(m_model->systemInfo());
        lstInfo.append(m_model->workspaceInfo());
        for (auto info : lstInfo) {
            if (!info->name.compare(m_shortCutNameEdit->text(),Qt::CaseSensitive)) {
                exist = true;
                break;
            }
        }
        m_buttonTuple->rightButton()->setEnabled(!exist);
    });

    mainLayout->addWidget(m_shortCutCmdEdit);
    mainLayout->addSpacing(15);

    m_shortcut = new CustomItem;
    m_shortcut->setShortcut("");
    m_shortcut->addBackground();

    mainLayout->addWidget(m_shortcut);

    QPushButton *cancel = m_buttonTuple->leftButton();
    cancel->setText(tr("Cancel"));
    QPushButton *ok = m_buttonTuple->rightButton();
    ok->setText(tr("Add"));

    m_bottomTip = new QLabel();
    m_bottomTip->setWordWrap(true);
    m_bottomTip->hide();

    mainLayout->addStretch();
    mainLayout->addWidget(m_buttonTuple);
    mainLayout->addWidget(m_bottomTip);
    setLayout(mainLayout);

    connect(cancel, &QPushButton::clicked, this, [ this ] {
        if (parentWidget())
            this->parentWidget()->setFocus();
        Q_EMIT back();
    });
    connect(ok, &QPushButton::clicked, this, [ this ] {
        if (parentWidget())
            this->parentWidget()->setFocus();
        Q_EMIT onShortcut();
    });
    connect(m_shortcut, &CustomItem::requestUpdateKey, this, &CustomContent::updateKey);
    connect(model, &ShortcutModel::keyEvent, this, &CustomContent::keyEvent);
    connect(m_shortcut, &CustomItem::changeAlert, this, [this] {
        m_shortcut->setAlert(false);
    });
}

void CustomContent::setBottomTip(ShortcutInfo *conflict)
{
    m_conflict = conflict;
    if (conflict) {
        QString accels = conflict->accels;
        accels = accels.replace("<", "");
        accels = accels.replace(">", "+");
        accels = accels.replace("_L", "");
        accels = accels.replace("_R", "");
        accels = accels.replace("Control", "Ctrl");

        QString str = tr("This shortcut conflicts with %1, click on Add to make this shortcut effective immediately")
                      .arg(QString("<span style=\"color: rgba(255, 90, 90, 1);\">%1 %2</span>").arg(conflict->name).arg(QString("[%1]").arg(accels)));
        m_bottomTip->setText(str);
        m_bottomTip->show();
    } else {
        m_bottomTip->clear();
        m_bottomTip->hide();
    }
}

void CustomContent::onShortcut()
{
    m_shortCutNameEdit->setAlert(m_shortCutNameEdit->text().isEmpty());
    m_shortCutCmdEdit->setAlert(m_shortCutCmdEdit->lineEdit()->text().isEmpty());
    m_shortcut->setAlert(m_shortcut->text().isEmpty());

    if (m_shortcut->text().isEmpty() || m_shortCutCmdEdit->lineEdit()->text().isEmpty() || m_shortCutNameEdit->text().isEmpty()) {
        return;
    }

    if (m_conflict)
        Q_EMIT requestForceSubs(m_conflict);

    Q_EMIT requestAddKey(m_shortCutNameEdit->text(), m_shortCutCmdEdit->text(), m_shortcut->text());
    Q_EMIT back();
}

void CustomContent::keyEvent(bool press, const QString &shortcut)
{
    if (!press) {
        if (shortcut.isEmpty() || shortcut == "BackSpace" || shortcut == "Delete") {
            m_shortcut->setShortcut("");
            setBottomTip(nullptr);
            return;
        }

        // check conflict
        ShortcutInfo *conflict = m_model->getInfo(shortcut);
        setBottomTip(conflict);
    }

    m_shortcut->setShortcut(shortcut);
}

void CustomContent::updateKey()
{
    Q_EMIT requestUpdateKey(nullptr);
}

void CustomContent::onGrab(ShortcutInfo *info)
{
    if (m_waylandGrab)
        m_waylandGrab->onGrab(info);
}

void CustomContent::keyPressEvent(QKeyEvent *ke)
{
    if (!QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive)
            || !m_waylandGrab || !m_waylandGrab->getZxgm()) {
        return;
    }
    m_waylandGrab->setKeyValue(WaylandkeyMap[ke->key()]);
    QString lastKey = m_waylandGrab->getLastKey();
    QString keyValue = m_waylandGrab->getKeyValue();

    m_waylandGrab->setRecordState(true);
    keyEvent(true, m_waylandGrab->getRecordState() ? lastKey + keyValue : keyValue);
    if (ke->key() == Qt::Key_Control || ke->key() == Qt::Key_Alt
            || ke->key() == Qt::Key_Shift || ke->key() == Qt::Key_Super_L || ke->key() == Qt::Key_Super_R) {
        lastKey += ("<" + keyValue.remove(keyValue.indexOf("_"), 2) + ">");
        m_waylandGrab->setLastKey(lastKey);
    }
    return QWidget::keyPressEvent(ke);
}

void CustomContent::keyReleaseEvent(QKeyEvent *ke)
{
    if (!QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive)
            || !m_waylandGrab || !m_waylandGrab->getZxgm() || !m_waylandGrab->getRecordState()) {
        return;
    }
    QString lastKey = m_waylandGrab->getLastKey();
    QString keyValue = m_waylandGrab->getKeyValue();
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
    return QWidget::keyReleaseEvent(ke);
}

void CustomContent::mousePressEvent(QMouseEvent *e)
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