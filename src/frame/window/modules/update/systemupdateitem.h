#ifndef SYSTEMUPDATEITEM_H
#define SYSTEMUPDATEITEM_H

#include "updatesettingitem.h"

#include <qdbusreply.h>
#include <QDBusInterface>
#include <QDBusPendingCall>


namespace dcc {
namespace update {

class SystemUpdateItem: public UpdateSettingItem
{
    Q_OBJECT
public:
    explicit SystemUpdateItem(QWidget *parent = nullptr);
    void showMore() override;
    void setData(UpdateItemInfo *updateItemInfo) override;

private:
    QList<dcc::widgets::DetailInfoItem *> m_updateDetailItemList;
    DHorizontalLine *m_line;
    QWidget *m_lineWidget;

};
}
}
#endif // SYSTEMUPDATEITEM_H
