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

    enum ConnectState {
        Connected,
        Connecting,
        ConnectFaild
    };

    enum MouseState {
        Enter,
        Leave
    };

signals:
    void clicked();

public slots:
    void onConnectChanged(ConnectState state);
    void onMouseState(MouseState state);

private:
    ConnectState m_state;
    DImageButton *m_disconnectBtn;
    LoadingIndicator *m_loading;
    DImageButton *m_connectedBtn;
};

}
}

#endif // CONNECTWIDGET_H
