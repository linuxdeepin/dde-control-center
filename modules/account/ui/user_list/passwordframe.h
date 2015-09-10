#ifndef PASSWORDLINE_H
#define PASSWORDLINE_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include "libdui/dseparatorhorizontal.h"
#include "libdui/dtextbutton.h"
#include "libdui/dheaderline.h"
#include "libdui/dlineedit.h"
#include "libdui/dconstants.h"
#include "confirmbuttonline.h"
#include "passwdline.h"
#include <QDebug>

DUI_USE_NAMESPACE


class PasswordFrame : public QStackedWidget
{
    Q_OBJECT
public:
    explicit PasswordFrame(QWidget *parent = 0);
    void preDestroy();

signals:
    void sizeChanged();
    void changePassword(QString password);

private:
    void onPasswdFocusChanged(bool focus);
    void onPasswdChanged(const QString &);
    void onPasswdRepeatFocusChanged(bool focus);
    void onPasswdRepeatChanged(const QString &passwd);
    void initInactiveWidget();
    void initActiveWidget();
    void resetData();
    bool validate();

private:
    PasswdLine *m_lineNew = NULL;
    PasswdLine *m_lineRepeat = NULL;
    QWidget *m_inactiveWidget = NULL;
    DHeaderLine *m_activeWidget = NULL;
};

#endif // PASSWORDLINE_H
