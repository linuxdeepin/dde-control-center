#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>

class NavigationBar : public QWidget
{
    Q_OBJECT

public:
    explicit NavigationBar(QWidget *parent = nullptr);

signals:
    void requestModule(const QString &name) const;

private:
    QButtonGroup *m_navigationsGroup;
    QPushButton *m_accountBtn;
    QPushButton *m_displayBtn;
    QPushButton *m_defaultAppsBtn;
    QPushButton *m_personalizationBtn;
    QPushButton *m_networkBtn;
    QPushButton *m_bluetoothBtn;
    QPushButton *m_soundBtn;
    QPushButton *m_timeBtn;
    QPushButton *m_powerBtn;
    QPushButton *m_mouseBtn;
    QPushButton *m_keyboardBtn;
    QPushButton *m_updateBtn;
    QPushButton *m_sysInfoBtn;
};

#endif // NAVIGATIONBAR_H
