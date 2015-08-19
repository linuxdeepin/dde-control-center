#ifndef PASSWORDLINE_H
#define PASSWORDLINE_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include "libdui/dseparatorhorizontal.h"
#include "libdui/dpasswordedit.h"
#include "libdui/dtextbutton.h"
#include "libdui/dheaderline.h"
#include "libdui/dlineedit.h"
#include "libdui/dconstants.h"
#include <QDebug>

DUI_USE_NAMESPACE

class InputLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit InputLine(QWidget *parent = 0);
    QString text() const;

private:
    DPasswordEdit *m_edit = NULL;
};

class ButtonLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit ButtonLine(QWidget *parent = 0);

signals:
    void cancel();
    void apply();
};

class PasswordLine : public QStackedWidget
{
    Q_OBJECT
public:
    explicit PasswordLine(QWidget *parent = 0);

signals:
    void sizeChanged();
    void changePassword(QString password);

private:
    void initInactiveWidget();
    void initActiveWidget();

private:
    QWidget *m_inactiveWidget = NULL;
    DHeaderLine *m_activeWidget = NULL;
};

#endif // PASSWORDLINE_H
