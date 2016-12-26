#ifndef PPPOEPAGE_H
#define PPPOEPAGE_H

#include "contentwidget.h"

#include <QMap>

class QVBoxLayout;

namespace dcc {

namespace widgets {
    class SettingsGroup;
}

namespace network {

class WiredDevice;
class NetworkModel;
class PppoePage : public ContentWidget
{
    Q_OBJECT
public:
    explicit PppoePage(QWidget *parent = 0);

    void setModel(NetworkModel *model);

signals:
    void requestCreatePppConnection() const;

private:
    NetworkModel *m_model;

    QVBoxLayout *m_grpsLayout;
    QMap<WiredDevice *, widgets::SettingsGroup *> m_grps;
};

}

}

#endif // PPPOEPAGE_H
