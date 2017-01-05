#ifndef TITLEVALUEITEM_H
#define TITLEVALUEITEM_H

#include "settingsitem.h"
#include <QLabel>

namespace dcc
{

namespace widgets
{

class TitleValueItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit TitleValueItem(QFrame *parent = 0);
    void setTitle(const QString& title);
    void setValue(const QString& value);

private:
    QLabel* m_title;
    QLabel* m_value;
};

}

}
#endif // TITLEVALUEITEM_H
