#include "keyboardwork.h"
#include "shortcutitem.h"
#include "keyboardmodel.h"
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
#ifndef DCC_DISABLE_LANGUAGE
      m_langSelector(new LangSelector("com.deepin.daemon.LangSelector",
                                      "/com/deepin/daemon/LangSelector",
                                      QDBusConnection::sessionBus(), this)),
#endif
      m_keybindInter(new KeybingdingInter("com.deepin.daemon.Keybinding",
                                          "/com/deepin/daemon/Keybinding",
                                          QDBusConnection::sessionBus(), this))
{
    connect(m_keybindInter, SIGNAL(Added(QString,int)), this,SLOT(onAdded(QString,int)));
    connect(m_keybindInter, &KeybingdingInter::Deleted, this, &KeyboardWork::removed);
#ifndef DCC_DISABLE_KBLAYOUT
    connect(m_keyboardInter, &KeyboardInter::UserLayoutListChanged, this, &KeyboardWork::onUserLayout);
    connect(m_keyboardInter, &KeyboardInter::CurrentLayoutChanged, this, &KeyboardWork::onCurrentLayout);
#endif
    connect(m_keyboardInter, SIGNAL(CapslockToggleChanged(bool)), m_model, SLOT(setCapsLock(bool)));
    connect(m_keybindInter, &KeybingdingInter::NumLockStateChanged, m_model, &KeyboardModel::setNumLock);
    connect(m_keybindInter, &KeybingdingInter::KeyEvent, this, &KeyboardWork::KeyEvent);
#ifndef DCC_DISABLE_LANGUAGE
    connect(m_langSelector, &LangSelector::CurrentLocaleChanged, m_model, &KeyboardModel::setLang);
#endif
    connect(m_keyboardInter, &KeyboardInter::RepeatDelayChanged, this, &KeyboardWork::setModelRepeatDelay);
    connect(m_keyboardInter, &KeyboardInter::RepeatIntervalChanged, this, &KeyboardWork::setModelRepeatInterval);
    connect(m_keybindInter, &KeybingdingInter::ShortcutSwitchLayoutChanged, m_model, &KeyboardModel::setKbSwitch);

    m_keyboardInter->setSync(false);
    m_keybindInter->setSync(false);
#ifndef DCC_DISABLE_LANGUAGE
    m_langSelector->setSync(false);
#endif
}

void KeyboardWork::active()
{
    m_keyboardInter->blockSignals(false);
#ifndef DCC_DISABLE_LANGUAGE
    m_langSelector->blockSignals(false);
#endif
    m_keybindInter->blockSignals(false);

    setModelRepeatDelay(m_keyboardInter->repeatDelay());
    setModelRepeatInterval(m_keyboardInter->repeatInterval());

    m_metaDatas.clear();
    m_letters.clear();

    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_keybindInter->List(), this);
    connect(result, SIGNAL(finished(QDBusPendingCallWatcher*)), this,
            SLOT(onRequestShortcut(QDBusPendingCallWatcher*)));

#ifndef DCC_DISABLE_LANGUAGE
    QDBusPendingCallWatcher *localResult = new QDBusPendingCallWatcher(m_langSelector->GetLocaleList(), this);
    connect(localResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWork::onLocalListsFinished);
#endif

    m_model->setCapsLock(m_keyboardInter->capslockToggle());
    m_model->setNumLock(m_keybindInter->numLockState());
    m_keyboardInter->currentLayout();
#ifndef DCC_DISABLE_LANGUAGE
    m_langSelector->currentLocale();
#endif
    m_keyboardInter->userLayoutList();
}

void KeyboardWork::deactive()
{
    m_keyboardInter->blockSignals(true);
#ifndef DCC_DISABLE_LANGUAGE
    m_langSelector->blockSignals(true);
#endif
    m_keybindInter->blockSignals(true);
}

bool KeyboardWork::keyOccupy(const QStringList &list)
{
    int bit = 0;
    for (QString t : list) {
        if (t == "Control")
            bit +=  Modifier::control;
        else if (t == "Alt")
            bit += Modifier::alt;
        else if (t == "Super")
            bit += Modifier::super;
        else if (t == "Shift")
            bit += Modifier::shift;
        else
            continue;
    }

    QMap<QStringList,int> keylist = m_model->allShortcut();
    QMap<QStringList, int>::iterator i;
    for (i = keylist.begin(); i != keylist.end(); ++i) {
        if (bit == i.value() && i.key().last() == list.last()) {
            return false;
        }
    }

    return true;
}

#ifndef DCC_DISABLE_KBLAYOUT
void KeyboardWork::onRefreshKBLayout()
{
    m_model->setKbSwitch(m_keybindInter->shortcutSwitchLayout());

    QDBusPendingCallWatcher *layoutResult = new QDBusPendingCallWatcher(m_keyboardInter->LayoutList(), this);
    connect(layoutResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWork::onLayoutListsFinished);
}
#endif

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

void KeyboardWork::modifyShortcutEdit(ShortcutInfo *info)
{
    if (!info)
        return;

    if (info->accels != tr("None"))
        m_keybindInter->ModifyCustomShortcut(info->id, info->name, info->command, info->accels);
    else
        m_keybindInter->ModifyCustomShortcut(info->id, info->name, info->command, QString());
}

void KeyboardWork::addCustomShortcut(const QString &name, const QString &command, const QString &accels, bool &result)
{
    if (accels.isEmpty())
        m_keybindInter->Add(name, command, tr("None"), result);
    else
        m_keybindInter->Add(name, command, accels, result);
}

void KeyboardWork::grabScreen()
{
    m_keybindInter->GrabScreen();
}

bool KeyboardWork::checkAvaliable(const QString &key)
{
    return m_keybindInter->CheckAvaliable(key);
}

void KeyboardWork::delShortcut(ShortcutInfo* info)
{
    m_keybindInter->Delete(info->id, info->type);
}

void KeyboardWork::setRepeatDelay(int value)
{
    m_keyboardInter->setRepeatDelay(converToDBusDelay(value));
}

void KeyboardWork::setRepeatInterval(int value)
{
    m_keyboardInter->setRepeatInterval(converToDBusInterval(value));
}

void KeyboardWork::setModelRepeatDelay(int value)
{
    m_model->setRepeatDelay(converToModelDelay(value));
}

void KeyboardWork::setModelRepeatInterval(int value)
{
    m_model->setRepeatInterval(converToModelInterval(value));
}

void KeyboardWork::setNumLock(bool value)
{
    m_keybindInter->SetNumLockState(value);
}

void KeyboardWork::setCapsLock(bool value)
{
    m_keyboardInter->setCapslockToggle(value);
}

void KeyboardWork::addUserLayout(const QString &value)
{
    m_keyboardInter->AddUserLayout(m_model->kbLayout().key(value));
}

void KeyboardWork::delUserLayout(const QString &value)
{
    m_keyboardInter->DeleteUserLayout(m_model->userLayout().key(value));
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

    QMap<QStringList,int> map;
    QJsonArray array = QJsonDocument::fromJson(info.toStdString().c_str()).array();
    foreach(QJsonValue value, array) {
        QJsonObject obj = value.toObject();
        if (obj.isEmpty())
            continue;
        if (obj["Accels"].toArray().isEmpty())
            continue;
        QString accels = obj["Accels"].toArray().at(0).toString();
        accels.replace("<", "");
        accels.replace(">", "-");
        //转换为list
        QStringList key;
        key = accels.split("-");
        int bit = 0;
        for (QString &t : key) {
            if (t == "Control")
                bit += Modifier::control;
            else if (t == "Alt")
                bit += Modifier::alt;
            else if (t == "Super")
                bit += Modifier::super;
            else if (t == "Shift")
                bit += Modifier::shift;
            else {
                QString s = t;
                s = ModelKeycode.value(s);
                if (!s.isEmpty())
                    t = s;
            }
        }
        if (bit == 0)
            continue;

        map.insert(key, bit);
    }
    m_model->setAllShortcut(map);
    watch->deleteLater();
}

void KeyboardWork::onAdded(const QString &in0, int in1)
{
    QDBusPendingReply<QString> reply = m_keybindInter->Query(in0, in1);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &KeyboardWork::onAddedFinished);
}

void KeyboardWork::onDisableShortcut(ShortcutInfo *info)
{
    m_keybindInter->Disable(info->id, info->type);
    info->accels = tr("None");
}

void KeyboardWork::onAddedFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;
    emit customInfo(reply.value());
    watch->deleteLater();
}

#ifndef DCC_DISABLE_KBLAYOUT
void KeyboardWork::onLayoutListsFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<KeyboardLayoutList> reply = *watch;

    KeyboardLayoutList tmp_map = reply.value();
    m_model->setLayoutLists(tmp_map);

    watch->deleteLater();
}
#endif

void KeyboardWork::onLocalListsFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<LocaleList> reply = *watch;

    m_datas.clear();

    LocaleList list = reply.value();

    for (int i = 0; i!=list.size(); ++i) {
        MetaData md;
        md.setKey(list.at(i).id);
        md.setText(list.at(i).name);
        m_datas.append(md);
    }

    qSort(m_datas.begin(), m_datas.end(), caseInsensitiveLessThan);

    m_model->setLocaleList(m_datas);

    watch->deleteLater();
}

void KeyboardWork::onSetSwitchKBLayout(int value)
{
    m_keybindInter->setShortcutSwitchLayout(value);
}

#ifndef DCC_DISABLE_KBLAYOUT
void KeyboardWork::onUserLayout(const QStringList &list)
{
    m_model->cleanUserLayout();

    for (const QString &data : list) {
        QDBusPendingCallWatcher *layoutResult = new QDBusPendingCallWatcher(m_keyboardInter->GetLayoutDesc(data), this);
        layoutResult->setProperty("id", data);
        connect(layoutResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWork::onUserLayoutFinished);
    }
}

void KeyboardWork::onUserLayoutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    m_model->addUserLayout(watch->property("id").toString(), reply.value());

    watch->deleteLater();
}

void KeyboardWork::onCurrentLayout(const QString &value)
{
    QDBusPendingCallWatcher *layoutResult = new QDBusPendingCallWatcher(m_keyboardInter->GetLayoutDesc(value), this);
    connect(layoutResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWork::onCurrentLayoutFinished);
}

void KeyboardWork::onCurrentLayoutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    m_model->setLayout(reply.value());

    watch->deleteLater();
}

void KeyboardWork::onPinyin()
{
    m_letters.clear();
    m_metaDatas.clear();
    QDBusInterface dbus_pinyin("com.deepin.api.Pinyin", "/com/deepin/api/Pinyin",
                               "com.deepin.api.Pinyin");

    foreach(const QString & str, m_model->kbLayout().keys()) {
        MetaData md;
        QString title = m_model->kbLayout()[str];
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

    QLocale locale;

    if (locale.language() == QLocale::Chinese) {
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
    } else {
        qSort(m_metaDatas.begin(), m_metaDatas.end(), caseInsensitiveLessThan);
    }
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
#endif

int KeyboardWork::converToDBusDelay(int value)
{
    switch (value) {
    case 1:
        return 20;
    case 2:
        return 80;
    case 3:
        return 150;
    case 4:
        return 250;
    case 5:
        return 360;
    case 6:
        return 480;
    case 7:
        return 600;
    default:
        return 4;
    }
}

int KeyboardWork::converToModelDelay(int value)
{
    if (value <= 20)
        return 1;
    else if (value <= 80)
        return 2;
    else if (value <= 150)
        return 3;
    else if (value <= 250)
        return 4;
    else if (value <= 360)
        return 5;
    else if (value <= 480)
        return 6;
    else
        return 7;
}

int KeyboardWork::converToDBusInterval(int value)
{
    switch (value) {
    case 1:
        return 100;
    case 2:
        return 80;
    case 3:
        return 65;
    case 4:
        return 50;
    case 5:
        return 35;
    case 6:
        return 25;
    case 7:
        return 20;
    default:
        return 4;
    }
}

int KeyboardWork::converToModelInterval(int value)
{
    if (value <= 20)
        return 7;
    else if (value <= 25)
        return 6;
    else if (value <= 35)
        return 5;
    else if (value <= 50)
        return 4;
    else if (value <= 65)
        return 3;
    else if (value <= 80)
        return 2;
    else
        return 1;
}

void KeyboardWork::setLayout(const QString &value)
{
    m_keyboardInter->setCurrentLayout(value);
}

#ifndef DCC_DISABLE_LANGUAGE
void KeyboardWork::setLang(const QString &value)
{
    m_langSelector->SetLocale(value);
}
#endif

}
}
