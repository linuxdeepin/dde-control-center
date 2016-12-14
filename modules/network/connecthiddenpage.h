#ifndef CONNECTHIDDENPAGE_H
#define CONNECTHIDDENPAGE_H

#include "contentwidget.h"

class QPushButton;

namespace dcc {

namespace widgets {

class LineEditWidget;
class NextPageWidget;

}

namespace network {

class ConnectHiddenPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ConnectHiddenPage(QWidget *parent = 0);

signals:
    void requestNextPage(ContentWidget * const w) const;

public slots:
    void onDeviceRemoved();

private:
    widgets::LineEditWidget *m_ssid;
    widgets::LineEditWidget *m_passwd;
    widgets::NextPageWidget *m_enctyptType;
    QPushButton *m_cancel;
    QPushButton *m_accept;
};

}

}

#endif // CONNECTHIDDENPAGE_H
