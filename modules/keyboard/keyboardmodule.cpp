#include "keyboardmodule.h"
#include "keyitem.h"

KeyboardModule::KeyboardModule(FrameProxyInterface *frame, QObject *parent)
    :QObject(parent),
      ModuleInterface(frame),
      m_loaded(false),
      m_keyboardWidget(nullptr),
      m_kbDetails(nullptr),
      m_kbLayoutWidget(nullptr),
      m_shortcutWidget(nullptr),
      m_langWidget(nullptr),
      m_scContent(nullptr)
{
}

void KeyboardModule::initialize()
{
    m_model = new KeyboardModel();
    m_shortcutModel = new ShortcutModel();
    m_work = new KeyboardWork(m_model);

    connect(m_work, SIGNAL(curLang(QString)), m_model, SLOT(setLang(QString)));
    connect(m_work, SIGNAL(curLayout(QString)), m_model, SLOT(setLayout(QString)));
    connect(m_work, SIGNAL(addLayout(QString)), m_model, SLOT(addUserLayout(QString)));
    connect(m_work,SIGNAL(delLayout(QString)), m_model, SLOT(delUserLayout(QString)));
    connect(m_work, SIGNAL(langValid(QList<MetaData>)), m_model, SLOT(setLocaleList(QList<MetaData>)));
    connect(m_work, SIGNAL(UserLayoutListChanged(QStringList)), m_model, SLOT(setUserLayout(QStringList)));
    connect(m_work, SIGNAL(shortcutInfo(QString)), m_shortcutModel, SLOT(onParseInfo(QString)));
    connect(m_shortcutModel, SIGNAL(parseFinish()), this, SLOT(onParseFinish()));

    m_work->getProperty();

    m_datas.clear();
    m_letters.clear();

    KeyboardLayoutList tmp_map = m_work->layoutLists();
    QDBusInterface dbus_pinyin("com.deepin.api.Pinyin", "/com/deepin/api/Pinyin",
                               "com.deepin.api.Pinyin");

    foreach(const QString & str, tmp_map.keys()) {
        MetaData md;
        QString title = tmp_map[str];
        md.setText(title);
        md.setKey(str);
        QChar letterFirst = title[0];
        QStringList letterFirstList;
        if (letterFirst.isLower() || letterFirst.isUpper()) {
            letterFirstList << QString(letterFirst);
            md.setPinyin(title);
        } else {
            QDBusMessage message = dbus_pinyin.call("Query", title);
            letterFirstList = message.arguments()[0].toStringList();
            md.setPinyin(letterFirstList.at(0));
        }

        append(md);
    }

    QList<MetaData>::iterator it = m_datas.end();
    --it;
    int index = m_datas.count() - 1;
    QChar ch = (*it).pinyin().at(0).toUpper();
    for(; it != m_datas.begin(); --it)
    {
        QChar pre = (*it).pinyin().at(0).toUpper();
        if(pre != ch)
        {
            m_datas.insert(index+1, MetaData(ch, true));
            m_letters.prepend(ch);
            ch = pre;
        }
        index--;
    }

    ch = (*it).pinyin().at(0).toUpper();
    m_datas.insert(index, MetaData(ch, true));
    m_letters.prepend(ch);

    m_model->setLayoutLists(m_work->layoutLists());
    m_model->setLayout(m_work->curLayout());
    m_model->setUserLayout(m_work->userLayout());

    m_model->moveToThread(qApp->thread());
    m_shortcutModel->moveToThread(qApp->thread());
    m_work->moveToThread(qApp->thread());
}

void KeyboardModule::moduleActive()
{

}

void KeyboardModule::moduleDeactive()
{

}

void KeyboardModule::reset()
{

}

ModuleWidget *KeyboardModule::moduleWidget()
{
    if(!m_keyboardWidget)
    {
        m_keyboardWidget = new KeyboardWidget();
        connect(m_keyboardWidget, SIGNAL(keyoard()), this, SLOT(onPushKBDetails()));
        connect(m_keyboardWidget, SIGNAL(language()), this, SLOT(onPushLanguage()));
        connect(m_keyboardWidget, SIGNAL(shortcut()), this, SLOT(onPushShortcut()));
    }

    return m_keyboardWidget;
}

const QString KeyboardModule::name() const
{
    return "Keyboard";
}

void KeyboardModule::contentPopped(ContentWidget * const w)
{
    if(w == m_kbDetails)
        m_kbDetails = nullptr;
    else if(w == m_kbLayoutWidget)
        m_kbLayoutWidget = nullptr;
    else if(w == m_shortcutWidget)
        m_shortcutWidget = nullptr;
    else if(w == m_langWidget)
        m_langWidget = nullptr;
    else if(w == m_scContent)
        m_scContent = nullptr;

    w->deleteLater();
}

void KeyboardModule::onPushKeyboard()
{
    if(!m_kbLayoutWidget)
    {
        m_kbLayoutWidget = new KeyboardLayoutWidget();
        m_kbLayoutWidget->setMetaData(m_datas);
        m_kbLayoutWidget->setLetters(m_letters);
        connect(m_kbLayoutWidget, SIGNAL(back()), this, SLOT(onKeyboardBack()));
    }
    m_frameProxy->pushWidget(this, m_kbLayoutWidget);
}

void KeyboardModule::onPushKBDetails()
{
    if(!m_kbDetails)
    {
        m_kbDetails = new KeyboardDetails();
        QStringList userlayout = m_model->userLayout();
        QString cur = m_model->curLayout();
        for(int i = 0; i<userlayout.count(); i++)
        {
            MetaData md;
            QString key = userlayout.at(i);
            QString text = m_model->layoutByValue(key);

            md.setKey(key);
            md.setText(text);
            md.setSelected(text == cur);
            m_kbDetails->onAddKeyboard(md);
        }
        connect(m_kbDetails, SIGNAL(layoutAdded()), this, SLOT(onPushKeyboard()));
        connect(m_kbDetails, SIGNAL(curLayout(QString)), this, SLOT(setCurrentLayout(QString)));
        connect(m_kbDetails, SIGNAL(delUserLayout(QString)), m_work, SLOT(delUserLayout(QString)));
    }

    m_frameProxy->pushWidget(this, m_kbDetails);
}

void KeyboardModule::onPushLanguage()
{
    if(!m_model->langLists().count())
        return;

    if(!m_langWidget)
    {
        m_langWidget = new LangWidget();
        m_langWidget->setModelData(m_model->langLists());
        connect(m_langWidget, SIGNAL(click(QModelIndex)), this, SLOT(onSetLocale(QModelIndex)));
    }

    m_frameProxy->pushWidget(this, m_langWidget);
}

void KeyboardModule::onPushShortcut()
{
    if(!m_loaded)
    {
        return;
    }

    if(!m_shortcutWidget)
    {
        m_shortcutWidget = new ShortcutWidget();
        connect(m_shortcutWidget, SIGNAL(shortcutChanged(QString)), this, SLOT(onShortcutChecked(QString)));
    }
    m_shortcutWidget->addShortcut(m_shortcutModel->systemInfo(), ShortcutWidget::System);
    m_shortcutWidget->addShortcut(m_shortcutModel->windowInfo(), ShortcutWidget::Window);
    m_shortcutWidget->addShortcut(m_shortcutModel->workspaceInfo(), ShortcutWidget::Workspace);

    m_frameProxy->pushWidget(this, m_shortcutWidget);
}

void KeyboardModule::onPushShortcutControl(const QString &shortcut)
{
    Q_UNUSED(shortcut);
}

void KeyboardModule::onKeyboardBack()
{
    if(m_kbLayoutWidget)
    {
        QList<MetaData> datas = m_kbLayoutWidget->selectData();
        for(int i = 0; i<datas.count(); i++)
        {
            if(m_kbDetails)
            {
                MetaData data = datas.at(i);
                m_kbDetails->onAddKeyboard(data);
                m_work->addUserLayout(data.key());
            }
        }
    }
}

void KeyboardModule::onParseFinish()
{
    m_loaded = true;
}

void KeyboardModule::setCurrentLayout(const QString& value)
{
    m_work->setLayout(value);
}

void KeyboardModule::setCurrentLang()
{

}

void KeyboardModule::onSetLocale(const QModelIndex &index)
{
    QVariant var = index.data();
    MetaData md = var.value<MetaData>();

    qDebug()<<Q_FUNC_INFO<<md.key();
    m_work->setLang(md.key());
}

void KeyboardModule::onShortcutChecked(const QString &shortcut)
{
    QString dest = shortcut;
    dest = dest.replace("<", "");
    dest = dest.replace(">", "-");
    QStringList dests = dest.split("-");
    QList<ShortcutInfo*> infos = m_shortcutModel->infos();

    QList<ShortcutInfo*>::iterator itinfo = infos.begin();
    QStringList checkeds;
    for(; itinfo != infos.end(); ++itinfo)
    {
        ShortcutInfo* info = (*itinfo);
        QString src = info->accels;
        src.replace("Control", "Ctrl");
        src = src.replace("<", "");
        src = src.replace(">", "-");
        QStringList srcs = src.split("-");
        if(srcs.count() == dests.count())
        {
            int score = 0;
            for(int i = 0; i< dests.count() - 1; i++)
            {
                if(srcs.contains(dests.at(i)))
                {
                    score++;
                }
            }

            if(score == dests.count() - 1)
            {
                checkeds<<srcs.last();
            }
        }
    }

    QList<KeyItem*> items = KeyItem::keyboards();
    QList<KeyItem*>::iterator it = items.begin();
    for(; it != items.end(); ++it)
    {
//        if(checkeds.contains(dests.last()))
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
        m_scContent = new ShortcutContent();
    }
    m_frameProxy->pushWidget(this, m_scContent);
}

KeyboardModule::~KeyboardModule()
{
    m_work->deleteLater();
    m_model->deleteLater();

    if(m_keyboardWidget)
        m_keyboardWidget->deleteLater();
}

void KeyboardModule::append(const MetaData &md)
{
    if(m_datas.count() == 0)
    {
        m_datas.append(md);
        return;
    }

    int index = 0;
    QList<MetaData>::iterator it = m_datas.begin();

    for(; it != m_datas.end(); ++it)
    {
        if((*it)>md)
        {
            m_datas.insert(index,md);
            return;
        }
        index++;
    }
    m_datas.append(md);
}
