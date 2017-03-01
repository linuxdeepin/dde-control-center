#include "customcontent.h"
#include "keyboardcontrol.h"
#include "keyboardwork.h"
#include "shortcutmodel.h"
#include "translucentframe.h"
#include "keyboardmodel.h"
#include "dimagebutton.h"
#include <QMap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

DWIDGET_USE_NAMESPACE

namespace dcc
{
namespace keyboard
{

CustomContent::CustomContent(KeyboardWork *work, QWidget *parent)
    : ContentWidget(parent),
      m_work(work),
      m_conflict(nullptr),
      m_buttonTuple(new ButtonTuple)
{
    setTitle(tr("Shortcuts"));
    TranslucentFrame *widget = new TranslucentFrame();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(10);

    m_commandGroup = new SettingsGroup();
    m_name = new LineEditWidget();
    m_name->setTitle(tr("Name"));

    m_command = new LineEditWidget();
    m_command->setTitle(tr("Command"));
    m_command->setPlaceholderText(tr("Required"));
    QPushButton *pushbutton = new QPushButton("...");
    pushbutton->setFixedWidth(50);
    m_command->addRightWidget(pushbutton);

    m_shortcut = new LineEditWidget();
    m_shortcut->setTitle(tr("Shortcut"));
    m_shortcut->setReadOnly(true);

    QWidget *labelwidget = new QWidget;
    QHBoxLayout *hlayout = new QHBoxLayout(labelwidget);
    hlayout->setSpacing(0);
    hlayout->setMargin(0);
    DImageButton *label = new DImageButton;
    label->setText(tr("Please Grab Shortcut Again"));
    hlayout->addWidget(label);
    hlayout->addSpacing(20);

    m_shortcut->addRightWidget(labelwidget);

    m_commandGroup->appendItem(m_name);
    m_commandGroup->appendItem(m_command);
    m_commandGroup->appendItem(m_shortcut);

    layout->addWidget(m_commandGroup);
    m_control = new KeyboardControl();
//    layout->addWidget(m_control);

    QPushButton *cancel = m_buttonTuple->leftButton();
    cancel->setText(tr("Cancel"));
    QPushButton *ok = m_buttonTuple->rightButton();
    ok->setText(tr("Add"));

    m_bottomTip = new QLabel();
    m_bottomTip->setWordWrap(true);
    m_bottomTip->hide();

    layout->addWidget(m_buttonTuple);
    layout->addWidget(m_bottomTip);
    layout->addStretch();

    widget->setLayout(layout);
    setContent(widget);

    connect(cancel, SIGNAL(clicked()), this, SIGNAL(back()));
    connect(ok, SIGNAL(clicked()), this, SLOT(onShortcut()));
    connect(pushbutton, &QPushButton::clicked, this, &CustomContent::onOpenFile);
    connect(label, &DImageButton::clicked, this, &CustomContent::onClick);
    connect(m_work, &KeyboardWork::KeyEvent, this, &CustomContent::onKeyEvent);
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
        m_work->addCustomShortcut(m_name->text(), m_command->text(), key, result);
    } else {
        m_work->addCustomShortcut(m_name->text(), m_command->text(), m_shortcut->text(), result);
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
    m_shortcut->setText(keylist);
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

void CustomContent::onOpenFile()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose File"), tr("/usr/bin"));
    m_command->setText(file);
}

}
}
