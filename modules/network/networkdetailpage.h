#ifndef NETWORKDETAILPAGE_H
#define NETWORKDETAILPAGE_H

#include "contentwidget.h"

#include <QList>
#include <QJsonObject>

namespace dcc {

namespace network {

class NetworkModel;
class NetworkDetailPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit NetworkDetailPage(QWidget *parent = 0);

    void setModel(NetworkModel *model);

private slots:
    void onActiveInfoChanged(const QList<QJsonObject> &infos);

private:
    QVBoxLayout *m_groupsLayout;
};

}

}

#endif // NETWORKDETAILPAGE_H
