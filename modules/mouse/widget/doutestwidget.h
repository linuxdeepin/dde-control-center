#ifndef DOUTESTWIDGET_H
#define DOUTESTWIDGET_H

#include "settingsitem.h"
#include <QLabel>
#include <QObject>
namespace dcc
{
namespace widgets
{
class SettingsItem;
}
namespace mouse
{
class DouTestWidget : public widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit DouTestWidget(QWidget *parent = 0);

signals:

public slots:
};

}
}


#endif // DOUTESTWIDGET_H
