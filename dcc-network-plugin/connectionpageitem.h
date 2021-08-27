#ifndef CONNECTIONPAGEITEM_H
#define CONNECTIONPAGEITEM_H

#include <DStandardItem>
#include <dtkwidget_global.h>

DWIDGET_BEGIN_NAMESPACE
class DSpinner;
class DViewItemAction;
class DListView;
DWIDGET_END_NAMESPACE

DWIDGET_USE_NAMESPACE

namespace dde {
  namespace network {
    enum class ConnectionStatus;
    class Connection;
  }
}

using namespace dde::network;

class ConnectionPageItem : public QObject, public DStandardItem
{
    Q_OBJECT

public:
    ConnectionPageItem(QWidget *widget, DListView *listView, dde::network::Connection *connection);
    ~ConnectionPageItem();

    void setItemData(void *data);
    void *itemData();

    void setConnectionStatus(const ConnectionStatus &status);

Q_SIGNALS:
    void detailClick(const dde::network::Connection *);
    void itemClick(const dde::network::Connection *);

private:
    void setLoading(const bool isLoading);

private:
    DSpinner *m_loadingIndicator;
    DListView *m_parentView;
    DViewItemAction *m_editAction;
    void *m_itemData;
    dde::network::Connection *m_connection;
};

#endif // INFODETAILITEM_H
