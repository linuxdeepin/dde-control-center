#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QFrame>
#include <QWidget>
#include <QVBoxLayout>
#include "moduleheader.h"
#include "accountlistpanel.h"
#include "libdui/dconstants.h"
#include "libdui/dseparatorhorizontal.h"

class MainWidget : public QFrame
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);

private:
    QVBoxLayout *m_mainLayout = NULL;
    ModuleHeader *m_header = NULL;
};

#endif // MAINWIDGET_H
