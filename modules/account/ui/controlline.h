#ifndef CONTROLLINE_H
#define CONTROLLINE_H

#include "passwdline.h"
#include "switchline.h"

#include "libdui/dlineedit.h"
#include "libdui/dtextbutton.h"
#include "libdui/dheaderline.h"
#include "libdui/dsegmentedcontrol.h"
#include "libdui/dseparatorhorizontal.h"

DUI_USE_NAMESPACE

//for special style
class AccountHeaderLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit AccountHeaderLine(QWidget *parent = 0) : DHeaderLine(parent) {D_THEME_INIT_WIDGET(AccountHeaderLine);}
};

class AccountPasswdLine : public AccountHeaderLine
{
    Q_OBJECT
public:
    explicit AccountPasswdLine(QWidget *parent = 0);
    ~AccountPasswdLine();
    void hideWarning();
    void showWarning(const QString &msg);
    void setText(const QString &text);
    QString text() const;
    DPasswordEdit *passwordEdit() const;

signals:
    void textChanged(const QString &text);
    void focusChanged(bool focus);

private:
    InputWarningFrame *m_warningFrame = NULL;
    DPasswordEdit *m_passwordEdit = NULL;
};



class AccountSwitchLine : public AccountHeaderLine
{
    Q_OBJECT
public:
    explicit AccountSwitchLine(QWidget *parent = 0);
    bool check() const;
    void setCheck(bool s);

signals:
    void checkChanged(bool s);

private:
    void setContent(QWidget *content);

private:
    DSwitchButton *m_button = NULL;
};



class AccountTypeLine : public AccountHeaderLine
{
    Q_OBJECT
public:
    explicit AccountTypeLine(QWidget *parent = 0);
    int currentIndex() const;
    void setType(int type);

signals:
    void typeChanged(int type);

private:
    DSegmentedControl *m_control = NULL;
};


class AccountConfirmButtonLine : public AccountHeaderLine
{
    Q_OBJECT
public:
    explicit AccountConfirmButtonLine(QWidget *parent = 0);

signals:
    void cancel();
    void confirm();
};


class AccountInputLine : public AccountHeaderLine
{
    Q_OBJECT
public:
    explicit AccountInputLine(QWidget *parent = 0);
    ~AccountInputLine();
    void hideWarning();
    void showWarning(const QString &msg);
    void setText(const QString &text);
    QString text() const;
    DLineEdit *lineEdit() const;

signals:
    void textChanged(const QString &text);
    void focusChanged(bool focus);

private:
    void setContent(QWidget *content);

private:
    InputWarningFrame *m_warningFrame = NULL;
    DLineEdit *m_lineEdit = NULL;
};

#endif // CONTROLLINE_H
