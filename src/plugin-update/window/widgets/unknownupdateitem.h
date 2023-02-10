#ifndef UNKNOWNUPDATEITEM_H
#define UNKNOWNUPDATEITEM_H

#include "updatesettingitem.h"

class UnknownUpdateItem : public UpdateSettingItem
{
    Q_OBJECT
public:
    explicit UnknownUpdateItem(QWidget *parent = nullptr);
    void init();
    void setData(UpdateItemInfo *updateItemInfo) override;
};

#endif // UNKNOWNUPDATEITEM_H
