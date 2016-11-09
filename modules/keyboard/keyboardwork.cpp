#include "keyboardwork.h"
#include <QDebug>
KeyboardWork::KeyboardWork(QObject *parent)
    : QObject(parent)
{
    m_keyboardInter = new KeyboardInter("com.deepin.daemon.InputDevices",
                                        "/com/deepin/daemon/InputDevice/Keyboard",
                                        QDBusConnection::sessionBus(), this);

    m_langSelector = new LangSelector("com.deepin.daemon.LangSelector",
                                      "/com/deepin/daemon/LangSelector",
                                      QDBusConnection::sessionBus(), this);


    m_keyboardInter->setSync(false);
    m_langSelector->setSync(false);

    connect(m_keyboardInter, SIGNAL(UserLayoutListChanged(QStringList)), this, SIGNAL(UserLayoutListChanged(QStringList)));
    connect(m_keyboardInter, SIGNAL(CurrentLayoutChanged(QString)), this, SIGNAL(curLayout(QString)));
}

KeyboardLayoutList KeyboardWork::layoutLists() const
{
    QDBusPendingReply<KeyboardLayoutList> list = m_keyboardInter->LayoutList();
    KeyboardLayoutList tmp_map = list.value();

    return tmp_map;
}

void KeyboardWork::addUserLayout(const QString &value)
{
    m_keyboardInter->AddUserLayout(value);

    emit addLayout(value);
}

void KeyboardWork::delUserLayout(const QString &value)
{
    m_keyboardInter->DeleteUserLayout(value);

    emit delLayout(value);
}

QStringList KeyboardWork::userLayout() const
{
    return m_keyboardInter->userLayoutList();
}

void KeyboardWork::setLayout(const QString &value)
{
    m_keyboardInter->setCurrentLayout(value);
    emit curLayout(value);
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
