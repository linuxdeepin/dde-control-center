#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include "loadingindicator.h"
#include <dimagebutton.h>
#include <QFrame>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {
class ConnectWidget : public QFrame
{
    Q_OBJECT
public:
    explicit ConnectWidget(QWidget *parent = 0);
    enum ConnectState{
        Connected,
        Connecting,
        ConnectFaild
    };

signals:
    void clicked();

public slots:
    void onConnectChanged(ConnectState state);

protected:
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    ConnectState m_state;
    DImageButton *m_connectStateBtn;
    LoadingIndicator *m_loading;
};

}
}

#endif // CONNECTWIDGET_H
