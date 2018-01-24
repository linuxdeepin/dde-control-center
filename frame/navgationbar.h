#ifndef NAVGATIONBAR_H
#define NAVGATIONBAR_H

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>

class NavgationBar : public QWidget
{
    Q_OBJECT

public:
    explicit NavgationBar(QWidget *parent = nullptr);

signals:
    void requestModule(const QString &name) const;

private:
    QButtonGroup *m_navgationsGroup;
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

#endif // NAVGATIONBAR_H
