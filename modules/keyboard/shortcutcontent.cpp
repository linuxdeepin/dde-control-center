#include "shortcutcontent.h"
#include "shortcutmodel.h"
#include "keyboardwork.h"
#include "shortcutitem.h"
#include "keyboardcontrol.h"
#include "translucentframe.h"

#include <QVBoxLayout>

namespace dcc {
namespace keyboard{
ShortcutContent::ShortcutContent(KeyboardWork *work, QWidget *parent)
    :ContentWidget(parent),
      m_work(work),
      m_conflict(NULL),
      m_curInfo(NULL)
{
    TranslucentFrame* widget = new TranslucentFrame();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);

    SettingsGroup* group = new SettingsGroup();
    TitleButtonItem* item = new TitleButtonItem();
    item->setTitle(tr("Maximized"));
    item->setValue(tr("Please Grab Shortcut Again"));
    group->appendItem(item);
    m_control = new KeyboardControl();
    group->appendItem(m_control);
    layout->addWidget(group);

    m_cancel = new QPushButton(tr("Cancel"));
    m_ok = new QPushButton(tr("Replace"));

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(m_cancel);
    hlayout->addWidget(m_ok);

    layout->addLayout(hlayout);

    m_bottomTip = new QLabel();
    layout->addWidget(m_bottomTip);
    widget->setLayout(layout);
    setContent(widget);

    connect(m_ok, SIGNAL(clicked()), this, SLOT(onReplace()));
    connect(m_cancel, SIGNAL(clicked()), this, SIGNAL(back()));
    connect(item,SIGNAL(click()), this, SLOT(onClick()));
    connect(m_control, SIGNAL(shortcutChanged(QString)), this, SLOT(setShortcut(QString)));
    connect(m_work, SIGNAL(KeyEvent(bool,QString)), this, SLOT(onKeyEvent(bool,QString)));
}

void ShortcutContent::setBottomTip(ShortcutInfo* conflict)
{
    QString str = tr("This shortcut conflicts with  %1, click on Replace to make this chortcut effective immediately").arg(conflict->name);
    m_bottomTip->setText(str);
    m_conflict = conflict;
}

void ShortcutContent::setCurInfo(ShortcutInfo *info)
{
    m_curInfo = info;
}

void ShortcutContent::setConflictString(const QStringList &list)
{
    m_control->setConflictString(list);
}

void ShortcutContent::onClick()
{
    m_work->grabScreen();
}

void ShortcutContent::onReplace()
{
    QString key = m_conflict->accels;
    m_work->modifyShortcut(m_conflict, tr("null"));
    m_work->modifyShortcut(m_curInfo, key);
    m_conflict->item->displayConflict(true);
    m_curInfo->item->displayConflict();
    sendBackSignal();
}

void ShortcutContent::setShortcut(const QString &shortcut)
{
    m_work->modifyShortcut(m_curInfo, shortcut);
    sendBackSignal();
}

void ShortcutContent::onKeyEvent(bool press, QString shortcut)
{
    QString key = shortcut;
    key.replace("Control", "Ctrl");
    key.replace("<","");
    key.replace(">","-");
    QStringList keys = key.split("-");
    for(int i = 0; i<keys.count(); i++)
    {
        m_control->setPress(keys.at(i), press);
    }
    if(!press)
    {
        bool result = m_work->checkAvaliable(shortcut);
        if(result)
        {
            m_work->modifyShortcut(m_curInfo, shortcut);
            sendBackSignal();
        }
    }
}
}
}
