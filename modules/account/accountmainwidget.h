#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QFrame>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include "moduleheader.h"
#include "userlistpanel.h"
#include "libdui/dconstants.h"
#include "libdui/dthememanager.h"
#include "libdui/dseparatorhorizontal.h"

class AccountMainWidget : public QFrame
{
    Q_OBJECT
public:
    explicit AccountMainWidget(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    QScrollArea *m_listScrollArea = NULL;
    UserListPanel *m_listPanel = NULL;
    QVBoxLayout *m_mainLayout = NULL;
    ModuleHeader *m_header = NULL;
};

#endif // MAINWIDGET_H
