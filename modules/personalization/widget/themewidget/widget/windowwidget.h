#ifndef WINDOWWIDGET_H
#define WINDOWWIDGET_H

#include "settingsitem.h"
#include <QObject>
using namespace dcc;
class windowWidget : public SettingsItem
{
    Q_OBJECT
public:
    explicit windowWidget(QObject *parent = 0);

signals:

public slots:


};

#endif // WINDOWWIDGET_H
