#ifndef KEYBOARDWORK_H
#define KEYBOARDWORK_H

#include <QObject>
#include <com_deepin_daemon_inputdevice_keyboard.h>

using KeyboardInter = com::deepin::daemon::inputdevice::Keyboard;

class KeyboardWork : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardWork(QObject *parent = 0);

signals:

public slots:

private:
    KeyboardInter* m_keyboardInter;
};

#endif // KEYBOARDWORK_H
