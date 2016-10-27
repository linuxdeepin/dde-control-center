#ifndef TIMEZONEHEAD_H
#define TIMEZONEHEAD_H

#include "settingsitem.h"
#include <QLabel>

namespace dcc {

class TimezoneHead : public SettingsItem
{
    Q_OBJECT

public:
    TimezoneHead(QFrame *parent = 0);

signals:
    void editChanged(bool edit);

public slots:
    void initStatus();

protected:
    void mouseMoveEvent(QMouseEvent* e);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QLabel *m_title;
    QLabel *m_edit;
};

}
#endif // TIMEZONEHEAD_H
