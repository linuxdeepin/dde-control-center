#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QEvent>
#include <QGuiApplication>

#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>

#include "shortcutedit.h"
#include "private/shortcutdbus.h"

DUI_USE_NAMESPACE

QString shortcutTransfrom(const QString& str)
{
    if(str == "")
        return QObject::tr("None");

    QStringList shortnamelist = str.split("-");
    for(QString &keyname: shortnamelist){
        if(keyname.count()>0)
            keyname[0]=keyname[0].toUpper();
    }
    return shortnamelist.join("+");
}

ShortcutEdit::ShortcutEdit(QWidget *parent) :
    QFrame(parent),
    m_layout(new QHBoxLayout),
    m_label(new QLabel),
    m_edit(new QLineEdit),
    m_dbus(new ShortcutDbus)
{
    m_label->setObjectName("Label");
    m_edit->setObjectName("Edit");

    D_THEME_INIT_WIDGET(ShortcutEdit);

    m_label->installEventFilter(this);
    m_edit->setReadOnly(true);
    m_edit->setPlaceholderText(tr("Please input new shortcut"));
    m_edit->setMinimumSize(170, 25);
    m_edit->hide();

    m_layout->setMargin(0);
    m_layout->addWidget(m_label, 0, Qt::AlignRight|Qt::AlignVCenter);
    m_layout->addWidget(m_edit, 0, Qt::AlignRight|Qt::AlignVCenter);

    connect(m_dbus, &ShortcutDbus::KeyReleaseEvent, [&](QString key){
        if(!m_edit->isVisible())
            return;

        if(key.isEmpty() || key.toLower() == "escape"){
            quitEditState();
            return;
        }

        if(key.toLower() == "backspace" || key.toLower() == "delete"){
            key = "";
        }else{
            int tmp;
            QString result = m_dbus->CheckShortcutConflict(key, tmp).value();

            if(result!="Valid"){
                key = result+"_"+key;
            }else{
                key = "Valid"+QString("_")+shortcutTransfrom(key);
            }
        }

        quitEditState();
        emit shortcutKeyFinished(key);
    });

    setLayout(m_layout);
}

QString ShortcutEdit::shortcut() const
{
    return m_label->text();
}

bool ShortcutEdit::editing() const
{
    return m_edit->isVisible();
}

void ShortcutEdit::setShortcut(QString shortcut)
{
    m_label->setText(shortcut);
}

void ShortcutEdit::setEnableEdit(bool arg)
{
    if(arg){
        m_label->installEventFilter(this);
    }else{
        m_edit->hide();
        m_label->show();
        m_label->removeEventFilter(this);
    }
}

void ShortcutEdit::quitEditState()
{
    m_edit->hide();
    m_label->show();
}

bool ShortcutEdit::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == m_label){
        if(e->type() == QEvent::MouseButtonPress){
            m_label->hide();
            m_edit->show();
            m_dbus->GrabKbdAndMouse();
        }
    }

    return false;
}

