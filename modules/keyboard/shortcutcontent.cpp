#include "shortcutcontent.h"

#include "keyboardcontrol.h"
#include <QVBoxLayout>

ShortcutContent::ShortcutContent(KeyboardWork *work, QWidget *parent)
    :ContentWidget(parent),
      m_work(work),
      m_conflict(NULL),
      m_curInfo(NULL)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);

    SettingsGroup* group = new SettingsGroup();
    TitleValueItem* item = new TitleValueItem();
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

    connect(m_ok, SIGNAL(clicked()), this, SLOT(onClick()));
    connect(m_control, SIGNAL(shortcutChanged(QString)), this, SLOT(setShortcut(QString)));
}

void ShortcutContent::setBottomTip(ShortcutInfo* conflict)
{
    QString str = QString("此快捷键和%1快捷键冲突，您依然可以点替换使冲突的快捷键失效").arg(conflict->name);
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
    QString key = m_conflict->accels;
    m_work->modifyShortcut(m_conflict, tr("null"));
    m_work->modifyShortcut(m_curInfo, key);
}

void ShortcutContent::setShortcut(const QString &shortcut)
{
    qDebug()<<Q_FUNC_INFO<<shortcut;
    m_work->modifyShortcut(m_curInfo, shortcut);
    sendBackSignal();
}
