#include "keyboardmodule.h"
#include "keyitem.h"
#include "keyboardmodel.h"
#include "customedit.h"

namespace dcc {
namespace keyboard{

KeyboardModule::KeyboardModule(FrameProxyInterface *frame, QObject *parent)
    :QObject(parent),
      ModuleInterface(frame),
      m_loaded(false),
      m_keyboardWidget(nullptr),
#ifndef DCC_DISABLE_KBLAYOUT
      m_kbDetails(nullptr),
      m_kbLayoutWidget(nullptr),
#endif
      m_shortcutWidget(nullptr),
#ifndef DCC_DISABLE_LANGUAGE
      m_langWidget(nullptr),
#endif
      m_scContent(nullptr),
      m_customContent(nullptr),
      m_customEdit(nullptr)
{
}

void KeyboardModule::initialize()
{
    m_model = new KeyboardModel();
    m_shortcutModel = new ShortcutModel();
    m_work = new KeyboardWork(m_model);

    connect(m_work, SIGNAL(shortcutInfo(QString)), m_shortcutModel, SLOT(onParseInfo(QString)));
    connect(m_work, SIGNAL(customInfo(QString)), m_shortcutModel, SLOT(onCustomInfo(QString)));

    m_model->moveToThread(qApp->thread());
    m_shortcutModel->moveToThread(qApp->thread());
    m_work->moveToThread(qApp->thread());
}

void KeyboardModule::moduleActive()
{
    m_work->active();
}

void KeyboardModule::moduleDeactive()
{
    m_work->deactive();
}

void KeyboardModule::reset()
{

}

ModuleWidget *KeyboardModule::moduleWidget()
{
    if(!m_keyboardWidget)
    {
        m_keyboardWidget = new KeyboardWidget(m_model);

#ifndef DCC_DISABLE_KBLAYOUT
        connect(m_keyboardWidget, SIGNAL(keyoard()), this, SLOT(onPushKBDetails()));
#endif
#ifndef DCC_DISABLE_LANGUAGE
        connect(m_keyboardWidget, SIGNAL(language()), this, SLOT(onPushLanguage()));
#endif
        connect(m_keyboardWidget, SIGNAL(shortcut()), this, SLOT(onPushShortcut()));
        connect(m_keyboardWidget, &KeyboardWidget::delayChanged, m_work, &KeyboardWork::setRepeatDelay);
        connect(m_keyboardWidget, &KeyboardWidget::speedChanged, m_work, &KeyboardWork::setRepeatInterval);
        connect(m_keyboardWidget, &KeyboardWidget::numLockChanged, m_work, &KeyboardWork::setNumLock);
        connect(m_keyboardWidget, &KeyboardWidget::capsLockChanged, m_work, &KeyboardWork::setCapsLock);
    }

    return m_keyboardWidget;
}

const QString KeyboardModule::name() const
{
    return QStringLiteral("keyboard");
}

void KeyboardModule::contentPopped(ContentWidget * const w)
{
    if(w == m_shortcutWidget)
        m_shortcutWidget = nullptr;
#ifndef DCC_DISABLE_KBLAYOUT
    else if(w == m_kbDetails)
        m_kbDetails = nullptr;
    else if(w == m_kbLayoutWidget)
        m_kbLayoutWidget = nullptr;
#endif
#ifndef DCC_DISABLE_LANGUAGE
    else if(w == m_langWidget)
        m_langWidget = nullptr;
#endif
    else if(w == m_scContent)
        m_scContent = nullptr;
    else if(w == m_customContent)
        m_customContent = nullptr;

    w->deleteLater();
}

ShortcutInfo *KeyboardModule::checkConflict(const QString &shortcut, QStringList &list)
{
    QString dest = shortcut;
    dest = dest.replace("Control", "Ctrl");
    dest = dest.replace("<", "");
    dest = dest.replace(">", "-");
    QStringList dests = dest.split("-");
    QList<ShortcutInfo*> infos = m_shortcutModel->infos();

    QList<ShortcutInfo*>::iterator itinfo = infos.begin();
    ShortcutInfo* conflict = NULL;
    QStringList checkeds;

    int dests_count = 0;
    for (QString &t : dests){
        if (t == "Ctrl")
            dests_count += m_work->Modifier::control;
        else if (t == "Alt")
            dests_count += m_work->Modifier::alt;
        else if (t == "Super")
            dests_count += m_work->Modifier::super;
        else if (t == "Shift")
            dests_count += m_work->Modifier::shift;
        else {
            QString s = t;
            s = ModelKeycode.value(s);
            if (!s.isEmpty())
                t = s;
        }
    }

    for(; itinfo != infos.end(); ++itinfo)
    {
        ShortcutInfo* item = (*itinfo);
        QString src = item->accels;
        src = src.replace("Control", "Ctrl");
        src = src.replace("<", "");
        src = src.replace(">", "-");
        QStringList srcs = src.split("-");

        int srcs_count = 0;
        for (QString &t : srcs){
            if (t == "Ctrl")
                srcs_count += m_work->Modifier::control;
            else if (t == "Alt")
                srcs_count += m_work->Modifier::alt;
            else if (t == "Super")
                srcs_count += m_work->Modifier::super;
            else if (t == "Shift")
                srcs_count += m_work->Modifier::shift;
            else {
                QString s = t;
                s = ModelKeycode.value(s);
                if (!s.isEmpty())
                    t = s;
            }
        }

        if (srcs_count == dests_count && converKey(dests.last()) == converKey(srcs.last()))
            conflict = item;
    }

    QList<KeyItem*> items = KeyItem::keyboards();
    QList<KeyItem*>::iterator it = items.begin();

    for(; it != items.end(); ++it)
    {
        if(conflict)
        {
            if(checkeds.contains((*it)->mainKey()) || dests.contains((*it)->mainKey()))
            {
                (*it)->setPress(true);
                (*it)->setConflict(true);
            }
            else
            {
                (*it)->setPress(false);
                (*it)->setConflict(false);
            }
        }
        else
        {
            (*it)->setPress(false);
            (*it)->setConflict(false);
        }
    }
    checkeds<<dests;
    list = checkeds;
    return conflict;
}

QString KeyboardModule::converKey(const QString &key)
{
     QString converkey = ModelKeycode.value(key);
     return converkey.isEmpty() ? key.toLower() : converkey;
}

#ifndef DCC_DISABLE_KBLAYOUT
void KeyboardModule::onPushKeyboard()
{
    if(!m_kbLayoutWidget)
    {
        m_work->onPinyin();
        m_kbLayoutWidget = new KeyboardLayoutWidget();
        m_kbLayoutWidget->setMetaData(m_work->getDatas());
        m_kbLayoutWidget->setLetters(m_work->getLetters());

        connect(m_kbLayoutWidget, &KeyboardLayoutWidget::layoutSelected, m_work, &KeyboardWork::addUserLayout);
    }
    m_frameProxy->pushWidget(this, m_kbLayoutWidget);
}

void KeyboardModule::onPushKBDetails()
{

    if(!m_kbDetails)
    {
        m_work->onRefreshKBLayout();
        m_kbDetails = new KeyboardDetails();
        m_kbDetails->setModel(m_model);
        connect(m_kbDetails, SIGNAL(layoutAdded()), this, SLOT(onPushKeyboard()));
        connect(m_kbDetails, SIGNAL(requestCurLayoutAdded(QString)), this, SLOT(setCurrentLayout(QString)));
        connect(m_kbDetails, SIGNAL(delUserLayout(QString)), m_work, SLOT(delUserLayout(QString)));
        connect(m_kbDetails, &KeyboardDetails::requestSwitchKBLayout, m_work, &KeyboardWork::onSetSwitchKBLayout);
    }

    m_frameProxy->pushWidget(this, m_kbDetails);
}
#endif

#ifndef DCC_DISABLE_LANGUAGE
void KeyboardModule::onPushLanguage()
{
    if(!m_langWidget)
    {
        m_langWidget = new LangWidget(m_model);
        connect(m_langWidget, SIGNAL(click(QModelIndex)), this, SLOT(onSetLocale(QModelIndex)));
    }

    m_frameProxy->pushWidget(this, m_langWidget);
}

void KeyboardModule::onSetLocale(const QModelIndex &index)
{
    QVariant var = index.data();
    MetaData md = var.value<MetaData>();

    m_work->setLang(md.key());
}
#endif

void KeyboardModule::onPushShortcut()
{
    if(!m_shortcutWidget)
    {
        m_shortcutWidget = new ShortcutWidget(m_shortcutModel);
        connect(m_shortcutWidget, SIGNAL(shortcutChanged(bool, ShortcutInfo* , QString)), this, SLOT(onShortcutChecked(bool, ShortcutInfo*, QString)));
        connect(m_shortcutWidget, SIGNAL(customShortcut()), this, SLOT(onPushCustomShortcut()));
        connect(m_shortcutWidget, SIGNAL(delShortcutInfo(ShortcutInfo*)), this, SLOT(onDelShortcut(ShortcutInfo*)));
        connect(m_work, SIGNAL(searchChangd(ShortcutInfo*,QString)), m_shortcutWidget, SLOT(onSearchInfo(ShortcutInfo*,QString)));
        connect(m_shortcutWidget, &ShortcutWidget::requestDisableShortcut, m_work, &KeyboardWork::onDisableShortcut);
        connect(m_shortcutWidget, &ShortcutWidget::shortcutEditChanged, this, &KeyboardModule::onShortcutEdit);
        connect(m_work, &KeyboardWork::removed, m_shortcutWidget, &ShortcutWidget::onRemoveItem);
    }
    m_frameProxy->pushWidget(this, m_shortcutWidget);
}

void KeyboardModule::onPushCustomShortcut()
{
    if(!m_customContent)
    {
        m_customContent = new CustomContent(m_work);
        connect(m_customContent, SIGNAL(shortcut(QString)), this, SLOT(onShortcutSet(QString)));
        connect(m_customContent, &CustomContent::requestFrameAutoHide, this, &KeyboardModule::onSetFrameAutoHide);
    }

    m_frameProxy->pushWidget(this, m_customContent);
}

void KeyboardModule::setCurrentLayout(const QString& value)
{
    m_work->setLayout(m_model->userLayout().key(value));
}

void KeyboardModule::onShortcutChecked(bool valid, ShortcutInfo* info, const QString &shortcut)
{
    if(valid)
    {
        m_work->modifyShortcut(info, shortcut);
    }
    else
    {
        QString dest = shortcut;
        dest.replace("Control","Ctrl");
        dest = dest.replace("<", "");
        dest = dest.replace(">", "-");
        QStringList dests = dest.split("-");
        QList<ShortcutInfo*> infos = m_shortcutModel->infos();


        int dests_count = 0;
        for (QString &t : dests){
            if (t == "Ctrl")
                dests_count += m_work->Modifier::control;
            else if (t == "Alt")
                dests_count += m_work->Modifier::alt;
            else if (t == "Super")
                dests_count += m_work->Modifier::super;
            else if (t == "Shift")
                dests_count += m_work->Modifier::shift;
            else {
                QString s = t;
                s = ModelKeycode.value(s);
                if (!s.isEmpty())
                    t = s;
            }
        }

        QList<ShortcutInfo*>::iterator itinfo = infos.begin();
        ShortcutInfo* conflict = NULL;
        QStringList checkeds;
        for(; itinfo != infos.end(); ++itinfo)
        {
            ShortcutInfo* item = (*itinfo);
            QString src = item->accels;
            src = src.replace("Control", "Ctrl");
            src = src.replace("<", "");
            src = src.replace(">", "-");
            QStringList srcs = src.split("-");

            int srcs_count = 0;
            for (QString &t : srcs){
                if (t == "Ctrl")
                    srcs_count += m_work->Modifier::control;
                else if (t == "Alt")
                    srcs_count += m_work->Modifier::alt;
                else if (t == "Super")
                    srcs_count += m_work->Modifier::super;
                else if (t == "Shift")
                    srcs_count += m_work->Modifier::shift;
                else {
                    QString s = t;
                    s = ModelKeycode.value(s);
                    if (!s.isEmpty())
                        t = s;
                }
            }
            if (srcs_count == dests_count && converKey(dests.last()) == converKey(srcs.last()))
                conflict = item;
        }

        QList<KeyItem*> items = KeyItem::keyboards();
        QList<KeyItem*>::iterator it = items.begin();
        for(; it != items.end(); ++it)
        {
//            if(checkeds.contains(dests.last()))
            {
                if(checkeds.contains((*it)->mainKey()) || dests.contains((*it)->mainKey()))
                {
                    (*it)->setPress(true);
                }
                else
                    (*it)->setPress(false);
            }
        }

        if(!m_scContent)
        {
            m_scContent = new ShortcutContent(m_work);
            connect(m_scContent, &ShortcutContent::shortcut, this, &KeyboardModule::onShortcutKeySet);
            checkeds<<dests;
            m_scContent->setConflictString(checkeds);
            if(conflict)
            {
                m_scContent->setBottomTip(conflict);
                m_scContent->setCurInfo(info);
            }
        }
        m_frameProxy->pushWidget(this, m_scContent);
    }
}

void KeyboardModule::onShortcutSet(const QString &shortcut)
{
    QStringList list;
    ShortcutInfo* conflict = checkConflict(shortcut, list);

    if(m_customContent)
    {
        m_customContent->setBottomTip(conflict);
        m_customContent->setConflictString(list);
    }

    if(m_customEdit)
        m_customEdit->setBottomTip(conflict);
}

void KeyboardModule::onShortcutKeySet(const QString &shortcut)
{
    QStringList list;
    ShortcutInfo* conflict = checkConflict(shortcut, list);

    if(m_scContent)
    {
        m_scContent->setBottomTip(conflict);
        m_scContent->setConflictString(list);
    }
}

void KeyboardModule::onDelShortcut(ShortcutInfo *info)
{
    m_work->delShortcut(info);
    m_shortcutModel->delInfo(info);
}

void KeyboardModule::onDelay(int value)
{
    m_work->setRepeatDelay(value);
}

void KeyboardModule::onSpeed(int value)
{
    m_work->setRepeatInterval(value);
}

void KeyboardModule::onShortcutEdit(ShortcutInfo *info)
{
    m_customEdit = new CustomEdit(m_work);
    m_customEdit->setShortcut(info);

    ShortcutWidget *shortcutWidget = static_cast<ShortcutWidget*>(sender());
    SettingsHead *head = shortcutWidget->getHead();

    connect(m_customEdit, &CustomEdit::requestEditFinished, head, &SettingsHead::toCancel);
    connect(m_customEdit, &CustomEdit::requestShortcutList, this, &KeyboardModule::onShortcutSet);
    connect(m_customEdit, &CustomEdit::requestFrameAutoHide, this, &KeyboardModule::onSetFrameAutoHide);

    m_frameProxy->pushWidget(this, m_customEdit);
}

void KeyboardModule::onSetFrameAutoHide(const bool autoHide)
{
    m_frameProxy->setFrameAutoHide(this, autoHide);
}

void KeyboardModule::setCapsLock(bool value)
{
    m_work->setCapsLock(value);
}

KeyboardModule::~KeyboardModule()
{
    m_work->deleteLater();
    m_model->deleteLater();
    m_shortcutModel->deleteLater();
}

}
}
