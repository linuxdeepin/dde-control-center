//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "customcontentdialog.h"
#include "operation/keyboardwork.h"
#include "operation/shortcutmodel.h"
#include "operation/keyboardmodel.h"
#include "customitem.h"
#include "widgets/buttontuple.h"
#include "widgets/lineeditwidget.h"
#include "widgets/settingsgroup.h"

#include <dfilechooseredit.h>

#include <DLineEdit>
#include <DTitlebar>
#include <DFontSizeManager>

#include <QMap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QPushButton>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

CustomContentDialog::CustomContentDialog(ShortcutModel *model, QWidget *parent)
    : DAbstractDialog(parent)
    , m_shortCutNameEdit(new DLineEdit(this))
    , m_shortCutCmdEdit(new DFileChooserEdit(this))
    , m_conflict(nullptr)
    , m_model(model)
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save, this))
{
    setFixedSize(QSize(400, 388));
    QVBoxLayout *mainVLayout = new QVBoxLayout();
    mainVLayout->setContentsMargins(0, 0, 0, 5);
    mainVLayout->setSpacing(0);

    QVBoxLayout *listVLayout = new QVBoxLayout();
    listVLayout->setAlignment(Qt::AlignHCenter);
    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr(""));

    mainVLayout->addWidget(titleIcon);

    QLabel *shortCutTitle = new QLabel(tr("Add Custom Shortcut"));
    DFontSizeManager::instance()->bind(shortCutTitle, DFontSizeManager::T5, QFont::DemiBold); // 设置label字体
    shortCutTitle->setAlignment(Qt::AlignCenter);
    listVLayout->addWidget(shortCutTitle);
    listVLayout->addSpacing(30);

    QLabel *shortCutName = new QLabel(tr("Name"));
    QHBoxLayout *shortCutNameLayout = new QHBoxLayout;
    shortCutNameLayout->addWidget(shortCutName);
    shortCutNameLayout->setContentsMargins(10, 0, 0, 0);
    listVLayout->addLayout(shortCutNameLayout);

    m_shortCutNameEdit->lineEdit()->setPlaceholderText(tr("Required"));
    connect(m_shortCutNameEdit, &DLineEdit::textChanged, this, [this] {
        if (!m_shortCutNameEdit->text().isEmpty()) {
            m_shortCutNameEdit->setAlert(false);
        }
    });

    listVLayout->addWidget(m_shortCutNameEdit);
    listVLayout->addSpacing(4);

    QLabel *shortCutCmd = new QLabel(tr("Command"));
    QHBoxLayout *shortCutCmdLayout = new QHBoxLayout;
    shortCutCmdLayout->addWidget(shortCutCmd);
    shortCutCmdLayout->setContentsMargins(10, 0, 0, 0);
    listVLayout->addLayout(shortCutCmdLayout);

    // TODO: DFileChooserEdit控件包含有button按钮，点击触发QFileDialog文件弹窗，其为exec模态显示。
    // 该显示方式方式会触发程序异常崩溃，具体详情为当控制中心存在exec()模态显示的对话框界面时，若通过dbus调用切换菜单时，会导致程序崩溃。
    // 模态对话框处于事件监听阻塞状态没有被主动关闭，此时触发切换其他界面则会使阻塞的模态对话框关闭异常而导致程序崩溃。
    // 目前该DFileChooserEdit控件类会触发上诉所描述问题，暂时未解决，待后续完善。。
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

    listVLayout->addWidget(m_shortCutCmdEdit);
    listVLayout->addSpacing(15);

    m_shortcut = new CustomItem;
    m_shortcut->setShortcut("");
    m_shortcut->addBackground();

    listVLayout->addWidget(m_shortcut);

    QPushButton *cancel = m_buttonTuple->leftButton();
    cancel->setText(tr("Cancel"));
    QPushButton *ok = m_buttonTuple->rightButton();
    ok->setText(tr("Add"));

    m_bottomTip = new QLabel();
    m_bottomTip->setWordWrap(true);
    m_bottomTip->hide();

    listVLayout->addStretch();
    listVLayout->addWidget(m_buttonTuple);
    listVLayout->addWidget(m_bottomTip);
    listVLayout->setContentsMargins(20, 10, 20, 10);

    mainVLayout->addLayout(listVLayout);
    setLayout(mainVLayout);
    setContentsMargins(0, 0, 0, 0);

    connect(cancel, &QPushButton::clicked, this, &CustomContentDialog::close);
    connect(ok, &QPushButton::clicked, this, &CustomContentDialog::onShortcut);
    connect(m_shortcut, &CustomItem::requestUpdateKey, this, &CustomContentDialog::updateKey);
    connect(model, &ShortcutModel::keyEvent, this, &CustomContentDialog::keyEvent);
    connect(m_shortcut, &CustomItem::changeAlert, this, [this] {
        m_shortcut->setAlert(false);
    });
}

void CustomContentDialog::keyPressEvent(QKeyEvent *e) {
    switch (e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return: {
        m_buttonTuple->rightButton()->click();
        return;
    }
    }

    DAbstractDialog::keyReleaseEvent(e);
}

void CustomContentDialog::setBottomTip(ShortcutInfo *conflict)
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

void CustomContentDialog::onShortcut()
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
    accept();
}

void CustomContentDialog::keyEvent(bool press, const QString &shortcut)
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

void CustomContentDialog::updateKey()
{
    Q_EMIT requestUpdateKey(nullptr);
}
