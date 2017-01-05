#include "keyboardwork.h"
#include "shortcutitem.h"
#include <QTime>
#include <QDebug>
#include <QLocale>
#include <QCollator>


namespace dcc {
namespace keyboard{


bool caseInsensitiveLessThan(const MetaData &s1, const MetaData &s2);

KeyboardWork::KeyboardWork(KeyboardModel *model, QObject *parent)
    : QObject(parent),
      m_model(model),
      m_keyboardInter(new KeyboardInter("com.deepin.daemon.InputDevices",
                                        "/com/deepin/daemon/InputDevice/Keyboard",
                                        QDBusConnection::sessionBus(), this)),
      m_langSelector(new LangSelector("com.deepin.daemon.LangSelector",
                                      "/com/deepin/daemon/LangSelector",
                                      QDBusConnection::sessionBus(), this)),
      m_keybindInter(new KeybingdingInter("com.deepin.daemon.Keybinding",
                                          "/com/deepin/daemon/Keybinding",
                                          QDBusConnection::sessionBus(), this))
{
    connect(m_keybindInter, SIGNAL(Added(QString,int)), this,SLOT(onAdded(QString,int)));
    connect(m_keybindInter, SIGNAL(KeyEvent(bool,QString)), this, SIGNAL(KeyEvent(bool,QString)));
    connect(m_keyboardInter, SIGNAL(UserLayoutListChanged(QStringList)), m_model, SLOT(setUserLayout(QStringList)));
    connect(m_keyboardInter, SIGNAL(CurrentLayoutChanged(QString)), m_model, SLOT(setLayout(QString)));
    connect(m_langSelector, SIGNAL(CurrentLocaleChanged(QString)), m_model, SLOT(setLang(QString)));
    connect(m_keyboardInter, SIGNAL(CapslockToggleChanged(bool)), m_model, SLOT(setCapsLock(bool)));

    getProperty();

    m_keyboardInter->setSync(false);
    m_keybindInter->setSync(false);

    if (m_langSelector->isValid()) {
        onValid();
    } else {
        m_langSelector->setSync(false);
    }

    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_keybindInter->List(), this);
    connect(result, SIGNAL(finished(QDBusPendingCallWatcher*)), this,
            SLOT(onRequestShortcut(QDBusPendingCallWatcher*)));
}

void KeyboardWork::active()
{
    m_keyboardInter->blockSignals(false);
    m_langSelector->blockSignals(false);
    m_keybindInter->blockSignals(false);

    onValid();

    m_model->setCapsLock(m_keyboardInter->capslockToggle());
    m_model->setLang(curLang());
}

void KeyboardWork::deactive()
{
    m_keyboardInter->blockSignals(true);
    m_langSelector->blockSignals(true);
    m_keybindInter->blockSignals(true);
}

void KeyboardWork::getProperty()
{
    m_metaDatas.clear();
    m_letters.clear();

    KeyboardLayoutList tmp_map = layoutLists();
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

    QChar ch = '\0';
    for (int i(0); i != m_metaDatas.size(); ++i)
    {
        const QChar flag = m_metaDatas[i].pinyin().at(0).toUpper();
        if (flag == ch)
            continue;
        ch = flag;

        m_letters.append(ch);
        m_metaDatas.insert(i, MetaData(ch, true));
    }

    m_model->setLayoutLists(layoutLists());
    m_model->setLayout(curLayout());
    m_model->setUserLayout(userLayout());
}

KeyboardLayoutList KeyboardWork::layoutLists() const
{
    QDBusPendingReply<KeyboardLayoutList> list = m_keyboardInter->LayoutList();
    KeyboardLayoutList tmp_map = list.value();

    return tmp_map;
}

void KeyboardWork::modifyShortcut(ShortcutInfo *info, const QString &key, bool clear)
{
    if (!info) {
        return;
    }
    QString str;
    QString accels = info->accels;
    if (info->accels != tr("None")) {
        bool result = m_keybindInter->ModifiedAccel(info->id, info->type, info->accels, false, str).value();//remove
        if(!result)
        {
            info->accels = str;
            if(clear && info->item)
                info->item->repaint();
        }

    }
    if(!clear)
    {
        if (!key.isEmpty() && key != tr("None")) {
            bool result = m_keybindInter->ModifiedAccel(info->id, info->type, key, true, str).value();
            if(!result)
            {
                info->accels = key;
                if(info->item)
                    info->item->repaint();
                emit searchChangd(info, info->name+accels);
            }
        }
    }
}

bool KeyboardWork::addCustonShortcut(const QString &name, const QString &command, const QString &accels, bool &result)
{
    m_keybindInter->Add(name, command, accels, result);

    return !result;
}

void KeyboardWork::grabScreen()
{
    m_keybindInter->GrabScreen();
}

bool KeyboardWork::checkAvaliable(const QString &key)
{
    return m_keybindInter->CheckAvaliable(key);
}

QString KeyboardWork::query(const QString &id, qint32 type)
{
    QDBusPendingReply<QString> reply = m_keybindInter->Query(id, type);
    reply.waitForFinished();
    return reply.value();
}

void KeyboardWork::delShortcut(ShortcutInfo* info)
{
    m_keybindInter->Delete(info->id, info->type);
}

void KeyboardWork::setRepeatDelay(int value)
{
    m_keyboardInter->setRepeatDelay(value);
}

uint KeyboardWork::repeatDelay() const
{
    return m_keyboardInter->repeatDelay();
}

void KeyboardWork::setRepeatInterval(int value)
{
    m_keyboardInter->setRepeatInterval(((1000 - value) + 200) / 10);
}

uint KeyboardWork::repeatInterval() const
{
    return m_keyboardInter->repeatInterval();
}

void KeyboardWork::setCapsLock(bool value)
{
    m_keyboardInter->setCapslockToggle(value);
}

void KeyboardWork::addUserLayout(const QString &value)
{
    m_keyboardInter->AddUserLayout(value);

    m_model->addUserLayout(value);
}

void KeyboardWork::delUserLayout(const QString &value)
{
    m_keyboardInter->DeleteUserLayout(value);

    m_model->delUserLayout(value);
}

void KeyboardWork::onValid()
{
    if (!m_langSelector->isValid())
        return;

    m_datas.clear();
    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_langSelector->GetLocaleList(), this);
    connect(result, SIGNAL(finished(QDBusPendingCallWatcher*)), this,
            SLOT(onLocaleListFinish(QDBusPendingCallWatcher*)));
}

void KeyboardWork::onLocaleListFinish(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<LocaleList> reply = *watch;

    if (reply.isError()) {
        qDebug()<<Q_FUNC_INFO<<reply.error().message();
        watch->deleteLater();
        return;
    }

    QString key = m_langSelector->currentLocale();
    m_model->setLang(key);
    LocaleList list = reply.value();

    for (int i = 0; i!=list.size(); ++i) {
        MetaData md;
        md.setKey(list.at(i).id);
        md.setText(list.at(i).name);
        m_datas.append(md);
    }

    qSort(m_datas.begin(), m_datas.end(), caseInsensitiveLessThan);

    m_model->setLocaleList(m_datas);
    emit requestSetLangTitle(m_model->langByKey(key));
    watch->deleteLater();
}

bool caseInsensitiveLessThan(const MetaData &s1, const MetaData &s2)
{
    QCollator qc;
    int i = qc.compare(s1.text(), s2.text());
    if (i < 0)
        return true;
    else
        return false;
}

void KeyboardWork::onRequestShortcut(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;
    if(reply.isError())
    {
        watch->deleteLater();
        return;
    }

    QString info = reply.value();

    emit shortcutInfo(info);
    watch->deleteLater();
}

void KeyboardWork::onAdded(const QString &in0, int in1)
{
    emit custonInfo(m_keybindInter->Query(in0, in1));
}

void KeyboardWork::append(const MetaData &md)
{
    if(m_metaDatas.count() == 0)
    {
        m_metaDatas.append(md);
        return;
    }

    int index = 0;
    for (int i = 0; i != m_metaDatas.size(); ++i) {
        if(m_metaDatas.at(i) > md)
        {
            m_metaDatas.insert(index,md);
            return;
        }
        index++;
    }

    m_metaDatas.append(md);
}

QStringList KeyboardWork::userLayout() const
{
    return m_keyboardInter->userLayoutList();
}

void KeyboardWork::setLayout(const QString &value)
{
    m_keyboardInter->setCurrentLayout(value);
    m_model->setLayout(value);
}

QString KeyboardWork::curLayout() const
{
    QString str =m_keyboardInter->currentLayout();

    return str;
}

void KeyboardWork::setLang(const QString &value)
{
    m_langSelector->SetLocale(value);
}

QString KeyboardWork::curLang() const
{
    return m_langSelector->currentLocale();
}
}
}
