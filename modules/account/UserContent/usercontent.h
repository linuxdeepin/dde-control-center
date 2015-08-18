#ifndef ACCOUNTCONTENT_H
#define ACCOUNTCONTENT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QStackedWidget>
#include "dbus/dbusaccountuser.h"
#include "libdui/dsegmentedcontrol.h"
#include "historyavatarpanel.h"
#include "webcamavatarpanel.h"
#include "avatargrid.h"

DUI_USE_NAMESPACE

class UserContent : public QWidget
{
    Q_OBJECT
public:
    explicit UserContent(const QString &userPath, QWidget *parent = 0);

signals:
    void sizeChanged();

private:
    void initSegmentedControl();
    void initAvatarPanel();

private:
    QVBoxLayout *m_mainLayout = NULL;
    QStackedWidget *m_stackWidget = NULL;
    DBusAccountUser *m_accountUser = NULL;
    DSegmentedControl *m_segmentedControl = NULL;

    QString m_userPath = "";
};

#endif // ACCOUNTCONTENT_H
