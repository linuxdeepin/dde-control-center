#ifndef SAFEUPDATEITEM_H
#define SAFEUPDATEITEM_H

#include "updatesettingitem.h"

class SafeUpdateItem : public UpdateSettingItem
{
    Q_OBJECT
public:
    explicit SafeUpdateItem(QWidget *parent = nullptr);

    void init();

    void setData(UpdateItemInfo *updateItemInfo) override;
};

#endif // SAFEUPDATEITEM_H
