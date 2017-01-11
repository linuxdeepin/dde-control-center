#include "shortcutcontent.h"
#include "shortcutmodel.h"
#include "keyboardwork.h"
#include "shortcutitem.h"
#include "keyboardcontrol.h"
#include "translucentframe.h"
#include "keyboardmodel.h"
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
    m_bottomTip->setWordWrap(true);
    layout->addWidget(m_bottomTip);
    layout->addStretch();
    widget->setLayout(layout);
    setContent(widget);

    connect(m_ok, SIGNAL(clicked()), this, SLOT(onReplace()));
    connect(m_cancel, SIGNAL(clicked()), this, SIGNAL(back()));
    connect(item,SIGNAL(click()), this, SLOT(onClick()));

    connect(m_work, &KeyboardWork::KeyEvent, this, &ShortcutContent::onKeyEvent);
}

void ShortcutContent::setBottomTip(ShortcutInfo* conflict)
{
    m_conflict = conflict;
    if(conflict)
    {
        QString str = tr("This shortcut conflicts with  %1, click on Replace to make this shortcut effective immediately").arg(conflict->name);
        m_bottomTip->setText(str);
        m_bottomTip->show();
    }
    else
    {
        m_bottomTip->clear();
        m_bottomTip->hide();
    }
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
    m_control->setFocus();

    m_bottomTip->clear();
    m_bottomTip->hide();

    for (int i = 0; i < ModelKeylist.count(); i++) {
        m_control->setConflicts(ModelKeylist.at(i), false);
    }
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

void ShortcutContent::onKeyEvent(const bool state, const QString &keylist)
{
    QString in = keylist;
    in.replace("<", "");
    in.replace(">", "-");
    in.replace("_L", "");
    QStringList value = in.split("-");
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
        bool result = m_work->checkAvaliable(keylist);
        emit this->shortcut(keylist);
        if(result)
        {
            m_work->modifyShortcut(m_curInfo, keylist);
            sendBackSignal();
        }
    }
}

}
}
