#ifndef KEYBOARDWORK_H
#define KEYBOARDWORK_H

#include <QObject>
#include "indexmodel.h"
#include <com_deepin_daemon_inputdevice_keyboard.h>
#include <com_deepin_daemon_langselector.h>

using KeyboardInter = com::deepin::daemon::inputdevice::Keyboard;
using LangSelector = com::deepin::daemon::LangSelector;

class KeyboardWork : public QObject
{
    Q_OBJECT

public:
    explicit KeyboardWork(QObject *parent = 0);

    KeyboardLayoutList layoutLists() const;
    QStringList userLayout() const;
    QString curLayout() const;
    QString curLang() const;

signals:
    void curLayout(const QString& layout);
    void curLang(const QString& lang);
    void delLayout(const QString& layout);
    void addLayout(const QString& layout);
    void UserLayoutListChanged(const QStringList & value) const;

public slots:
    void setLang(const QString& value);
    void setLayout(const QString& value);
    void addUserLayout(const QString& value);
    void delUserLayout(const QString& value);

private:
    KeyboardInter* m_keyboardInter;
    LangSelector* m_langSelector;

};

#endif // KEYBOARDWORK_H
