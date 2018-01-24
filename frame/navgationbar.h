#ifndef NAVGATIONBAR_H
#define NAVGATIONBAR_H

#include <QWidget>

#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

class NavgationBar : public QWidget
{
    Q_OBJECT

public:
    explicit NavgationBar(QWidget *parent = nullptr);

private:
    DImageButton *m_accountBtn;
    DImageButton *m_displayBtn;
    DImageButton *m_defaultAppsBtn;
    DImageButton *m_personalizationBtn;
    DImageButton *m_networkBtn;
    DImageButton *m_bluetoothBtn;
    DImageButton *m_soundBtn;
    DImageButton *m_timeBtn;
    DImageButton *m_powerBtn;
    DImageButton *m_mouseBtn;
    DImageButton *m_keyboardBtn;
    DImageButton *m_updateBtn;
    DImageButton *m_sysInfoBtn;
};

#endif // NAVGATIONBAR_H
