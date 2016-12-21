#ifndef PPPOEPAGE_H
#define PPPOEPAGE_H

#include <contentwidget.h>

namespace dcc {

namespace network {

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
};

}

}

#endif // PPPOEPAGE_H
