#include "customcontent.h"
#include "keyboardcontrol.h"
#include "keyboardwork.h"
#include "shortcutmodel.h"
#include "inputitem.h"
#include "translucentframe.h"
#include "keyboardmodel.h"
#include <QMap>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace dcc
{
namespace keyboard
{

CustomContent::CustomContent(KeyboardWork *work, QWidget *parent)
    : ContentWidget(parent),
      m_work(work)
{
    setTitle(tr("Shortcuts"));
    TranslucentFrame *widget = new TranslucentFrame();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    m_commandGroup = new SettingsGroup();
    m_name = new InputItem();
    m_name->setTitle(tr("Name"));

    m_command = new InputItem();
    m_command->setTitle(tr("Command"));
    m_command->setChooseVisible(true);
    m_command->setPlaceholderText(tr("Required"));
    connect(m_command, SIGNAL(click()), m_command, SLOT(onClick()));

    m_shortcut = new InputItem();
    m_shortcut->setTitle(tr("Shortcut"));
    m_shortcut->setChooseVisible(true);
    m_shortcut->setReadOnly(true);
    m_shortcut->setRightText(tr("Please Grab Shortcut Again"));

    m_commandGroup->appendItem(m_name);
    m_commandGroup->appendItem(m_command);
    m_commandGroup->appendItem(m_shortcut);

    layout->addWidget(m_commandGroup);
    m_control = new KeyboardControl();
    layout->addWidget(m_control);

    m_cancel = new QPushButton(tr("Cancel"));
    m_ok = new QPushButton(tr("Add"));

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(m_cancel);
    hlayout->addWidget(m_ok);
    layout->addLayout(hlayout);

    m_bottomTip = new QLabel();
    m_bottomTip->setWordWrap(true);
    layout->addWidget(m_bottomTip);
    layout->addStretch();
    m_bottomTip->hide();

    widget->setLayout(layout);
    setContent(widget);

    connect(m_cancel, SIGNAL(clicked()), this, SIGNAL(back()));
    connect(m_shortcut, SIGNAL(click()), this, SLOT(onClick()));
    connect(m_work, &KeyboardWork::KeyEvent, this, &CustomContent::onKeyEvent);
    connect(m_ok, SIGNAL(clicked()), this, SLOT(onShortcut()));
}

void CustomContent::setBottomTip(ShortcutInfo *conflict)
{
    m_conflict = conflict;
    if (conflict) {
        QString str = tr("This shortcut conflicts with %1, click on Add to make this shortcut effective immediately").arg(conflict->name);
        m_bottomTip->setText(str);
        m_bottomTip->show();
    } else {
        m_bottomTip->clear();
        m_bottomTip->hide();
    }
}

void CustomContent::setConflictString(const QStringList &list)
{
    m_control->setConflictString(list);
}

void CustomContent::onClick()
{
    m_work->grabScreen();
    m_control->setFocus();

    m_bottomTip->clear();
    m_bottomTip->hide();

    for (int i = 0; i < ModelKeylist.size(); ++i) {
        m_control->setConflicts(ModelKeylist.at(i), false);
    }
}

void CustomContent::onShortcut()
{
    bool result = true;
    if (m_conflict) {
        QString key = m_conflict->accels;
        m_work->modifyShortcut(m_conflict, tr("null"));
        m_work->addCustonShortcut(m_name->value(), m_command->value(), key, result);
    } else {
        m_work->addCustonShortcut(m_name->value(), m_command->value(), m_shortcut->value(), result);
    }

    sendBackSignal();
}

void CustomContent::onKeyEvent(const bool state, const QString &keylist)
{
    QString in = keylist;
    in.replace("<", "");
    in.replace(">", "-");
    in.replace("_L", "");
    QStringList value = in.split("-");
    m_shortcut->setValue(in);
    QMap<QString, bool> list;
    for (QString key : ModelKeylist) {
        QStringList t;
        t << value << key;
        list.insert(key, m_work->keyOccupy(t));
    }

    for (int i = 0; i < ModelKeylist.size(); ++i)
        m_control->setPress(ModelKeylist.at(i), false);

    for (int i = 0; i < ModelKeylist.size(); ++i) {
        m_control->setPress(ModelKeylist.at(i), list[ModelKeylist.at(i)]);
    }
    if (!state && !keylist.isEmpty()){
        emit shortcut(keylist);
    }
}

}
}
