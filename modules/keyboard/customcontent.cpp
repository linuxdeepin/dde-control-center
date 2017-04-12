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
    layout->addSpacing(10);

    m_commandGroup = new SettingsGroup();
    m_name = new LineEditWidget();
    m_name->setTitle(tr("Name"));

    m_command = new LineEditWidget();
    m_command->setTitle(tr("Command"));
    m_command->setPlaceholderText(tr("Required"));
    QPushButton *pushbutton = new QPushButton("...");
    pushbutton->setFixedWidth(50);
    m_command->addRightWidget(pushbutton);

    m_shortcut = new CustomItem(work);

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
    connect(m_shortcut, &CustomItem::shortcut, this, &CustomContent::shortcut);
}

void CustomContent::setBottomTip(ShortcutInfo *conflict)
{
    m_conflict = conflict;
    if (conflict) {
        m_shortcut->setInfo(conflict);
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

void CustomContent::onOpenFile()
{
    emit requestFrameAutoHide(false);

    QString file = QFileDialog::getOpenFileName(this, tr("Choose File"), tr("/usr/bin"));
    m_command->setText(file);

    emit requestFrameAutoHide(true);
}

}
}
